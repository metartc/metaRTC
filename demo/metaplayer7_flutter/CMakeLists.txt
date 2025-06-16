cmake_minimum_required(VERSION 2.8)
project(metaplayer7)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=gnu11 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=gnu++11 ")
set(HOME_BASE_REL "../../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)
set(Yang_3RD_DIR ${HOME_BASE}/thirdparty/lib/pic/)
set(Yang_Lib_DIR ${HOME_BASE}/bin/lib_debug)
# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/user_include)
include_directories(${HOME_BASE}/thirdparty/user_include/ffmpeg)
include_directories(${HOME_BASE}/libmetartc7/src)
include_directories(${HOME_BASE}/demo/libmetaApp/src)


aux_source_directory(./metaplayer7 DIR_SRCS)

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(metaplayer7 SHARED ${DIR_SRCS})

target_link_libraries(metaplayer7  
		${Yang_Lib_DIR}/libmetartc7.a  ${Yang_Lib_DIR}/libmetartccore7.a
        ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl2.a ${Yang_3RD_DIR}/libcrypto2.a
        ${Yang_3RD_DIR}/libspeexdsp.a ${Yang_3RD_DIR}/libyuv.a 
        ${Yang_3RD_DIR}/libyangh264decoder.a ${Yang_3RD_DIR}/libopus.a
        asound pthread dl    
        )


