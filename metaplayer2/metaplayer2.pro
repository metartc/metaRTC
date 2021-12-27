QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_WIN_MSC
HOME_BASE=../
unix{

    INCLUDEPATH += $$HOME_BASE/include
    CONFIG(debug, debug|release) {
        LIBS += -L$$HOME_BASE/bin/lib_debug
        DESTDIR += $$HOME_BASE/bin/app_debug
    }else{
        LIBS += -L$$HOME_BASE/bin/lib_release
        DESTDIR += $$HOME_BASE/bin/app_release
    }
 LIBS +=  -L$$HOME_BASE/thirdparty/lib -lyangh264decoder
 LIBS += -lmetartc2  -lavutil -lavcodec -lpthread -lasound -ldl -lssl2 -lcrypto2  -lsrtp2
}

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    video/yangrecordthread.cpp \
    yangplayer/YangMouseListenWidget.cpp \
    yangplayer/YangPlayWidget.cpp


HEADERS += \
    mainwindow.h \
    video/yangrecordthread.h \
    yangplayer/YangMouseListenWidget.h \
    yangplayer/YangPlayWidget.h \
    yangplayer_config.h


FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
