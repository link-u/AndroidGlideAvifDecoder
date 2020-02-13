#include <jni.h>
#include <string>
#include <avif/avif.h>
#include <android/log.h>

#include "my_bitmap.hpp"

#define TAG               "AvifTest"
#define LOGD(...)         __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

extern "C" JNIEXPORT int JNICALL
Java_jp_co_link_1u_library_glideavif_Avif_decodeAvif(
        JNIEnv *env,
        jobject obj,
        jobject jbitmap,
        jbyteArray byteArray,
        int byteArrayLength
) {
    jbyte *bytes = env->GetByteArrayElements(byteArray, nullptr);

    avifROData raw;
    raw.data = (const uint8_t *) bytes;
    raw.size = (size_t) byteArrayLength;

    avifImage *im = avifImageCreateEmpty();
    avifDecoder *decoder = avifDecoderCreate();
    avifResult result = avifDecoderRead(decoder, im, &raw);

    if (result != AVIF_RESULT_OK) {
        LOGD("decode failed");
        return 1;
    }
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

    auto bitmap = new MyBitmap(env, jbitmap);
    bitmap->Load(rgbaList);
    return 0;
}

extern "C" JNIEXPORT jintArray JNICALL
Java_jp_co_link_1u_library_glideavif_Avif_decodeAvif2(
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

    avifImage *im = avifImageCreateEmpty();
    avifDecoder *decoder = avifDecoderCreate();
    avifResult result = avifDecoderRead(decoder, im, &raw);

    env->ReleaseByteArrayElements(sourceData, bytes, JNI_ABORT);

    if (result != AVIF_RESULT_OK) {
        LOGD("decode failed");
        return nullptr;
    }
    avifImageYUVToRGB(im);

    uint32_t bufferSize = 2 + im->width * im->height;
    jint *buffer0 = (jint *) malloc(bufferSize * sizeof(jint));
    if (buffer0 == nullptr) {
        LOGD("allocation failed");
        return nullptr;
    }

    buffer0[0] = im->width;
    buffer0[1] = im->height;

    auto buffer = buffer0 + 2;

    for (auto i = 0; i < im->height; ++i) {
        for (auto j = 0; j < im->width; ++j) {
            uint32_t c = 0xff000000;
            for (auto k = 0; k < 3; ++k) {
                uint8_t c0 = im->rgbPlanes[k][i * im->rgbRowBytes[k] + j * 2];
                uint8_t c1 = im->rgbPlanes[k][i * im->rgbRowBytes[k] + j * 2 + 1];
                uint32_t c01 = (c0 >> 2) | (c1 << 6);
                c |= c01 << (8 * (3 - k));
            }
            buffer[i * im->width + j] = c;
        }
    }

//    if (im->alphaPlane) {
//        for (int i = 0; i < im->height; ++i) {
//            for (int j = 0; j < im->width; ++j) {
//                uint32_t c = im->alphaPlane[i * im->alphaRowBytes + j];
//                buffer[i * im->width + j] &= c << 24;
//            }
//        }
//    }

    auto returnBuffer = env->NewIntArray(bufferSize);
    env->SetIntArrayRegion(returnBuffer, 0, bufferSize, buffer0);
    free(buffer0);
    return returnBuffer;
}
