cmake_minimum_required(VERSION 2.8)
project(metaipc5)

add_definitions(-D__STDC_FORMAT_MACROS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -ffunction-sections -fdata-sections")
set(HOME_BASE_REL "../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)
set(Yang_3RD_DIR ${HOME_BASE}/thirdparty/lib/)
set(Yang_Lib_DIR ${HOME_BASE}/bin/lib_debug)
# 头文件目录
include_directories(${HOME_BASE}/include)
include_directories(${HOME_BASE}/thirdparty/include)
include_directories(${HOME_BASE}/thirdparty/user_include)
include_directories(${HOME_BASE}/metaipc5/src)

# 发现目录下的源文件

aux_source_directory(./src DIR_SRCS)

include(utils.cmake)
add_executable(metaipc5 ${DIR_SRCS})
if (NOT NoCapture)
target_link_libraries(metaipc5  ${Yang_Lib_DIR}/libmetartc5.a  ${Yang_Lib_DIR}/libmetartccore5.a
        ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl2.a ${Yang_3RD_DIR}/libcrypto2.a
        ${Yang_3RD_DIR}/libspeexdsp.a ${Yang_3RD_DIR}/libyuv.a ${Yang_3RD_DIR}/libx264.a
        ${Yang_3RD_DIR}/libopus.a ${Yang_3RD_DIR}/libopenh264.a
        ${Yang_3RD_DIR}/libusrsctp.a pthread dl
        asound 
        )
else()
target_link_libraries(metaipc5  ${Yang_Lib_DIR}/libmetartc5.a  ${Yang_Lib_DIR}/libmetartccore5.a
        ${Yang_3RD_DIR}/libsrtp2.a ${Yang_3RD_DIR}/libssl2.a ${Yang_3RD_DIR}/libcrypto2.a
        ${Yang_3RD_DIR}/libusrsctp.a pthread dl    
        )

endif()

