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
HOME_BASE=../
INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/thirdparty/include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/ffmpeg
INCLUDEPATH += $$HOME_BASE/libmetartc7/src

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
    src/yangaudiodev/YangAudioCapture.cpp \
    src/yangaudiodev/YangAudioCaptureData.cpp \
    src/yangaudiodev/YangAudioCaptureHandle.cpp \
    src/yangaudiodev/YangAudioPlay.cpp \
    src/yangaudiodev/YangAudioRenderData.cpp \
    src/yangaudiodev/linux/YangAudioAecLinux.cpp \
    src/yangaudiodev/linux/YangAudioCaptureLinux.cpp \
    src/yangaudiodev/linux/YangAudioPlayLinux.cpp \
    src/yangaudiodev/mac/YangAudioCaptureMac.cpp \
    src/yangaudiodev/mac/YangAudioDeviceMac.cpp \
    src/yangaudiodev/mac/YangAudioMac.cpp \
    src/yangaudiodev/mac/YangAudioPlayMac.cpp \
    src/yangaudiodev/win/YangAudioCaptureWindows.cpp \
    src/yangaudiodev/win/YangRecAudioCaptureHandle.cpp \
    src/yangaudiodev/win/YangWinAudioApi.cpp \
    src/yangaudiodev/win/YangWinAudioApiAec.cpp \
    src/yangaudiodev/win/YangWinAudioApiCapture.cpp \
    src/yangaudiodev/win/YangWinAudioApiDevice.cpp \
    src/yangaudiodev/win/YangWinAudioApiRender.cpp \
    src/yangaudiodev/win/YangWinAudioCapture.cpp \
    src/yangaudiodev/win/YangWinAudioCaptureHandle.cpp \
    src/yangavutil/YangMakeWave.cpp \
    src/yangavutil/YangVideoEncoderMeta.cpp \
    src/yangavutil/YangYuvConvert.cpp \
    src/yangcapture/YangCaptureFactory.cpp \
    src/yangcapture/YangScreenShare.cpp \
    src/yangcapture/YangVideoCapture.cpp \
    src/yangcapture/YangVideoCaptureHandle.cpp \
    src/yangcapture/android/YangCameraAndroid.cpp \
    src/yangcapture/android/YangVideoCaptureAndroid.cpp \
    src/yangcapture/linux/YangVideoCaptureLinux.cpp \
    src/yangcapture/win/YangDXGIManager.cpp \
    src/yangcapture/win/YangVideoCaptureWindows.cpp \
    src/yangcapture/win/YangWinVideoCaptureHandle.cpp \
    src/yangdecoder/YangAudioDecoder.cpp \
    src/yangdecoder/YangAudioDecoderHandle.cpp \
    src/yangdecoder/YangAudioDecoderHandles.cpp \
    src/yangdecoder/YangAudioDecoderOpus.cpp \
    src/yangdecoder/YangDecoderFactory.cpp \
    src/yangdecoder/YangDecoderH264.cpp \
    src/yangdecoder/YangDecoderLibde265.cpp \
    src/yangdecoder/YangDecoderMediacodec.cpp \
    src/yangdecoder/YangDecoderOpenh264.cpp \
    src/yangdecoder/YangH264DecoderSoft.cpp \
    src/yangdecoder/YangH264DecoderSoft2.cpp \
    src/yangdecoder/YangVideoDecoderHandle.cpp \
    src/yangdecoder/YangVideoDecoderHandles.cpp \
    src/yangdecoder/pc/YangAudioDecoderAac.cpp \
    src/yangdecoder/pc/YangAudioDecoderSpeex.cpp \
    src/yangdecoder/pc/YangHeaderParseFfmpeg.cpp \
    src/yangdecoder/pc/YangVideoDecoderFfmpeg.cpp \
    src/yangencoder/YangAudioEncoder.cpp \
    src/yangencoder/YangAudioEncoderAac.cpp \
    src/yangencoder/YangAudioEncoderHandle.cpp \
    src/yangencoder/YangAudioEncoderHandleCb.cpp \
    src/yangencoder/YangAudioEncoderMeta.cpp \
    src/yangencoder/YangAudioEncoderMp3.cpp \
    src/yangencoder/YangAudioEncoderOpus.cpp \
    src/yangencoder/YangAudioEncoderSpeex.cpp \
    src/yangencoder/YangEncoderFactory.cpp \
    src/yangencoder/YangEncoderMediacodec.cpp \
    src/yangencoder/YangFfmpegEncoderMeta.cpp \
    src/yangencoder/YangH264EncoderMeta.cpp \
    src/yangencoder/YangH264EncoderSoft.cpp \
    src/yangencoder/YangH265EncoderMeta.cpp \
    src/yangencoder/YangH265EncoderSoft.cpp \
    src/yangencoder/YangOpenH264Encoder.cpp \
    src/yangencoder/YangVideoEncoder.cpp \
    src/yangencoder/YangVideoEncoderFfmpeg.cpp \
    src/yangencoder/YangVideoEncoderHandle.cpp \
    src/yangrtc/YangPeerConnection2.cpp \
    src/yangstream/YangStreamManager.cpp \
    src/yangstream/YangSynBuffer.cpp \
    src/yangstream/YangSynBufferManager.cpp \
    src/yangutil/YangAvinfo.cpp \
    src/yangutil/YangLoadLib.cpp \
    src/yangutil/YangString.cpp \
    src/yangutil/YangSysMessageHandle.cpp \
    src/yangutil/YangThread.cpp \
    src/yangutil/YangTimer.cpp \
    src/yangutil/buffer/YangAudioBuffer.cpp \
    src/yangutil/buffer/YangAudioEncoderBuffer.cpp \
    src/yangutil/buffer/YangAudioPlayBuffer.cpp \
    src/yangutil/buffer/YangMediaBuffer.cpp \
    src/yangutil/buffer/YangVideoBuffer.cpp \
    src/yangutil/buffer/YangVideoDecoderBuffer.cpp \
    src/yangutil/buffer/YangVideoEncoderBuffer.cpp\
    src/yangcapture/mac/YangVideoCaptureMac.cpp \

