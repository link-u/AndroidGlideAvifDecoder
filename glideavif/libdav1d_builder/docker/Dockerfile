FROM debian:buster

RUN apt-get update \
    && apt-get install -y \
        cmake curl python3 python3-pip python3-setuptools python3-wheel ninja-build git nasm unzip

WORKDIR /usr/local/src

ARG NDK
RUN curl -SL https://dl.google.com/android/repository/android-ndk-${NDK}-linux-x86_64.zip > android-ndk-${NDK}-linux-x86_64.zip && \
    unzip -q android-ndk-${NDK}-linux-x86_64.zip && \
    mv android-ndk-${NDK} android-ndk && \
    rm android-ndk-${NDK}-linux-x86_64.zip

RUN ln -s /usr/bin/python3 /usr/bin/python && \
    pip3 install --user --no-warn-script-location meson

## dav1d

ARG DAV1D_VERSION
RUN git clone -b ${DAV1D_VERSION} https://code.videolan.org/videolan/dav1d.git dav1d && \
    mkdir /usr/local/src/dav1d/build

WORKDIR /usr/local/src/dav1d/build

COPY dav1d_cross.txt ./dav1d_cross.txt

ARG CLANG
ARG UTIL
ARG CPU_FAMILY
ARG CPU
ARG API_LEVEL
RUN sed -i  \
    -e "s/%CLANG/${CLANG}/" \
    -e "s/%UTIL/${UTIL}/" \
    -e "s/%CPU_FAMILY/${CPU_FAMILY}/" \
    -e "s/%CPU/${CPU}/" \
    -e "s/%API_LEVEL/${API_LEVEL}/" \
    -e 's/%TOOLCHAIN_ROOT/\/usr\/local\/src\/android-ndk\/toolchains\/llvm\/prebuilt\/linux-x86_64/' dav1d_cross.txt

ARG LIBTYPE
ARG DEPTH
RUN /root/.local/bin/meson .. \
    "-Dbitdepths=${DEPTH}" \
    --cross-file dav1d_cross.txt \
    --default-library ${LIBTYPE} \
    --buildtype release && \
    ninja

ARG ARCH
ARG DEPTH_NAME
RUN mkdir -p /opt/libdav1d/${DEPTH_NAME}/lib/${ARCH} /opt/libdav1d/${DEPTH_NAME}/include/${ARCH} && \
    cp src/libdav1d.a /opt/libdav1d/${DEPTH_NAME}/lib/${ARCH}/ && \
    cp -rf ../include/* /opt/libdav1d/${DEPTH_NAME}/include/${ARCH}/ && \
    find /opt/libdav1d/${DEPTH_NAME}/include/${ARCH}/ -type f -not -name "*.h" -exec rm {} + && \
    cp -rf include/* /opt/libdav1d/${DEPTH_NAME}/include/${ARCH}/

CMD ["cp", "-rf", "/opt/libdav1d", "/mnt/"]
