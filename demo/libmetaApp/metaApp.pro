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
HOME_BASE=../../
INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/thirdparty/include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/ffmpeg
INCLUDEPATH += $$HOME_BASE/demo/libmetaApp/src
INCLUDEPATH += $$HOME_BASE/libmetartc6/src
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
    src/yangp2p/YangP2pFactory.cpp \
    src/yangp2p/YangP2pHandleImpl.cpp \
    src/yangp2p/YangP2pMessageHandle.cpp \
    src/yangp2p/YangP2pRtc.cpp \
    src/yangp2p2/YangP2pCapture.cpp \
    src/yangp2p2/YangP2pCaputreCamera.cpp \
    src/yangp2p2/YangP2pDecoderImpl.cpp \
    src/yangp2p2/YangP2pFactoryImpl.cpp \
    src/yangp2p2/YangP2pPublishImpl.cpp \
    src/yangplayer/YangPlayFactory.cpp \
    src/yangplayer/YangPlayerBase.cpp \
    src/yangplayer/YangPlayerDecoder.cpp \
    src/yangplayer/YangPlayerHandleImpl.cpp \
    src/yangplayer/YangPlayerPlay.cpp \
    src/yangplayer/YangRtcReceive.cpp \
    src/yangpush/YangPushCapture.cpp \
    src/yangpush/YangPushEncoder.cpp \
    src/yangpush/YangPushFactory.cpp \
    src/yangpush/YangPushHandleImpl.cpp \
    src/yangpush/YangPushMessageHandle.cpp \
    src/yangpush/YangPushPublish.cpp \
    src/yangpush/YangRtcPublish.cpp \
    src/yangpush/YangSendVideoImpl.cpp \
    src/yangrecord/YangFlvWrite.cpp \
    src/yangrecord/YangMp4File.cpp \
    src/yangrecord/YangMp4FileApp.cpp \
    src/yangrecord/YangRecEncoder.cpp \
    src/yangrecord/YangRecord.cpp \
    src/yangrecord/YangRecordApp.cpp \
    src/yangrecord/YangRecordCapture.cpp \
    src/yangrecord/YangRecordHandle.cpp \
    src/yangrecord/YangRecordMp4.cpp



HEADERS += \
    src/yangp2p/YangP2pHandleImpl.h \
    src/yangp2p/YangP2pMessageHandle.h \
    src/yangp2p/YangP2pRtc.h \
    src/yangp2p2/YangP2pCapture.h \
    src/yangp2p2/YangP2pCaputreCamera.h \
    src/yangp2p2/YangP2pDecoderImpl.h \
    src/yangp2p2/YangP2pPublishImpl.h \
    src/yangplayer/YangPlayerHandleImpl.h \
    src/yangplayer/YangRtcReceive.h \
    src/yangpush/YangPushEncoder.h \
    src/yangpush/YangPushHandleImpl.h \
    src/yangpush/YangPushMessageHandle.h \
    src/yangpush/YangSendVideoImpl.h \
    src/yangrecord/YangRecordHandle.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
