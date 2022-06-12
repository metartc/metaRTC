QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
HOME_BASE=../
INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/yangwincodec5/codec/QsvCodec/include

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
win32{
    DEFINES += _AMD64_
    INCLUDEPATH += $$HOME_BASE\thirdparty\include\win
  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE\bin\lib_win_debug
    }else{
        DESTDIR += $$HOME_BASE\bin\lib_win_release
    }

    msvc{
        QMAKE_CFLAGS += /utf-8
        QMAKE_CXXFLAGS += /utf-8
       # QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
        DEFINES +=HAVE_STRUCT_TIMESPEC
        DEFINES +=WIN32_LEAN_AND_MEAN
        INCLUDEPATH += $$HOME_BASE\thirdparty\include\win\include
    }

}
SOURCES += \
    YangEncoderGpu.cpp \
    YangGpuEncoderFactory.cpp \
    codec/NvCodec/NvEncoder/NvEncoder.cpp \
    codec/NvCodec/NvEncoder/NvEncoderD3D11.cpp \
    codec/NvCodec/nvenc.cpp \
    codec/QsvCodec/QsvEncoder.cpp \
    codec/QsvCodec/common_directx11.cpp \
    codec/QsvCodec/common_directx9.cpp \
    codec/QsvCodec/common_utils.cpp \
    codec/QsvCodec/common_utils_windows.cpp \
    codec/QsvCodec/src/mfx_critical_section.cpp \
    codec/QsvCodec/src/mfx_dispatcher.cpp \
    codec/QsvCodec/src/mfx_dispatcher_log.cpp \
    codec/QsvCodec/src/mfx_dxva2_device.cpp \
    codec/QsvCodec/src/mfx_function_table.cpp \
    codec/QsvCodec/src/mfx_library_iterator.cpp \
    codec/QsvCodec/src/mfx_load_dll.cpp \
    codec/QsvCodec/src/mfx_load_plugin.cpp \
    codec/QsvCodec/src/mfx_plugin_hive.cpp \
    codec/QsvCodec/src/mfx_win_reg_key.cpp \
    codec/QsvCodec/src/qsv_main.cpp \
    codec/YangH264EncoderGpu.cpp

HEADERS += \
    YangEncoderGpu.h \
    YangGpuEncoderFactory.h \
    codec/NvCodec/NvEncoder/NvEncoder.h \
    codec/NvCodec/NvEncoder/NvEncoderD3D11.h \
    codec/NvCodec/NvEncoder/nvEncodeAPI.h \
    codec/NvCodec/encoder_info.h \
    codec/NvCodec/nvenc.h \
    codec/QsvCodec/QsvEncoder.h \
    codec/QsvCodec/bits/linux_defs.h \
    codec/QsvCodec/bits/windows_defs.h \
    codec/QsvCodec/common_directx11.h \
    codec/QsvCodec/common_directx9.h \
    codec/QsvCodec/common_utils.h \
    codec/QsvCodec/include/mfxastructures.h \
    codec/QsvCodec/include/mfxaudio++.h \
    codec/QsvCodec/include/mfxaudio.h \
    codec/QsvCodec/include/mfxbrc.h \
    codec/QsvCodec/include/mfxcamera.h \
    codec/QsvCodec/include/mfxcommon.h \
    codec/QsvCodec/include/mfxdefs.h \
    codec/QsvCodec/include/mfxenc.h \
    codec/QsvCodec/include/mfxjpeg.h \
    codec/QsvCodec/include/mfxla.h \
    codec/QsvCodec/include/mfxmvc.h \
    codec/QsvCodec/include/mfxpak.h \
    codec/QsvCodec/include/mfxplugin++.h \
    codec/QsvCodec/include/mfxplugin.h \
    codec/QsvCodec/include/mfxsession.h \
    codec/QsvCodec/include/mfxstructures.h \
    codec/QsvCodec/include/mfxvideo++.h \
    codec/QsvCodec/include/mfxvideo.h \
    codec/QsvCodec/include/mfxvp8.h \
    codec/QsvCodec/include/mfxvp9.h \
    codec/QsvCodec/include/mfxvstructures.h \
    codec/QsvCodec/src/mfx_critical_section.h \
    codec/QsvCodec/src/mfx_dispatcher.h \
    codec/QsvCodec/src/mfx_dispatcher_defs.h \
    codec/QsvCodec/src/mfx_dispatcher_log.h \
    codec/QsvCodec/src/mfx_dxva2_device.h \
    codec/QsvCodec/src/mfx_exposed_functions_list.h \
    codec/QsvCodec/src/mfx_library_iterator.h \
    codec/QsvCodec/src/mfx_load_dll.h \
    codec/QsvCodec/src/mfx_load_plugin.h \
    codec/QsvCodec/src/mfx_plugin_hive.h \
    codec/QsvCodec/src/mfx_vector.h \
    codec/QsvCodec/src/mfx_win_reg_key.h \
    codec/QsvCodec/src/mfxaudio_exposed_functions_list.h \
    codec/YangH264EncoderGpu.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
