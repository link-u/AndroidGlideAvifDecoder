#include <jni.h>
#include <string>
#include <avif/avif.h>
#include <jni_util.hpp>
#include <libyuv/convert_argb.h>

#include "common.hpp"
#include "my_bitmap.hpp"

template<typename T>
void copyColorPixels(JNIEnv *env, avifImage *im, std::vector<uint8_t> &rgbaList) {
    uint32_t w = im->width * sizeof(T) / sizeof(uint8_t);
    if (im->rgbRowBytes[0] != w || im->rgbRowBytes[1] != w || im->rgbRowBytes[2] != w) {
        throwException(env, "invalid rgb bytes");
    }

    for (int k = 0; k < 3; ++k) {
        for (int i = 0; i < im->height; ++i) {
            auto p = (T *) (im->rgbPlanes[k] + (i * im->rgbRowBytes[k]));
            for (int j = 0; j < im->width; ++j) {
                rgbaList.at(4 * (i * im->width + j) + k) = (uint8_t) (p[j] >> (im->depth - 8));
            }
        }
    }

    for (int i = 0; i < im->height; ++i) {
        for (int j = 0; j < im->width; ++j) {
            rgbaList.at(4 * (i * im->width + j) + 3) = 0xff;
        }
    }
}

template<typename T>
void copyGrayscalePixels(JNIEnv *env, avifImage *im, std::vector<uint8_t> &rgbaList) {
    uint32_t w = im->width * sizeof(T) / sizeof(uint8_t);
    if (im->yuvRowBytes[0] < w) {
        throwException(env, "invalid yuv bytes");
    }

    auto array = (uint32_t *) rgbaList.data();
    for (int i = 0; i < im->height; ++i) {
        auto p = (T *) (im->yuvPlanes[0] + (i * im->yuvRowBytes[0]));
        for (int j = 0; j < im->width; ++j) {
            uint32_t c0 = p[j] >> (im->depth - 8);
            array[i * im->width + j] = 0xff000000 | (c0 << 16U) | (c0 << 8U) | c0;
        }
    }
}

bool isGrayscale(avifImage *im) {
    return !(im->yuvRowBytes[1] && im->yuvRowBytes[2]);
}

class AvifDecoder {
public:
    AvifDecoder() {
        decoder = avifDecoderCreate();
    }

    ~AvifDecoder() noexcept {
        avifDecoderDestroy(decoder);
    }

    avifDecoder *getDecoder() {
        return decoder;
    }

    AvifDecoder(const AvifDecoder &) = delete;

    AvifDecoder &operator=(const AvifDecoder &) = delete;

private:
    avifDecoder *decoder;
};

int (*getYUVConvertFunc(avifImage *im))
        (const uint8_t *src_y, int src_stride_y,
         const uint8_t *src_u, int src_stride_u,
         const uint8_t *src_v, int src_stride_v,
         uint8_t *dst_abgr, int dst_stride_abgr,
         int width, int height) {
    switch (im->nclx.matrixCoefficients) {
        // BT601_7_625/BT601_7_525でKrとKbは同じ（See: H.273）
        case AVIF_NCLX_MATRIX_COEFFICIENTS_BT601_7_625:
        case AVIF_NCLX_MATRIX_COEFFICIENTS_BT601_7_525:
            switch (im->yuvFormat) {
                case AVIF_PIXEL_FORMAT_YUV420:
                    return libyuv::I420ToABGR;
                case AVIF_PIXEL_FORMAT_YUV422:
                    return libyuv::I422ToABGR;
                case AVIF_PIXEL_FORMAT_YUV444:
                    return libyuv::I444ToABGR;
                default:
                    return nullptr;
            }
            break;
        case AVIF_NCLX_MATRIX_COEFFICIENTS_BT709:
            switch (im->yuvFormat) {
                case AVIF_PIXEL_FORMAT_YUV420:
                    return libyuv::H420ToABGR;
                case AVIF_PIXEL_FORMAT_YUV422:
                    return libyuv::H422ToABGR;
                case AVIF_PIXEL_FORMAT_YUV444:
                    return libyuv::H444ToABGR;
                default:
                    return nullptr;
            }
            break;
       case AVIF_NCLX_MATRIX_COEFFICIENTS_BT2020_NCL:
           switch (im->yuvFormat) {
               case AVIF_PIXEL_FORMAT_YUV420:
                   return libyuv::U420ToABGR;
               case AVIF_PIXEL_FORMAT_YUV422:
                   return libyuv::U422ToABGR;
               case AVIF_PIXEL_FORMAT_YUV444:
                   return libyuv::U444ToABGR;
               default:
                   return nullptr;
           }
           break;
        default:
            // 他のColorMatrixはlibyuvは現在非対応
            return nullptr;
    }
}
void convert8bitLimitedYUVTORGB(JNIEnv *env, avifImage *im, std::vector<uint8_t> &rgbaList) {
    if (isGrayscale(im)) {
        auto result = libyuv::I400ToARGB(im->yuvPlanes[0], im->yuvRowBytes[0],
                                         rgbaList.data(), im->width * 4,
                                         im->width, im->height);
        if (result != 0) {
            throwException(env, "convert yuv to rgb with libyuv failed");
        }
        return;
    }
    auto convertYUV = getYUVConvertFunc(im);
    if (convertYUV) {
        auto result = convertYUV(
                im->yuvPlanes[0], im->yuvRowBytes[0],
                im->yuvPlanes[1], im->yuvRowBytes[1],
                im->yuvPlanes[2], im->yuvRowBytes[2],
                rgbaList.data(), im->width * 4,
                im->width, im->height);
        if (result != 0) {
            throwException(env, "convert 8bpc limited yuv to rgb with libyuv failed");
        }
    } else {
        auto result = avifImageYUVToRGB(im);
        if (result != AVIF_RESULT_OK) {
            throwException(env, "convert 8bpc limited yuv to rgb with libavif failed");
        }

        copyColorPixels<uint8_t>(env, im, rgbaList);
    }
}

