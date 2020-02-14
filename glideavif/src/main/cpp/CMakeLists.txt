cmake_minimum_required(VERSION 3.4.1)

add_library(avif_decoder SHARED avif_decoder.cpp my_bitmap.cpp)

find_library(log-lib log)

add_library(dav1d SHARED IMPORTED)
set_target_properties(dav1d PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${ANDROID_ABI}/libdav1d.so)

include_directories(include)

# libavif
set(AVIF_CODEC_DAV1D ON CACHE BOOL "Use the dav1d codec for decoding (overrides AOM decoding if also enabled)" FORCE)
add_subdirectory(libavif EXCLUDE_FROM_ALL)

target_link_libraries(
        avif_decoder
        android
        jnigraphics
        dav1d
        avif
        ${log-lib}
)
