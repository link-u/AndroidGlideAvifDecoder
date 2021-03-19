cmake_minimum_required(VERSION 3.4.1)

project(avif_decoder CXX)

# ignore warning of shared textrel
string(REPLACE "-Wl,--fatal-warnings " "" NOWARNING ${CMAKE_SHARED_LINKER_FLAGS})
set(CMAKE_SHARED_LINKER_FLAGS ${NOWARNING})

set(DAV1D_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libdav1d/${FLAVOR_DEPTH})
set(DAV1D_INCLUDES ${DAV1D_DIR}/include/${ANDROID_ABI})
set(DAV1D_LIBS ${DAV1D_DIR}/lib/${ANDROID_ABI})
set(YUV_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YUV_INCLUDES ${YUV_DIR}/include)
set(YUV_LIBS ${YUV_DIR}/lib/${ANDROID_ABI})

add_library(dav1d STATIC IMPORTED)
set_target_properties(dav1d PROPERTIES IMPORTED_LOCATION ${DAV1D_LIBS}/libdav1d.a)
include_directories(${DAV1D_INCLUDES})

add_library(yuv STATIC IMPORTED)
set_target_properties(yuv PROPERTIES IMPORTED_LOCATION ${YUV_LIBS}/libyuv.a)
include_directories(${YUV_INCLUDES})

include_directories(jni_util/include)

# libavif
set(DAV1D_LIBRARY dav1d)
set(AVIF_CODEC_DAV1D ON CACHE BOOL "Use the dav1d codec for decoding (overrides AOM decoding if also enabled) " FORCE)
add_subdirectory(libavif EXCLUDE_FROM_ALL)

add_library(avif_decoder SHARED common.cpp avif_decoder.cpp my_bitmap.cpp avif_wrapper.cpp)

find_library(log-lib log)

target_link_libraries(
        avif_decoder
        android
        jnigraphics
        avif
        yuv
        ${log-lib}
)
