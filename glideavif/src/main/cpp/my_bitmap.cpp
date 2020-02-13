//
// Created by gc on 20/02/06.
//

#include <jni.h>

#include "my_bitmap.hpp"

jobject createBitmap(JNIEnv *env, int width, int height) {
    jclass bitmapConfig = env->FindClass("android/graphics/Bitmap$Config");
    jfieldID rgba8888FieldID = env->GetStaticFieldID(bitmapConfig, "ARGB_8888",
                                                     "Landroid/graphics/Bitmap$Config;");
    jobject rgba8888Obj = env->GetStaticObjectField(bitmapConfig, rgba8888FieldID);

    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMethodID = env->GetStaticMethodID(bitmapClass, "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject bitmapObj = env->CallStaticObjectMethod(bitmapClass, createBitmapMethodID, width,
                                                    height, rgba8888Obj);
    return bitmapObj;
}

MyBitmap::MyBitmap(JNIEnv *env, int width, int height) {
    this->env = env;
    this->jbitmap = createBitmap(env, width, height);

    if (AndroidBitmap_getInfo(env, jbitmap, &this->info) < 0) {
        // throw
    }

    if (this->info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        // throw
    }
}

void MyBitmap::Load(const std::vector<uint8_t> &rgbaList) {
    if (info.stride * info.height != rgbaList.size()) {
        // throw
    }

    void *ptr;
    if (AndroidBitmap_lockPixels(env, jbitmap, &ptr) < 0) {
        // throw
    }

    memcpy(ptr, (void *) rgbaList.data(), rgbaList.size());

    if (AndroidBitmap_unlockPixels(env, jbitmap) < 0) {
        // throw
    }
}
