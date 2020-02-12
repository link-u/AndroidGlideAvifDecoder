#include <jni.h>
#include <string>
#include <avif/avif.h>
#include <android/log.h>

#include "my_bitmap.hpp"

#define TAG               "AvifTest"
#define LOGD(...)         __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_sample1_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_sample1_MainActivity_testAvif(
        JNIEnv *env,
        jobject obj,
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
        return;
    }

    LOGD("size: %dx%d", im->width, im->height);
    LOGD("depth: %d", im->depth);
    LOGD("yuv format: %d", im->yuvFormat);
    LOGD("yuvRowBytes: %u %u %u", im->yuvRowBytes[0], im->yuvRowBytes[1], im->yuvRowBytes[2]);

    avifImageYUVToRGB(im);

    LOGD("rgbRowBytes: %u %u %u", im->rgbRowBytes[0], im->rgbRowBytes[1], im->rgbRowBytes[2]);

    avifImageDestroy(im);
    avifDecoderDestroy(decoder);
}

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
