cmake_minimum_required(VERSION 2.8)
project(metaipc6)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -ffunction-sections -fdata-sections")
set(HOME_BASE_REL "../../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)
set(Yang_3RD_DIR ${HOME_BASE}/thirdparty/lib/mips32)
set(Yang_JZ_DIR ${HOME_BASE}/demo/metaipc6_jz/lib)
set(Yang_Lib_DIR ${HOME_BASE}/bin/lib_debug)
# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/user_include)
include_directories(${HOME_BASE}/demo/metaipc6_jz/lib/include)
include_directories(${HOME_BASE}/demo/metaipc6_jz/src)

# 发现目录下的源文件
aux_source_directory(./src/yangipc_jz DIR_SRCS)
aux_source_directory(./src DIR_SRCS)

include(utils.cmake)
add_executable(metaipc6 ${DIR_SRCS})
if (NOT NoCapture)
target_link_libraries(metaipc6  ${Yang_Lib_DIR}/libmetaCApp.a   ${Yang_Lib_DIR}/libmetartccore6.a
        ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl.a ${Yang_3RD_DIR}/libcrypto.a
       # ${Yang_3RD_DIR}/libsrtp2_mbed.a ${Yang_3RD_DIR}/libmbedtls.a ${Yang_3RD_DIR}/libmbedx509.a ${Yang_3RD_DIR}/libmbedcrypto.a    
        ${Yang_3RD_DIR}/libspeexdsp.a ${Yang_3RD_DIR}/libyuv.a ${Yang_3RD_DIR}/libx264.a
        ${Yang_3RD_DIR}/libopus.a ${Yang_3RD_DIR}/libopenh264.a
        ${Yang_3RD_DIR}/libusrsctp.a pthread dl      
        asound 
        )
else()
target_link_libraries(metaipc6   ${Yang_Lib_DIR}/libmetaCApp.a   ${Yang_Lib_DIR}/libmetartccore6.a
        ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl.a ${Yang_3RD_DIR}/libcrypto.a
        ${Yang_JZ_DIR}/libimp.a ${Yang_JZ_DIR}/libsysutils.a ${Yang_JZ_DIR}/libalog.a 
        ${Yang_3RD_DIR}/libusrsctp.a pthread dl rt m         
        )

endif()

