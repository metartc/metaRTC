cmake_minimum_required(VERSION 2.8)
project(libspeexdsp)
add_definitions(-DHAVE_CONFIG_H)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -ffunction-sections -fdata-sections")
set(HOME_BASE "../")

# 头文件目录

include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/include/speex)
include_directories(${HOME_BASE}/libspeexdsp/src)


# 发现目录下的源文件

#aux_source_directory(./src DIR_SRCS)


# 排除不参与编译的文件


add_library(speexdsp     src/buffer.c 
    src/fftwrap.c 
    src/filterbank.c 
    src/jitter.c 
    src/kiss_fft.c 
    src/kiss_fftr.c 
    src/mdf.c 
    src/preprocess.c 
    src/resample.c 
    src/scal.c 
    src/smallft.c)
