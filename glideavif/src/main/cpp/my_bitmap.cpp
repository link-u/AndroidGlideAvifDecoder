//
// Created by gc on 20/02/06.
//

#include <jni.h>

#include "my_bitmap.hpp"

MyBitmap::MyBitmap(JNIEnv *env, jobject jbitmap) {
    this->env = env;
    this->jbitmap = jbitmap;

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
