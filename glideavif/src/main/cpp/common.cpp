//
// Created by gc on 20/02/17.
//

#include "common.hpp"

void throwException(JNIEnv *env, const char *mes) {
    jni_util::throwRuntimeException(env, mes);
    throw std::runtime_error(mes);
}
