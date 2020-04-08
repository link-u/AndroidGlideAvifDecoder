//
// Created by gc on 20/02/17.
//

#include "common.hpp"

void throwRuntimeException(JNIEnv *env, const char *what) {
    jclass classj = env->FindClass("java/lang/RuntimeException");
    if (classj == nullptr)
        return;

    env->ThrowNew(classj, what);
    env->DeleteLocalRef(classj);
}
