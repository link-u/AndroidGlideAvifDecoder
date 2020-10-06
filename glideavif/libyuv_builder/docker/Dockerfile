FROM debian:buster

RUN apt-get update && \
    apt-get install -y \
        cmake curl ninja-build git nasm unzip

WORKDIR /usr/local/src

ARG NDK
RUN curl -SL https://dl.google.com/android/repository/android-ndk-${NDK}-linux-x86_64.zip > android-ndk-${NDK}-linux-x86_64.zip && \
    unzip -q android-ndk-${NDK}-linux-x86_64.zip && \
    mv android-ndk-${NDK} android-ndk && \
    rm android-ndk-${NDK}-linux-x86_64.zip

## libyuv
ARG YUV_VERSION

RUN git clone https://chromium.googlesource.com/libyuv/libyuv libyuv && \
    cd libyuv && \
    git reset --hard ${YUV_VERSION} && \
    mkdir -p /usr/local/src/libyuv/build

WORKDIR /usr/local/src/libyuv/build

ARG ARCH
ARG API_LEVEL
RUN cmake \
    -DCMAKE_TOOLCHAIN_FILE=/usr/local/src/android-ndk/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$ARCH \
    -DANDROID_NATIVE_API_LEVEL=$API_LEVEL \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -GNinja \
    .. && ninja

RUN mkdir -p /opt/libyuv/lib/${ARCH} /opt/libyuv/include/${ARCH} && \
    cp libyuv.a /opt/libyuv/lib/${ARCH}/ && \
    cp -rf ../include/* /opt/libyuv/include/${ARCH}/

CMD ["cp", "-rf", "/opt/libyuv", "/mnt/"]
