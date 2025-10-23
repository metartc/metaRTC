cmake_minimum_required(VERSION 3.16)
project(metapushstream7 VERSION 1.0 LANGUAGES C CXX)


set(HOME_BASE_REL "../../")
get_filename_component(HOME_BASE ${HOME_BASE_REL} REALPATH)

if(APPLE)
    set(MACOS 1)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 OPTIONAL_COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Gui OpenGLWidgets)

qt_add_executable(metapushstream7 WIN32 MACOSX_BUNDLE
    main.cpp
    recordmainwindow.cpp recordmainwindow.h recordmainwindow.ui
    video/YangPlayWidget.cpp video/YangPlayWidget.h
    video/YangYuvPlayWidget.cpp video/YangYuvPlayWidget.h
    video/yangrecordthread.cpp video/yangrecordthread.h
    video/yangrecordvideowin.cpp video/yangrecordvideowin.h
    video/yangvideotype.h
    yangjanus.cpp yangjanus.h yangjanus.ui
    yangpush/YangPushCapture.cpp
    yangpush/YangPushEncoder.cpp yangpush/YangPushEncoder.h
    yangpush/YangPushFactory.cpp
    yangpush/YangPushHandleImpl.cpp yangpush/YangPushHandleImpl.h
    yangpush/YangPushMessageHandle.cpp yangpush/YangPushMessageHandle.h
    yangpush/YangPushPublish.cpp
    yangpush/YangRtcPublish.cpp
    yangpush/YangSendVideoImpl.cpp yangpush/YangSendVideoImpl.h
)
target_include_directories(metapushstream7 PRIVATE
    ${HOME_BASE}/libmetartc7/src
    ${HOME_BASE}/thirdparty/include
)

target_compile_definitions(metapushstream7 PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(metapushstream7 PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGLWidgets
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(metapushstream7 PRIVATE
        Qt::Widgets
    )
endif()

if(MACOS)
    target_include_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/include
    )

    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/thirdparty/lib/mac
    )

    set_target_properties(metapushstream7 PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    )

    target_link_libraries(metapushstream7 PRIVATE
        "-framework AVFoundation"
        "-framework AudioUnit"
        "-framework CoreAudio"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework OpenGL"
        "-framework VideoToolbox"
        crypto
        dl
        metartc7
        metartccore7
        openh264
        opus
        pthread
        speexdsp
        srtp2
        ssl
        usrsctp
        yangwhip7
        yuv
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug AND MACOS)
    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/bin/lib_debug
    )
     set_target_properties(metapushstream7 PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY "${HOME_BASE}/bin/app_debug"
    )
endif()

if(MACOS AND NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/bin/lib_release
    )
    set_target_properties(metapushstream7 PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY "${HOME_BASE}/bin/app_release"
    )
endif()

if(UNIX AND NOT MACOS)
    target_include_directories(metapushstream7 PRIVATE
       ${HOME_BASE}/include
    )

    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/thirdparty/lib
    )

    target_link_libraries(metapushstream7 PRIVATE
        asound
        crypto
        dl
        metartc7
        metartccore7
        openh264
        opus
        pthread
        speexdsp
        srtp2
        ssl
        usrsctp
        yangwhip7
        yuv
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug AND UNIX AND NOT MACOS)
    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/bin/lib_debug
    )
    set_target_properties(metapushstream7 PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY "${HOME_BASE}/bin/app_debug"
    )
endif()

if(UNIX AND NOT CMAKE_BUILD_TYPE STREQUAL Debug AND NOT MACOS)
    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/bin/lib_release
    )
    set_target_properties(metapushstream7 PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY "${HOME_BASE}/bin/app_release"
    )
endif()

if(WIN32)
    target_include_directories(metapushstream7 PRIVATE
        ${HOME_BASE}\include
    )

    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/thirdparty/lib/win
    )

    target_link_libraries(metapushstream7 PRIVATE
        Crypt32
        Strmiids
        avcodec
        avutil
        crypto
        ksuser
        metartc7
        metartccore7
        ole32
        openh264
        opus
        speexdsp
        srtp2
        ssl
        usrsctp
        winmm
        ws2_32
        yangwhip7
        yuv
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug AND WIN32)
    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/bin/lib_win_debug
    )
    set_target_properties(metapushstream7 PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY "${HOME_BASE}/bin/app_win_debug"
    )
endif()

if(WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_directories(metapushstream7 PRIVATE
        ${HOME_BASE}/bin/lib_win_release
    )
    set_target_properties(metapushstream7 PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY "${HOME_BASE}/bin/app_win_release"
    )
endif()

if(MSVC AND WIN32)
    target_include_directories(metapushstream7 PRIVATE
        ${HOME_BASE}\thirdparty\include\win\include
    )

    target_compile_definitions(metapushstream7 PRIVATE
        HAVE_STRUCT_TIMESPEC
        WIN32_LEAN_AND_MEAN
    )

    target_compile_options(metapushstream7
        /execution-charset:utf-8
        /source-charset:utf-8
    )
endif()

install(TARGETS metapushstream7
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
