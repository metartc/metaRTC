cmake_minimum_required(VERSION 2.8)
project(libyangwhip8)
add_definitions(-D__STDC_FORMAT_MACROS)


set(CMAKE_BUILD_TYPE "RELEASE")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=gnu++11")
set(HOME_BASE "../")

# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(./src)

# 发现目录下的源文件


aux_source_directory(./src/yangwhip DIR_SRCS)




include(utils.cmake)
# 排除不参与编译的文件


add_library(yangwhip8 ${DIR_SRCS})