macx{
    SOURCES += src/yangcapture/mac/YangVideoDeviceMac.mm
}


HEADERS += \
    src/yangaudiodev/YangAudioCaptureHandle.h \
    src/yangaudiodev/YangCaptureCallback.h \
    src/yangaudiodev/linux/YangAudioAecLinux.h \
    src/yangaudiodev/linux/YangAudioCaptureLinux.h \
    src/yangaudiodev/linux/YangAudioPlayLinux.h \
    src/yangaudiodev/mac/YangAudioCaptureMac.h \
    src/yangaudiodev/mac/YangAudioMac.h \
    src/yangaudiodev/mac/YangAudioPlayMac.h \
    src/yangaudiodev/win/YangAudioApiCapture.h \
    src/yangaudiodev/win/YangAudioCaptureWindows.h \
    src/yangaudiodev/win/YangRecAudioCaptureHandle.h \
    src/yangaudiodev/win/YangWinAudioApi.h \
    src/yangaudiodev/win/YangWinAudioApiAec.h \
    src/yangaudiodev/win/YangWinAudioApiCapture.h \
    src/yangaudiodev/win/YangWinAudioApiDevice.h \
    src/yangaudiodev/win/YangWinAudioApiRender.h \
    src/yangaudiodev/win/YangWinAudioCapture.h \
    src/yangaudiodev/win/YangWinAudioCaptureHandle.h \
    src/yangaudiodev/win/YangWinAudioDevice.h \
    src/yangcapture/YangScreenCaptureImpl.h \
    src/yangcapture/YangScreenShare.h \
    src/yangcapture/YangVideoCaptureHandle.h \
    src/yangcapture/android/YangCameraAndroid.h \
    src/yangcapture/android/YangVideoCaptureAndroid.h \
    src/yangcapture/linux/YangVideoCaptureLinux.h \
    src/yangcapture/mac/YangVideoCaptureMac.h \
    src/yangcapture/mac/YangVideoDeviceMac.h \
    src/yangcapture/win/YangDXGIManager.h \
    src/yangcapture/win/YangVideoCaptureWindows.h \
    src/yangcapture/win/YangVideoSrc.h \
    src/yangcapture/win/YangWinVideoCaptureHandle.h \
    src/yangdecoder/YangAudioDecoderOpus.h \
    src/yangdecoder/YangDecoderH264.h \
    src/yangdecoder/YangDecoderLibde265.h \
    src/yangdecoder/YangDecoderMediacodec.h \
    src/yangdecoder/YangDecoderOpenh264.h \
    src/yangdecoder/YangH264Dec.h \
    src/yangdecoder/YangH264DecoderSoft.h \
    src/yangdecoder/YangH264DecoderSoft2.h \
    src/yangdecoder/YangH264DecoderSoftFactory.h \
    src/yangdecoder/pc/YangAudioDecoderAac.h \
    src/yangdecoder/pc/YangAudioDecoderSpeex.h \
    src/yangdecoder/pc/YangHeaderParseFfmpeg.h \
    src/yangdecoder/pc/YangVideoDecoderFfmpeg.h \
    src/yangencoder/YangAudioEncoderAac.h \
    src/yangencoder/YangAudioEncoderMp3.h \
    src/yangencoder/YangAudioEncoderOpus.h \
    src/yangencoder/YangAudioEncoderSpeex.h \
    src/yangencoder/YangEncoderMediacodec.h \
    src/yangencoder/YangFfmpegEncoderMeta.h \
    src/yangencoder/YangH264EncHeader.h \
    src/yangencoder/YangH264EncoderMeta.h \
    src/yangencoder/YangH264EncoderSoft.h \
    src/yangencoder/YangH265EncoderMeta.h \
    src/yangencoder/YangH265EncoderSoft.h \
    src/yangencoder/YangOpenH264Encoder.h \
    src/yangencoder/YangVideoEncoderFfmpeg.h \
    src/yangencoder/lame.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
