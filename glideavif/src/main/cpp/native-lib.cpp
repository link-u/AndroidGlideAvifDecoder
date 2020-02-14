#include <jni.h>
#include <string>
#include <avif/avif.h>
#include <android/log.h>

#include "my_bitmap.hpp"

#define TAG               "AvifDecoder"
#define LOGD(...)         __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

template<typename T, size_t depth>
void copyPixels(avifImage *im, std::vector<uint8_t> &rgbaList) {
    for (int k = 0; k < 3; ++k) {
        for (int i = 0; i < im->height; ++i) {
            auto p = (T *) (im->rgbPlanes[k] + (i * im->rgbRowBytes[k]));
            for (int j = 0; j < im->width; ++j) {
                rgbaList.at(4 * (i * im->width + j) + k) = (uint8_t) (p[j] >> (depth - 8));
            }
        }
    }
}

extern "C" JNIEXPORT jobject JNICALL
Java_jp_co_link_1u_library_glideavif_Avif_decodeAvif(
        JNIEnv *env,
        jobject,
        jbyteArray sourceData,
        int sourceDataLength
) {
    jbyte *bytes = env->GetByteArrayElements(sourceData, nullptr);
    if (bytes == nullptr) {
        LOGD("allocation failed");
        return nullptr;
    }

    avifROData raw;
    raw.data = (const uint8_t *) bytes;
    raw.size = (size_t) sourceDataLength;

    avifDecoder *decoder = avifDecoderCreate();
    avifResult result = avifDecoderParse(decoder, &raw);
    if (result != AVIF_RESULT_OK) {
        LOGD("parse failed");
        return nullptr;
    }

    result = avifDecoderNextImage(decoder);
    if (result != AVIF_RESULT_OK) {
        LOGD("decode failed");
        return nullptr;
    }

    avifImage *im = decoder->image;
    avifImageYUVToRGB(im);

    std::vector<uint8_t> rgbaList(im->width * im->height * 4);

    switch (im->depth) {
        case 8:
            copyPixels<uint8_t, 8>(im, rgbaList);
            break;
        case 10:
            copyPixels<uint16_t, 10>(im, rgbaList);
            break;
        case 12:
            copyPixels<uint16_t, 12>(im, rgbaList);
            break;
        default:
            LOGD("unknown color depth");
            return nullptr;
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

    avifDecoderDestroy(decoder);
    env->ReleaseByteArrayElements(sourceData, bytes, JNI_ABORT);

    auto bitmap = new MyBitmap(env, im->width, im->height);
    bitmap->Load(rgbaList);
    return bitmap->Bitmap();
}
