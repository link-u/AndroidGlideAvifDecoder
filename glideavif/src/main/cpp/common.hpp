#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <stdexcept>
#include <jni.h>
#include <android/log.h>
#include <jni_util.hpp>

#define TAG "AvifDecoder"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

void throwException(JNIEnv *env, const char *mes);

#endif
