#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <stdexcept>
#include <jni.h>
#include <jni_util.hpp>

#define TAG "AvifDecoder"
#define LOGD(...) android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

void throwException(JNIEnv *env, const char *mes);

#endif
