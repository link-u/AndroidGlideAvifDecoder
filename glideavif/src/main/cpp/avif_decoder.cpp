#include <jni.h>
#include <string>
#include <avif/avif.h>
#include <libyuv/convert_argb.h>
#include <algorithm>

#include "common.hpp"
#include "my_bitmap.hpp"
#include "avif_wrapper.hpp"

template<typename T>
void copyGrayscalePixels(avifImage *im, std::vector<uint8_t> &rgbaList) {
    uint32_t w = im->width * sizeof(T) / sizeof(uint8_t);
    if (im->yuvRowBytes[0] < w) {
        throw std::runtime_error("invalid yuv bytes");
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

template<typename T>
void
copyAlphaPixels(avifImage *im, bool limitedRange, std::vector<uint8_t> &rgbaList) {
    uint32_t w = im->width * sizeof(T) / sizeof(uint8_t);
    if (im->alphaRowBytes < w) {
        throw std::runtime_error("invalid alpha bytes");
    }

    for (int i = 0; i < im->height; ++i) {
        auto p = (T *) (im->alphaPlane + (i * im->alphaRowBytes));
        for (int j = 0; j < im->width; ++j) {
            uint32_t c0 = p[j] >> (im->depth - 8);
            if (limitedRange) {
                c0 = std::max<uint32_t>(std::min<uint32_t>((c0 - 16) * 255 / (235 - 16), 255), 0);
            }
            rgbaList.at(4 * (i * im->width + j) + 3) = (uint8_t) c0;
        }
    }
}

inline bool isGrayscale(avifImage *im) {
    return !(im->yuvRowBytes[1] && im->yuvRowBytes[2]);
}

int (*getYUVConvertFunc(avifImage *im))
        (const uint8_t *src_y, int src_stride_y,
         const uint8_t *src_u, int src_stride_u,
         const uint8_t *src_v, int src_stride_v,
         uint8_t *dst_abgr, int dst_stride_abgr,
         int width, int height) {
    switch (im->nclx.matrixCoefficients) {
        // See: H.273
        case AVIF_NCLX_MATRIX_COEFFICIENTS_BT470BG:
        case AVIF_NCLX_MATRIX_COEFFICIENTS_BT601:
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

jobject decodeAvif(JNIEnv *env, const uint8_t *sourceData, int sourceDataLength) {
    avifROData raw{};
    raw.data = sourceData;
    raw.size = (size_t) sourceDataLength;

    avifDecoderPtr decoder(avifDecoderCreate());
    auto result = avifDecoderParse(decoder.get(), &raw);
    if (result != AVIF_RESULT_OK) {
        throw std::runtime_error("avifDecoderParse failed");
    }

    result = avifDecoderNextImage(decoder.get());
    if (result != AVIF_RESULT_OK) {
        throw std::runtime_error("avifDecoderNextImage failed");
    }

    avifImage *im = decoder->image;
    std::vector<uint8_t> rgbaList(im->width * im->height * 4);

    [&] {
        if (im->nclx.range == AVIF_RANGE_FULL && isGrayscale(im)) {
            // fullRangeかつGrayscaleの時だけは、Yの値をそのままコピーしてよい
            // FIXME(ledyba-z):
            //  transferCharacteristicsとmatrixCoefficientsの組み合わせによっては駄目な可能性がある
            switch (im->depth) {
                case 8:
                    copyGrayscalePixels<uint8_t>(im, rgbaList);
                    return;
                case 10:
                case 12:
                    copyGrayscalePixels<uint16_t>(im, rgbaList);
                    return;
                default: {
                    throw std::runtime_error("unknown color depth");
                }
            }
        }

        if (!im->nclx.range == AVIF_RANGE_FULL && im->depth == 8) {
            // libyuv は8bitかつlimited rangeにのみ対応
            if (isGrayscale(im)) {
                auto result1 = libyuv::I400ToARGB(
                        im->yuvPlanes[0], im->yuvRowBytes[0],
                        rgbaList.data(), im->width * 4,
                        im->width, im->height);
                if (result1 != 0) {
                    throw std::runtime_error("convert yuv to rgb with libyuv failed");
                }
                return;
            }

            auto convertYUV = getYUVConvertFunc(im);
            if (convertYUV) {
                auto result1 = convertYUV(
                        im->yuvPlanes[0], im->yuvRowBytes[0],
                        im->yuvPlanes[1], im->yuvRowBytes[1],
                        im->yuvPlanes[2], im->yuvRowBytes[2],
                        rgbaList.data(), im->width * 4,
                        im->width, im->height);
                if (result1 != 0) {
                    throw std::runtime_error("convert 8bpc limited yuv to rgb with libyuv failed");
                }
                return;
            }
        }

        auto rgb = avifRGBImagePtr(new avifRGBImage);
        avifRGBImageSetDefaults(rgb.get(), im);
        rgb->format = AVIF_RGB_FORMAT_RGBA;
        rgb->depth = 8;

        avifRGBImageAllocatePixels(rgb.get());
        avifImageYUVToRGB(im, rgb.get());
        if (rgbaList.size() != rgb->rowBytes * rgb->height) {
            throw std::runtime_error("invalid size");
        }
        memcpy(rgbaList.data(), rgb->pixels, rgb->rowBytes * rgb->height);

        // On non-alpha image, A of RGBA is decoded 0x00
        if (!im->alphaPlane) {
            for (int i = 0; i < im->height; ++i) {
                for (int j = 0; j < im->width; ++j) {
                    rgbaList.at(4 * (i * im->width + j) + 3) = 0xff;
                }
            }
        }
        return;
    }();

    if (im->alphaPlane) {
        bool limitedRange = false;
        switch (im->alphaRange) {
            case AVIF_RANGE_LIMITED:
                limitedRange = true;
                break;
            case AVIF_RANGE_FULL:
                limitedRange = false;
                break;
        }
        switch (im->depth) {
            case 8:
                copyAlphaPixels<uint8_t>(im, limitedRange, rgbaList);
                break;
            case 10:
            case 12:
                copyAlphaPixels<uint16_t>(im, limitedRange, rgbaList);
                break;
            default: {
                throw std::runtime_error("unknown color depth");
            }
        }
    }

    MyBitmap bitmap(env, im->width, im->height);
    bitmap.Load(rgbaList);
    return bitmap.Bitmap();
}

extern "C" JNIEXPORT jobject JNICALL
Java_jp_co_link_1u_library_glideavif_AvifDecoderFromByteBuffer_decodeAvif(
        JNIEnv *env,
        jobject,
        jobject sourceData,
        int sourceDataLength
) {
    try {
        auto buffer = env->GetDirectBufferAddress(sourceData);
        if (buffer == nullptr) {
            throw std::runtime_error("buffer must be DirectByteBuffer");
        }

        return decodeAvif(env, (const uint8_t *) buffer, sourceDataLength);
    }
    catch (const std::exception &e) {
        throwRuntimeException(env, e.what());
        return nullptr;
    }
    catch (...) {
        throwRuntimeException(env, "unknown error occurred");
        return nullptr;
    }
}
