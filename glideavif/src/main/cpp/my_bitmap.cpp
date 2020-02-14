//
// Created by gc on 20/02/06.
//

#include <jni.h>

#include "my_bitmap.hpp"

template<const char *name>
jclass findClass(JNIEnv *env) {
    static jclass classID;
    if (classID == nullptr) {
        auto localClass = env->FindClass(name);
        classID = reinterpret_cast<jclass>(env->NewGlobalRef(localClass));
    }
    return classID;
}

template<const char *name, const char *sig>
jfieldID getStaticFieldID(JNIEnv *env, jclass classID) {
    static jfieldID fieldID;
    if (fieldID == nullptr) {
        fieldID = env->GetStaticFieldID(classID, name, sig);
    }
    return fieldID;
}

template<const char *name, const char *sig>
jmethodID getStaticMethodID(JNIEnv *env, jclass classID) {
    static jmethodID methodID;
    if (methodID == nullptr) {
        methodID = env->GetStaticMethodID(classID, name, sig);
    }
    return methodID;
}

char bitmapConfigClassPath[] = "android/graphics/Bitmap$Config";
char bitmapClassPath[] = "android/graphics/Bitmap";
char argb8888Name[] = "ARGB_8888";
char bitmapConfigSig[] = "Landroid/graphics/Bitmap$Config;";
char createBitmapName[] = "createBitmap";
char bitmapSig[] = "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;";

jobject createBitmap(JNIEnv *env, int width, int height) {
    auto bitmapConfigClass =
            findClass<bitmapConfigClassPath>(env);
    auto rgba8888FieldID =
            getStaticFieldID<argb8888Name, bitmapConfigSig>(env, bitmapConfigClass);
    auto rgba8888Obj =
            env->GetStaticObjectField(bitmapConfigClass, rgba8888FieldID);

    auto bitmapClass =
            findClass<bitmapClassPath>(env);
    auto createBitmapMethodID =
            getStaticMethodID<createBitmapName, bitmapSig>(env, bitmapClass);

    return env->CallStaticObjectMethod(bitmapClass, createBitmapMethodID, width, height,
                                       rgba8888Obj);
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
