CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += __STDC_FORMAT_MACROS
DEFINES += MBEDTLS_SSL_DTLS_SRTP
HOME_BASE=../
macx{
HOME_BASE=../../../
}


INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/thirdparty/include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/ffmpeg
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/mbedtls3
INCLUDEPATH += $$HOME_BASE/libyangutil8/src

unix{
  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE/bin/lib_debug
    }else{

        DESTDIR += $$HOME_BASE/bin/lib_release
    }
}
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
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/yangaudio/YangAudioMix.c \
    src/yangaudio/YangAudioUtil.c \
    src/yangaudio/YangPreProcess.c \
    src/yangaudio/YangResample.c \
    src/yangaudio/YangRtcAec.c \
    src/yangjson/YangJson.c \
    src/yangjson/YangJsonImpl.c \
    src/yangutil/buffer2/YangBuffer2.c \
    src/yangutil/buffer2/YangMediaBuffer2.c \
    src/yangutil/buffer2/YangPacket.c \
    src/yangutil/buffer2/YangSortBuffer.c \
    src/yangutil/buffer2/YangVideoEncoderBuffer2.c \
    src/yangutil/sys/YangAvtype.c \
    src/yangutil/sys/YangBuffer.c \
    src/yangutil/sys/YangCLog.c \
    src/yangutil/sys/YangCString.c \
    src/yangutil/sys/YangCTimer.c \
    src/yangutil/sys/YangEndian.c \
    src/yangutil/sys/YangFile.c \
    src/yangutil/sys/YangHttp.c \
    src/yangutil/sys/YangIni.c \
    src/yangutil/sys/YangLibHandle.c \
    src/yangutil/sys/YangMath.c \
    src/yangutil/sys/YangSocket.c \
    src/yangutil/sys/YangSocketIP.c \
    src/yangutil/sys/YangSsrc.c \
    src/yangutil/sys/YangThread.c \
    src/yangutil/sys/YangTime.c \
    src/yangutil/sys/YangTimestamp.c \
    src/yangutil/sys/YangUrl.c \
    src/yangutil/sys/YangVector.c \
    src/yangvideo/YangMeta.c \
    src/yangvideo/YangNalu.c \
    src/yangvideo/YangSpspps.c


HEADERS += \
    src/yangjson/YangJsonImpl.h


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
