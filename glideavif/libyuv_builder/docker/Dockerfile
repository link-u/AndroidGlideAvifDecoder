FROM debian:buster

ADD https://dl.google.com/android/repository/android-ndk-r20b-linux-x86_64.zip /usr/local/src/android-ndk-r20-linux-x86_64.zip

RUN apt-get update \
    && apt-get install -y \
        cmake ninja-build git nasm unzip

WORKDIR /usr/local/src

RUN unzip android-ndk-r20-linux-x86_64.zip && mv android-ndk-r20b android-ndk

## libyuv

RUN git clone -b master https://chromium.googlesource.com/libyuv/libyuv libyuv && \
    mkdir -p /usr/local/src/libyuv/build

WORKDIR /usr/local/src/libyuv/build

ARG ARCH
ARG API_LEVEL
RUN cmake \
    -DCMAKE_TOOLCHAIN_FILE=/usr/local/src/android-ndk/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$ARCH \
    -DANDROID_NATIVE_API_LEVEL=$API_LEVEL \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -GNinja \
    .. && ninja

RUN mkdir -p /opt/libyuv/lib/${ARCH} /opt/libyuv/include/${ARCH} && \
    cp libyuv.a /opt/libyuv/lib/${ARCH}/ && \
    cp -rf ../include/* /opt/libyuv/include/${ARCH}/

CMD ["cp", "-rf", "/opt/libyuv", "/mnt/"]
