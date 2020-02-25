cmake_minimum_required(VERSION 3.4.1)

add_library(avif_decoder SHARED common.cpp avif_decoder.cpp my_bitmap.cpp)

find_library(log-lib log)

set(DAV1D_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(DAV1D_INCLUDES ${DAV1D_DIR}/include)
set(DAV1D_LIBS ${DAV1D_DIR}/lib/${ANDROID_ABI})

add_library(dav1d SHARED IMPORTED)
set_target_properties(dav1d PROPERTIES IMPORTED_LOCATION ${DAV1D_LIBS}/libdav1d.so)
include_directories(${DAV1D_INCLUDES})
set(DAV1D_LIBRARY dav1d)

include_directories(jni_util/include)

# libavif
set(AVIF_CODEC_DAV1D ON CACHE BOOL "Use the dav1d codec for decoding (overrides AOM decoding if also enabled)" FORCE)
add_subdirectory(libavif EXCLUDE_FROM_ALL)

add_subdirectory(libyuv EXCLUDE_FROM_ALL)
include_directories(libyuv/include)

target_link_libraries(
        avif_decoder
        android
        jnigraphics
        dav1d
        avif
        yuv
        ${log-lib}
)
