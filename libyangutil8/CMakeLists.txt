cmake_minimum_required(VERSION 2.8)
project(libyangutil8)
add_definitions(-D__STDC_FORMAT_MACROS)


set(CMAKE_BUILD_TYPE "RELEASE")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=gnu++11")
set(HOME_BASE "../")


if (Yang_Moc EQUAL 2)
	include_directories(${HOME_BASE}/thirdparty/user_include/alsa-dev)
elseif(Yang_Moc EQUAL 1)
	include_directories(${HOME_BASE}/thirdparty/user_include/mips/include)
endif()

# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/user_include)
include_directories(${HOME_BASE}/thirdparty/user_include/openssl)
include_directories(${HOME_BASE}/thirdparty/user_include/mbedtls3)
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
add_definitions(-D_TIMESPEC_DEFINED)
include_directories(${HOME_BASE}/thirdparty/include/win/include)
endif()
include_directories(${HOME_BASE}/libyangutil8/src)

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# 发现目录下的源文件
aux_source_directory(./src/yangcodec DIR_SRCS)
aux_source_directory(./src/yangutil/sys DIR_SRCS)
aux_source_directory(./src/yangutil/buffer2 DIR_SRCS)
aux_source_directory(./src/yangaudio DIR_SRCS)
aux_source_directory(./src/yangvideo DIR_SRCS)
aux_source_directory(./src/yangstream DIR_SRCS)
aux_source_directory(./src/yangjson DIR_SRCS)
aux_source_directory(./src/yangmqtt DIR_SRCS)
aux_source_directory(./src/yangp2p DIR_SRCS)
aux_source_directory(./src/yangssl DIR_SRCS)
include(utils.cmake)
# 排除不参与编译的文件


add_library(yangutil8 ${DIR_SRCS})
