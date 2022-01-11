cmake_minimum_required(VERSION 2.8)
project(libmetartc2)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -ffunction-sections -fdata-sections")
set(HOME_BASE "../")

# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/user_include)
include_directories(${HOME_BASE}/thirdparty/user_include/ffmpeg)
include_directories(${HOME_BASE}/libmetartc2/src)


# 发现目录下的源文件
aux_source_directory(./src/yangaudiodev DIR_SRCS)
aux_source_directory(./src/yangaudiodev/linux DIR_SRCS)
aux_source_directory(./src/yangutil DIR_SRCS)
aux_source_directory(./src/yangutil/buffer DIR_SRCS)
aux_source_directory(./src/yangavutil DIR_SRCS)
aux_source_directory(./src/yangstream DIR_SRCS)
aux_source_directory(./src/yangcapture DIR_SRCS)
aux_source_directory(./src/yangencoder DIR_SRCS)
aux_source_directory(./src/yangdecoder DIR_SRCS)
aux_source_directory(./src/yangplayer DIR_SRCS)
aux_source_directory(./src/yangpush DIR_SRCS)
aux_source_directory(./src/yangrecord DIR_SRCS)
aux_source_directory(./src/yangrtmp DIR_SRCS)
aux_source_directory(./src/yangsrs DIR_SRCS)
aux_source_directory(./src/yangsrt DIR_SRCS)

include(utils.cmake)
# 排除不参与编译的文件


add_library(metartc2 ${DIR_SRCS})
