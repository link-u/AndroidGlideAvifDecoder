#include <jni.h>
#include <string>
#include <avif/avif.h>
#include <android/log.h>

#include "my_bitmap.hpp"

#define TAG               "AvifTest"
#define LOGD(...)         __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

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
        LOGD("decode failed");
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
    for (int i = 0; i < im->height; ++i) {
        for (int j = 0; j < im->width; ++j) {
            for (int k = 0; k < 3; ++k) {
                uint8_t c0 = im->rgbPlanes[k][i * im->rgbRowBytes[k] + j * 2];
                uint8_t c1 = im->rgbPlanes[k][i * im->rgbRowBytes[k] + j * 2 + 1];
                rgbaList.at(4 * (i * im->width + j) + k) = (c0 >> 2) | (c1 << 6);
            }
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
                rgbaList.at(4 * (i * im->width + j) + 3) = 255;
            }
        }
    }

    avifDecoderDestroy(decoder);
    env->ReleaseByteArrayElements(sourceData, bytes, JNI_ABORT);

    auto bitmap = new MyBitmap(env, im->width, im->height);
    bitmap->Load(rgbaList);
    return bitmap->Bitmap();
}
