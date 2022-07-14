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
INCLUDEPATH += $$HOME_BASE/libmetartc5/src

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
    src/yangaudiodev/linux/YangAlsaDeviceHandle.cpp \
    src/yangaudiodev/linux/YangAlsaHandle.cpp \
    src/yangaudiodev/linux/YangAudioCaptureLinux.cpp \
    src/yangaudiodev/linux/YangAudioPlayLinux.cpp \
    src/yangaudiodev/win/YangAudioCaptureWindows.cpp \
    src/yangaudiodev/win/YangRecAudioCaptureHandle.cpp \
    src/yangaudiodev/win/YangWinAudioApi.cpp \
    src/yangaudiodev/win/YangWinAudioApiAec.cpp \
    src/yangaudiodev/win/YangWinAudioApiCapture.cpp \
    src/yangaudiodev/win/YangWinAudioApiDevice.cpp \
    src/yangaudiodev/win/YangWinAudioApiRender.cpp \
    src/yangaudiodev/win/YangWinAudioCapture.cpp \
    src/yangaudiodev/win/YangWinAudioCaptureHandle.cpp \
    src/yangavutil/YangImageConvert.cpp \
    src/yangavutil/YangMakeWave.cpp \
    src/yangavutil/YangPicUtilFfmpeg.cpp \
    src/yangavutil/YangVideoEncoderMeta.cpp \
    src/yangavutil/YangYuvConvert.cpp \
    src/yangcapture/YangCaptureFactory.cpp \
    src/yangcapture/YangScreenCaptureImpl.cpp \
    src/yangcapture/YangScreenShare.cpp \
    src/yangcapture/YangVideoCapture.cpp \
    src/yangcapture/YangVideoCaptureHandle.cpp \
    src/yangcapture/android/YangCameraAndroid.cpp \
    src/yangcapture/android/YangVideoCaptureAndroid.cpp \
    src/yangcapture/linux/YangVideoCaptureLinux.cpp \
    src/yangcapture/win/YangDXGIManager.cpp \
    src/yangcapture/win/YangVideoCaptureWindows.cpp \
    src/yangcapture/win/YangVideoSrc.cpp \
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
    src/yangdecoder/pc/YangH264Header.cpp \
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
    src/yangencoder/YangH264EncHeader.cpp \
    src/yangencoder/YangH264EncoderMeta.cpp \
    src/yangencoder/YangH264EncoderSoft.cpp \
    src/yangencoder/YangH265EncoderMeta.cpp \
    src/yangencoder/YangH265EncoderSoft.cpp \
    src/yangencoder/YangOpenH264Encoder.cpp \
    src/yangencoder/YangVideoEncoder.cpp \
    src/yangencoder/YangVideoEncoderFfmpeg.cpp \
    src/yangencoder/YangVideoEncoderHandle.cpp \
    src/yangp2p/YangP2pFactory.cpp \
    src/yangp2p/YangP2pHandleImpl.cpp \
    src/yangp2p/YangP2pMessageHandle.cpp \
    src/yangp2p/YangP2pRtc.cpp \
    src/yangp2p/YangP2pRtcIce.cpp \
    src/yangp2p2/YangP2pCapture.cpp \
    src/yangp2p2/YangP2pCaputreCamera.cpp \
    src/yangp2p2/YangP2pDecoderImpl.cpp \
    src/yangp2p2/YangP2pFactoryImpl.cpp \
    src/yangp2p2/YangP2pPublishImpl.cpp \
    src/yangplayer/YangPlayFactory.cpp \
    src/yangplayer/YangPlayReceive.cpp \
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
    src/yangpush/YangRtmpPublish.cpp \
    src/yangpush/YangSendVideoImpl.cpp \
    src/yangrecord/YangFlvWrite.cpp \
    src/yangrecord/YangMp4File.cpp \
    src/yangrecord/YangMp4FileApp.cpp \
    src/yangrecord/YangRecEncoder.cpp \
    src/yangrecord/YangRecord.cpp \
    src/yangrecord/YangRecordApp.cpp \
    src/yangrecord/YangRecordCapture.cpp \
    src/yangrecord/YangRecordHandle.cpp \
    src/yangrecord/YangRecordMp4.cpp \
    src/yangrtc/YangPeerConnection2.cpp \
    src/yangsrt/YangSrtBase.cpp \
    src/yangsrt/YangTsBuffer.cpp \
    src/yangsrt/YangTsMuxer.cpp \
    src/yangsrt/YangTsPacket.cpp \
    src/yangsrt/YangTsdemux.cpp \
    src/yangsrt/common.cpp \
    src/yangsrt/crc.cpp \
    src/yangsrt/srt_data.cpp \
    src/yangstream/YangStreamHandle.cpp \
    src/yangstream/YangStreamManager.cpp \
    src/yangstream/YangStreamSrt.cpp \
    src/yangstream/YangSynBuffer.cpp \
    src/yangutil/YangAvinfo.cpp \
    src/yangutil/YangJson.cpp \
    src/yangutil/YangLoadLib.cpp \
    src/yangutil/YangString.cpp \
    src/yangutil/YangSysMessageHandle.cpp \
    src/yangutil/YangThread.cpp \
    src/yangutil/YangTimer.cpp \
    src/yangutil/YangWindowsMouse.cpp \
    src/yangutil/YangYuvGl.cpp \
    src/yangutil/buffer/YangAudioBuffer.cpp \
    src/yangutil/buffer/YangAudioEncoderBuffer.cpp \
    src/yangutil/buffer/YangAudioPlayBuffer.cpp \
    src/yangutil/buffer/YangMediaBuffer.cpp \
    src/yangutil/buffer/YangVideoBuffer.cpp \
    src/yangutil/buffer/YangVideoDecoderBuffer.cpp \
    src/yangutil/buffer/YangVideoEncoderBuffer.cpp \
    src/yangutil/sys/YangIniImpl.cpp



HEADERS += \
    src/yangaudiodev/YangAudioCaptureHandle.h \
    src/yangaudiodev/YangCaptureCallback.h \
    src/yangaudiodev/linux/YangAlsaDeviceHandle.h \
    src/yangaudiodev/linux/YangAlsaHandle.h \
    src/yangaudiodev/linux/YangAudioCaptureLinux.h \
    src/yangaudiodev/linux/YangAudioPlayLinux.h \
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
    src/yangavutil/YangImageConvert.h \
    src/yangcapture/YangScreenCaptureImpl.h \
    src/yangcapture/YangScreenShare.h \
    src/yangcapture/YangVideoCaptureHandle.h \
    src/yangcapture/android/YangCameraAndroid.h \
    src/yangcapture/android/YangVideoCaptureAndroid.h \
    src/yangcapture/linux/YangVideoCaptureLinux.h \
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
    src/yangdecoder/pc/YangH264Header.h \
    src/yangdecoder/pc/YangH264Header1.h \
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
    src/yangencoder/lame.h \
    src/yangp2p/YangP2pHandleImpl.h \
    src/yangp2p/YangP2pMessageHandle.h \
    src/yangp2p/YangP2pRtc.h \
    src/yangp2p/YangP2pRtcIce.h \
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
    src/yangrecord/YangRecordHandle.h \
    src/yangstream/YangStreamSrt.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
