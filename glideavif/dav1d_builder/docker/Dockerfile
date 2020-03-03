FROM debian:buster

ADD https://dl.google.com/android/repository/android-ndk-r20b-linux-x86_64.zip /usr/local/src/android-ndk-r20-linux-x86_64.zip

RUN apt-get update \
    && apt-get install -y \
        cmake python3 python3-pip python3-setuptools python3-wheel ninja-build git nasm unzip

WORKDIR /usr/local/src

RUN unzip android-ndk-r20-linux-x86_64.zip && mv android-ndk-r20b android-ndk

RUN ln -s /usr/bin/python3 /usr/bin/python && \
    pip3 install --user --no-warn-script-location meson

## dav1d

RUN git clone -b master https://code.videolan.org/videolan/dav1d.git dav1d && \
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
RUN /root/.local/bin/meson .. \
    --cross-file dav1d_cross.txt \
    --default-library ${LIBTYPE} \
    --buildtype release && \
    ninja

ARG ARCH
RUN mkdir -p /opt/libdav1d/lib/${ARCH} /opt/libdav1d/include/${ARCH} && \
    cp src/libdav1d.* /opt/libdav1d/lib/${ARCH}/ && \
    cp -rf ../include/* /opt/libdav1d/include/${ARCH}/ && \
    find /opt/libdav1d/include/${ARCH}/ -type f -not -name "*.h" -exec rm {} + && \
    cp -rf include/* /opt/libdav1d/include/${ARCH}/

CMD ["cp", "-rf", "/opt/libdav1d", "/mnt/"]
