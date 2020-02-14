#ifndef __MY_BITMAP_HPP__
#define __MY_BITMAP_HPP__

#include <vector>
#include <jni.h>
#include <android/bitmap.h>
#include <string>

class MyBitmap {
public:
    MyBitmap(JNIEnv *env, int width, int height);

    void Load(const std::vector<uint8_t> &rgbaList);

    jobject Bitmap() { return jbitmap; }

private:
    JNIEnv *env;
    jobject jbitmap;
    AndroidBitmapInfo info;

public:
    class MyException : std::exception {
    public:
        MyException(const char *message) : std::exception(), message(message) {}

        const char *getMessage() const {
            return message;
        };

    private:
        const char *message;
    };
};

#endif
