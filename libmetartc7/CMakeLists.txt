cmake_minimum_required(VERSION 2.8)
project(libmetartc7)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=gnu11 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=gnu++11 ")
set(HOME_BASE_REL "../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)
#set(Yang_3RD_DIR ${HOME_BASE}/thirdparty/lib/android/${CMAKE_ANDROID_ARCH_ABI})
#set(Yang_Lib_DIR ${HOME_BASE}/bin/lib_android/${CMAKE_ANDROID_ARCH_ABI})
# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/user_include)
include_directories(${HOME_BASE}/thirdparty/user_include/ffmpeg)
include_directories(${HOME_BASE}/libmetartc7/src)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT NoCapture)
# 发现目录下的源文件
aux_source_directory(./src/yangaudiodev DIR_SRCS)
aux_source_directory(./src/yangcapture DIR_SRCS)
aux_source_directory(./src/yangencoder DIR_SRCS)
aux_source_directory(./src/yangdecoder DIR_SRCS)
aux_source_directory(./src/yangpush DIR_SRCS)
endif()

aux_source_directory(./src/yangutil DIR_SRCS)
aux_source_directory(./src/yangutil/buffer DIR_SRCS)
aux_source_directory(./src/yangavutil DIR_SRCS)
aux_source_directory(./src/yangstream DIR_SRCS)




if (NOT NoPlayer)
aux_source_directory(./src/yangplayer DIR_SRCS)
endif()
aux_source_directory(./src/yangp2p DIR_SRCS)
aux_source_directory(./src/yangrtc DIR_SRCS)
if ((CMAKE_SYSTEM_NAME MATCHES "Windows") OR (CMAKE_SYSTEM_NAME MATCHES "Linux"))
aux_source_directory(./src/yangdecoder/pc DIR_SRCS)
aux_source_directory(./src/yangsrt DIR_SRCS)
aux_source_directory(./src/yangrecord DIR_SRCS)
aux_source_directory(./src/yangutil/sys DIR_SRCS)
endif()

if (NOT NoCapture)
	aux_source_directory(./src/yangp2p2 DIR_SRCS)
	if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	aux_source_directory(./src/yangaudiodev/win DIR_SRCS)
	aux_source_directory(./src/yangcapture/win DIR_SRCS)
	elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	aux_source_directory(./src/yangaudiodev/linux DIR_SRCS)
	aux_source_directory(./src/yangcapture/linux DIR_SRCS)
	elseif(APPLE)
	aux_source_directory(./src/yangaudiodev/mac DIR_SRCS)
	aux_source_directory(./src/yangcapture/mac DIR_SRCS)
	else()
	aux_source_directory(./src/yangaudiodev/android DIR_SRCS)
	aux_source_directory(./src/yangcapture/android DIR_SRCS)
	aux_source_directory(./src/yangandroid DIR_SRCS)
	endif()

endif()



include(utils.cmake)
# 排除不参与编译的文件
add_library(metartc7 ${DIR_SRCS})
#if ((CMAKE_SYSTEM_NAME MATCHES "Windows") OR (CMAKE_SYSTEM_NAME MATCHES "Linux"))
#add_library(metartc5 ${DIR_SRCS})
#else()
#set(opengl-bilGLESv3)
#add_library(metartc3 ${DIR_SRCS})
#target_link_libraries( # Specifies the target library.
#       metartc5
#        ${Yang_Lib_DIR}/libmetartccore3.a
#       ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl.a ${Yang_3RD_DIR}/libcrypto.a
#        ${Yang_3RD_DIR}/libspeexdsp.a ${Yang_3RD_DIR}/libyuv.a
#        ${Yang_3RD_DIR}/libyangh264decoder.a ${Yang_3RD_DIR}/libopus.a
#        # Links the target library to the log library
        # included in the NDK.
#        EGL GLESv3 android log)

#endif()


