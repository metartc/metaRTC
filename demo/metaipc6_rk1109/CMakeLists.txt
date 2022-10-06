cmake_minimum_required(VERSION 2.8)
project(metaipc6)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=gnu++11 ")
set(HOME_BASE_REL "../../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)
set(Yang_3RD_DIR ${HOME_BASE}/thirdparty/lib/mips32)
set(Yang_RK_DIR ${HOME_BASE}/demo/metaipc6_rk1109/lib)
set(Yang_Lib_DIR ${HOME_BASE}/bin/lib_debug)
# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/demo/metaipc6_rk1109/lib/include)
include_directories(${HOME_BASE}/demo/metaipc6_rk1109/lib/include/easymedia)
include_directories(${HOME_BASE}/demo/metaipc6_rk1109/lib/include/rkmedia)
include_directories(${HOME_BASE}/demo/metaipc6_rk1109/src)

# 发现目录下的源文件

aux_source_directory(./src DIR_SRCS)
aux_source_directory(./src/yangipc DIR_SRCS)
aux_source_directory(./src/yangipc_rk DIR_SRCS)

include(utils.cmake)
add_executable(metaipc6 ${DIR_SRCS})
#${Yang_RK_DIR}/libeasymedia.so
target_link_libraries(metaipc6  ${Yang_Lib_DIR}/libmetaCApp.a    ${Yang_Lib_DIR}/libmetartccore5.a
        ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl.so ${Yang_3RD_DIR}/libcrypto.so ${Yang_3RD_DIR}/libusrsctp.a
         ${Yang_RK_DIR}/libdrm.so.2 ${Yang_RK_DIR}/librockchip_mpp.so.1 ${Yang_RK_DIR}/libsound.so.2
        ${Yang_RK_DIR}/libRKAP_3A.so ${Yang_RK_DIR}/libBasicUsageEnvironment.so.1 ${Yang_RK_DIR}/libgroupsock.so.8
        ${Yang_RK_DIR}/libdrm.so.2 ${Yang_RK_DIR}/libliveMedia.so.61 ${Yang_RK_DIR}/librockchip_mpp.so.1
        ${Yang_RK_DIR}/libRKAP_ANR.so ${Yang_RK_DIR}/libRKAP_Common.so ${Yang_RK_DIR}/libeasymedia.so
        ${Yang_RK_DIR}/libv4l2.so.0 ${Yang_RK_DIR}/librga.so.2 ${Yang_RK_DIR}/libmd_share.so
        ${Yang_RK_DIR}/libod_share.so ${Yang_RK_DIR}/librkaiq.so ${Yang_RK_DIR}/libv4lconvert.so.0
        ${Yang_RK_DIR}/libUsageEnvironment.so.3
        pthread dl rt m  
        )



