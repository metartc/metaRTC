cmake_minimum_required(VERSION 2.8)
project(libmetaiot5)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -ffunction-sections -fdata-sections")
set(HOME_BASE_REL "../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)
#set(Yang_3RD_DIR ${HOME_BASE}/thirdparty/lib/android/${CMAKE_ANDROID_ARCH_ABI})
#set(Yang_Lib_DIR ${HOME_BASE}/bin/lib_android/${CMAKE_ANDROID_ARCH_ABI})
# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/user_include)
include_directories(${HOME_BASE}/libmetaiot5/src)

# 发现目录下的源文件

aux_source_directory(./src/yangp2p DIR_SRCS)

include(utils.cmake)
# 排除不参与编译的文件
add_library(metaiot5 ${DIR_SRCS})

