#include <jni.h>
#include <string>
#include <avif/avif.h>
#include <android/log.h>

#include "my_bitmap.hpp"

#define TAG               "AvifDecoder"
#define LOGD(...)         __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

template<typename T, size_t depth>
void copyColorPixels(avifImage *im, std::vector<uint8_t> &rgbaList) {
    for (int k = 0; k < 3; ++k) {
        for (int i = 0; i < im->height; ++i) {
            auto p = (T *) (im->rgbPlanes[k] + (i * im->rgbRowBytes[k]));
            for (int j = 0; j < im->width; ++j) {
                rgbaList.at(4 * (i * im->width + j) + k) = (uint8_t) (p[j] >> (depth - 8));
            }
        }
    }
}

template<typename T, size_t depth>
void copyGrayscalePixels(avifImage *im, std::vector<uint8_t> &rgbaList) {
    for (int i = 0; i < im->height; ++i) {
        auto p = (T *) (im->yuvPlanes[0] + (i * im->yuvRowBytes[0]));
        for (int j = 0; j < im->width; ++j) {
            uint8_t c = p[j] >> (depth - 8);
            for (int k = 0; k < 3; ++k) {
                rgbaList.at(4 * (i * im->width + j) + k) = c;
            }
        }
    }
}

bool isGrayscale(avifImage *im) {
    return !(im->yuvRowBytes[1] && im->yuvRowBytes[2]);
}

class SourceData {
public:
    SourceData(JNIEnv *env, jbyteArray sourceData) {
        this->env = env;
        this->sourceData = sourceData;
        bytes = env->GetByteArrayElements(sourceData, nullptr);
    }

    ~SourceData() {
        env->ReleaseByteArrayElements(sourceData, bytes, JNI_ABORT);
    }

    jbyte *getBytes() const {
        return bytes;
    }

    SourceData(const SourceData &) = delete;

    SourceData &operator=(const SourceData &) = delete;

private:
    JNIEnv *env;
    jbyteArray sourceData;
    jbyte *bytes;
};

class AvifDecoder {
public:
    AvifDecoder() {
        decoder = avifDecoderCreate();
    }

    ~AvifDecoder() {
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

extern "C" JNIEXPORT jobject JNICALL
Java_jp_co_link_1u_library_glideavif_Avif_decodeAvif(
        JNIEnv *env,
        jobject,
        jbyteArray sourceData,
        int sourceDataLength
) {
    SourceData sd(env, sourceData);
    if (sd.getBytes() == nullptr) {
        LOGD("allocation failed");
        return nullptr;
    }

    avifROData raw;
    raw.data = (const uint8_t *) sd.getBytes();
    raw.size = (size_t) sourceDataLength;

    AvifDecoder ad;
    auto decoder = ad.getDecoder();
    auto result = avifDecoderParse(decoder, &raw);
    if (result != AVIF_RESULT_OK) {
        LOGD("parse failed");
        return nullptr;
    }

    result = avifDecoderNextImage(decoder);
    if (result != AVIF_RESULT_OK) {
        LOGD("decode failed");
        return nullptr;
    }

    auto im = decoder->image;
    std::vector<uint8_t> rgbaList(im->width * im->height * 4);

    if (isGrayscale(im)) {
        switch (im->depth) {
            case 8:
                copyGrayscalePixels<uint8_t, 8>(im, rgbaList);
                break;
            case 10:
                copyGrayscalePixels<uint16_t, 10>(im, rgbaList);
                break;
            case 12:
                copyGrayscalePixels<uint16_t, 12>(im, rgbaList);
                break;
            default:
                LOGD("unknown color depth");
                return nullptr;
        }
    } else {
        result = avifImageYUVToRGB(im);
        if (result != AVIF_RESULT_OK) {
            LOGD("convert yuv to rgb failed");
            return nullptr;
        }

        switch (im->depth) {
            case 8:
                copyColorPixels<uint8_t, 8>(im, rgbaList);
                break;
            case 10:
                copyColorPixels<uint16_t, 10>(im, rgbaList);
                break;
            case 12:
                copyColorPixels<uint16_t, 12>(im, rgbaList);
                break;
            default:
                LOGD("unknown color depth");
                return nullptr;
        }
    }

    if (im->alphaPlane) {
        for (int i = 0; i < im->height; ++i) {
            for (int j = 0; j < im->width; ++j) {
                uint8_t c = im->alphaPlane[i * im->alphaRowBytes + j];
                rgbaList.at(4 * (i * im->width + j) + 3) = c;
            }
        }
    } else {
        for (int i = 0; i < im->height; ++i) {
            for (int j = 0; j < im->width; ++j) {
                rgbaList.at(4 * (i * im->width + j) + 3) = 255U;
            }
        }
    }

    auto bitmap = new MyBitmap(env, im->width, im->height);
    bitmap->Load(rgbaList);
    return bitmap->Bitmap();
}
