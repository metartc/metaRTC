cmake_minimum_required(VERSION 2.8)
project(metaipc5)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=gnu++11 ")
set(HOME_BASE_REL "../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)
set(Yang_3RD_DIR ${HOME_BASE}/thirdparty/lib/mips32/)
set(Yang_JZ_DIR ${HOME_BASE}/metaipc5_jz/lib/)
set(Yang_Lib_DIR ${HOME_BASE}/bin/lib_debug)
# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/metaipc5_jz/lib/include)
include_directories(${HOME_BASE}/metaipc5_jz/src)

# 发现目录下的源文件

aux_source_directory(./src DIR_SRCS)
aux_source_directory(./src/yangipc DIR_SRCS)
aux_source_directory(./src/yangipc_jz DIR_SRCS)

include(utils.cmake)
add_executable(metaipc5 ${DIR_SRCS})

target_link_libraries(metaipc5  ${Yang_Lib_DIR}/libmetartccore5.a
        ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl.a ${Yang_3RD_DIR}/libcrypto.a
        ${Yang_JZ_DIR}/libimp.a ${Yang_JZ_DIR}/libalog.a  ${Yang_JZ_DIR}/libsysutils.a 
        ${Yang_3RD_DIR}/libusrsctp.a pthread dl rt m  
        )



