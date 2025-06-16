CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += __STDC_FORMAT_MACROS

HOME_BASE=../
INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/thirdparty/include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include
INCLUDEPATH += $$HOME_BASE/libyangwhip7/src

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
    src/yangwhip/YangSrsConnection.c \
    src/yangwhip/YangWhip.c \
    src/yangwhip/YangZlmConnection.c


HEADERS += \
    src/yangwhip/YangSrsConnection.h \
    src/yangwhip/YangWhip.h \
    src/yangwhip/YangZlmConnection.h


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
