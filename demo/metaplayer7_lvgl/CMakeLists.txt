cmake_minimum_required(VERSION 2.8)
project(metaplayer7_lvgl)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=gnu11 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=gnu++11 ")
set(HOME_BASE_REL "../../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)
set(Yang_3RD_DIR ${HOME_BASE}/thirdparty/lib/)
set(Yang_Lib_DIR ${HOME_BASE}/bin/lib_debug)
# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/user_include)
include_directories(${HOME_BASE}/thirdparty/user_include/ffmpeg)
include_directories(${HOME_BASE}/libmetartc7/src)
include_directories(${HOME_BASE}/demo/metaplayer7_lvgl/yangplayer)
include_directories(${HOME_BASE}/demo/metaplayer7_lvgl/liblvgl)

aux_source_directory(./ DIR_SRCS)
aux_source_directory(./yangplayer DIR_SRCS)

add_executable(metaplayer7_lvgl ${DIR_SRCS})

target_link_libraries(metaplayer7_lvgl  ${Yang_Lib_DIR}/liblvgl.a
		${Yang_Lib_DIR}/libmetartc7.a  ${Yang_Lib_DIR}/libmetartccore7.a
        ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl2.a ${Yang_3RD_DIR}/libcrypto2.a
        ${Yang_3RD_DIR}/libspeexdsp.a ${Yang_3RD_DIR}/libyuv.a ${Yang_3RD_DIR}/libusrsctp.a
        ${Yang_3RD_DIR}/libyangh264decoder.a ${Yang_3RD_DIR}/libopus.a
         SDL2 asound pthread dl    
        )


