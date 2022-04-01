CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += HAVE_CONFIG_H
HOME_BASE=../
INCLUDEPATH += $$HOME_BASE/thirdparty/include/
INCLUDEPATH += $$HOME_BASE/thirdparty/include/speex
INCLUDEPATH += $$HOME_BASE/libspeexdsp/src
unix{
  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE/bin/lib_debug
    }else{

        DESTDIR += $$HOME_BASE/bin/lib_release
    }
}
win32{
    DEFINES += __x86_64__
    DEFINES += HAVE_WINSOCK2_H

  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE\bin\lib_win_debug
    }else{
        DESTDIR += $$HOME_BASE\bin\lib_win_release
    }

    msvc{
        QMAKE_CFLAGS += /utf-8
        QMAKE_CXXFLAGS += /utf-8
       # QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
       # DEFINES +=HAVE_STRUCT_TIMESPEC
        #DEFINES +=WIN32_LEAN_AND_MEAN

    }

}
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/buffer.c \
    src/fftwrap.c \
    src/filterbank.c \
    src/jitter.c \
    src/kiss_fft.c \
    src/kiss_fftr.c \
    src/mdf.c \
    src/preprocess.c \
    src/resample.c \
    src/scal.c \
    src/smallft.c


HEADERS += \
    src/_kiss_fft_guts.h \
    src/arch.h \
    src/bfin.h \
    src/config.h \
    src/fftwrap.h \
    src/filterbank.h \
    src/fixed_arm4.h \
    src/fixed_arm5e.h \
    src/fixed_bfin.h \
    src/fixed_debug.h \
    src/fixed_generic.h \
    src/kiss_fft.h \
    src/kiss_fftr.h \
    src/math_approx.h \
    src/misc_bfin.h \
    src/os_support.h \
    src/pseudofloat.h \
    src/resample_neon.h \
    src/resample_sse.h \
    src/smallft.h \
    src/speex/speex_buffer.h \
    src/stack_alloc.h \
    src/vorbis_psy.h


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    include/srtp2/meson.build \
    src/echo_diagnostic.m