jobject decodeAvif(JNIEnv *env, jbyteArray sourceData, int sourceDataLength) {
    jni_util::CopiedArrayAccess<jbyte> source(env, sourceData, JNI_ABORT);
    if (source.elements() == nullptr) {
        throwException(env, "allocation failed");
    }

    avifROData raw;
    raw.data = (const uint8_t *) source.elements();
    raw.size = (size_t) sourceDataLength;

    AvifDecoder ad;
    auto decoder = ad.getDecoder();
    auto result = avifDecoderParse(decoder, &raw);
    if (result != AVIF_RESULT_OK) {
        throwException(env, "parse failed");
    }

    result = avifDecoderNextImage(decoder);
    if (result != AVIF_RESULT_OK) {
        throwException(env, "decode failed");
    }

    auto im = decoder->image;
    std::vector<uint8_t> rgbaList(im->width * im->height * 4);

    if(im->nclx.fullRangeFlag) {
        if (isGrayscale(im)) {
            // fullRangeかつGrayscaleの時だけは、Yの値をそのままコピーしてよい
            // FIXME(ledyba-z):
            //  transferCharacteristicsとmatrixCoefficientsの組み合わせによっては駄目な可能性がある
            switch (im->depth) {
                case 8:
                    copyGrayscalePixels<uint8_t>(env, im, rgbaList);
                    break;
                case 10:
                case 12:
                    copyGrayscalePixels<uint16_t>(env, im, rgbaList);
                    break;
                default:
                    throwException(env, "unknown color depth");
            }
        } else {
            result = avifImageYUVToRGB(im);
            if (result != AVIF_RESULT_OK) {
                throwException(env, "convert 8bpc full yuv to rgb with libavif failed");
            }

            copyColorPixels<uint16_t>(env, im, rgbaList);
        }
    } else {
        switch (im->depth) {
            case 8:
                convert8bitLimitedYUVTORGB(env, im, rgbaList);
                break;
            case 10:
            case 12:
                result = avifImageYUVToRGB(im);
                if (result != AVIF_RESULT_OK) {
                    throwException(env, "convert high bit-depth limited yuv to rgb with libavif failed");
                }

                copyColorPixels<uint16_t>(env, im, rgbaList);
                break;
            default:
                throwException(env, "unknown color depth");
        }
    }

    if (im->alphaPlane) {
        for (int i = 0; i < im->height; ++i) {
            for (int j = 0; j < im->width; ++j) {
                uint8_t c = im->alphaPlane[i * im->alphaRowBytes + j];
                rgbaList.at(4 * (i * im->width + j) + 3) = c;
            }
        }
    }

    MyBitmap bitmap(env, im->width, im->height);
    bitmap.Load(rgbaList);
    return bitmap.Bitmap();
}

extern "C" JNIEXPORT jobject JNICALL
Java_jp_co_link_1u_library_glideavif_Avif_decodeAvif(
        JNIEnv *env,
        jobject,
        jbyteArray sourceData,
        int sourceDataLength
) {
    try {
        return decodeAvif(env, sourceData, sourceDataLength);
    }
    catch (const std::exception &e) {
        return nullptr;
    }
}
