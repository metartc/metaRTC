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
INCLUDEPATH += $$HOME_BASE/libmetartccore2/src

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
    src/yangavutil/YangAudioMix.c \
    src/yangavutil/YangAudioUtil.c \
    src/yangavutil/YangConvert.c \
    src/yangavutil/YangMeta.c \
    src/yangavutil/YangNalu.c \
    src/yangavutil/YangPreProcess.c \
    src/yangavutil/YangResample.c \
    src/yangavutil/YangRtcAec.c \
    src/yangavutil/YangYuvUtil.c \
    src/yangcsrs/YangSrsConnection.c \
    src/yangcsrs/YangSrsSdp.c \
    src/yangrtp/YangPublishNackBuffer.c \
    src/yangrtp/YangReceiveNackBuffer.c \
    src/yangrtp/YangRtcp.c \
    src/yangrtp/YangRtcpApp.c \
    src/yangrtp/YangRtcpCommon.c \
    src/yangrtp/YangRtcpCompound.c \
    src/yangrtp/YangRtcpNack.c \
    src/yangrtp/YangRtcpPli.c \
    src/yangrtp/YangRtcpPsfbCommon.c \
    src/yangrtp/YangRtcpRR.c \
    src/yangrtp/YangRtcpRpsi.c \
    src/yangrtp/YangRtcpSR.c \
    src/yangrtp/YangRtcpSli.c \
    src/yangrtp/YangRtcpTWCC.c \
    src/yangrtp/YangRtcpXr.c \
    src/yangrtp/YangRtp.c \
    src/yangrtp/YangRtpBuffer.c \
    src/yangrtp/YangRtpFUAPayload.c \
    src/yangrtp/YangRtpFUAPayload2.c \
    src/yangrtp/YangRtpHeader.c \
    src/yangrtp/YangRtpPacket.c \
    src/yangrtp/YangRtpRawPayload.c \
    src/yangrtp/YangRtpRecvNack.c \
    src/yangrtp/YangRtpSTAPPayload.c \
    src/yangsdp/YangAudioPayload.c \
    src/yangsdp/YangCodecPayload.c \
    src/yangsdp/YangMediaDesc.c \
    src/yangsdp/YangMediaPayloadType.c \
    src/yangsdp/YangRedPayload.c \
    src/yangsdp/YangRtcSdp.c \
    src/yangsdp/YangRtxPayloadDes.c \
    src/yangsdp/YangSSRCInfo.c \
    src/yangsdp/YangSdp.c \
    src/yangsdp/YangSdpHandle.c \
    src/yangutil/sys/YangAmf.c \
    src/yangutil/sys/YangAvtype.c \
    src/yangutil/sys/YangBuffer.c \
    src/yangutil/sys/YangCLog.c \
    src/yangutil/sys/YangCString.c \
    src/yangutil/sys/YangCTimer.c \
    src/yangutil/sys/YangEndian.c \
    src/yangutil/sys/YangFile.c \
    src/yangutil/sys/YangHttpSocket.c \
    src/yangutil/sys/YangMath.c \
    src/yangutil/sys/YangSRtp.c \
    src/yangutil/sys/YangSocket.c \
    src/yangutil/sys/YangSsl.c \
    src/yangutil/sys/YangSsrc.c \
    src/yangutil/sys/YangTime.c \
    src/yangutil/sys/YangUrl.c \
    src/yangutil/sys/YangVector.c \
    src/yangwebrtc/YangAec.c \
    src/yangwebrtc/YangH264RecvTrack.c \
    src/yangwebrtc/YangH264RtpEncode.c \
    src/yangwebrtc/YangH265RecvTrack.c \
    src/yangwebrtc/YangH265RtpEncode.c \
    src/yangwebrtc/YangMetaConnection.c \
    src/yangwebrtc/YangPeerConnection.c \
    src/yangwebrtc/YangRecvTrack.c \
    src/yangwebrtc/YangRtcAudioRecvTrack.c \
    src/yangwebrtc/YangRtcConnection.c \
    src/yangwebrtc/YangRtcContext.c \
    src/yangwebrtc/YangRtcDtls.c \
    src/yangwebrtc/YangRtcPlayStream.c \
    src/yangwebrtc/YangRtcPublishStream.c \
    src/yangwebrtc/YangRtcSession.c \
    src/yangwebrtc/YangRtcStun.c \
    src/yangwebrtc/YangStreamHandle.c \
    src/yangwebrtc/YangUdpHandle.c \
    src/yangwebrtc/YangVideoRecvTrack.c


HEADERS += \
    src/yangcsrs/YangSrsConnection.h \
    src/yangcsrs/YangSrsSdp.h \
    src/yangrtp/YangPublishNackBuffer.h \
    src/yangrtp/YangReceiveNackBuffer.h \
    src/yangrtp/YangRtcp.h \
    src/yangrtp/YangRtcpApp.h \
    src/yangrtp/YangRtcpCommon.h \
    src/yangrtp/YangRtcpCompound.h \
    src/yangrtp/YangRtcpNack.h \
    src/yangrtp/YangRtcpPli.h \
    src/yangrtp/YangRtcpPsfbCommon.h \
    src/yangrtp/YangRtcpRR.h \
    src/yangrtp/YangRtcpRpsi.h \
    src/yangrtp/YangRtcpSR.h \
    src/yangrtp/YangRtcpSli.h \
    src/yangrtp/YangRtcpTWCC.h \
    src/yangrtp/YangRtcpXr.h \
    src/yangrtp/YangRtp.h \
    src/yangrtp/YangRtpBuffer.h \
    src/yangrtp/YangRtpConstant.h \
    src/yangrtp/YangRtpFUAPayload.h \
    src/yangrtp/YangRtpFUAPayload2.h \
    src/yangrtp/YangRtpHeader.h \
    src/yangrtp/YangRtpPacket.h \
    src/yangrtp/YangRtpRawPayload.h \
    src/yangrtp/YangRtpRecvNack.h \
    src/yangrtp/YangRtpSTAPPayload.h \
    src/yangsdp/YangAudioPayload.h \
    src/yangsdp/YangCodecPayload.h \
    src/yangsdp/YangMediaDesc.h \
    src/yangsdp/YangMediaPayloadType.h \
    src/yangsdp/YangRedPayload.h \
    src/yangsdp/YangRtcSdp.h \
    src/yangsdp/YangRtxPayloadDes.h \
    src/yangsdp/YangSSRCInfo.h \
    src/yangsdp/YangSdp.h \
    src/yangsdp/YangSdpHandle.h \
    src/yangsdp/YangSdpType.h \
    src/yangwebrtc/YangH264RecvTrack.h \
    src/yangwebrtc/YangH264RtpEncode.h \
    src/yangwebrtc/YangH265RecvTrack.h \
    src/yangwebrtc/YangH265RtpEncode.h \
    src/yangwebrtc/YangRecvTrack.h \
    src/yangwebrtc/YangRtcAudioRecvTrack.h \
    src/yangwebrtc/YangRtcConnection.h \
    src/yangwebrtc/YangRtcContext.h \
    src/yangwebrtc/YangRtcContextH.h \
    src/yangwebrtc/YangRtcDtls.h \
    src/yangwebrtc/YangRtcDtlsH.h \
    src/yangwebrtc/YangRtcEncodeCommon.h \
    src/yangwebrtc/YangRtcPlayStream.h \
    src/yangwebrtc/YangRtcPublishStream.h \
    src/yangwebrtc/YangRtcSession.h \
    src/yangwebrtc/YangRtcSessionH.h \
    src/yangwebrtc/YangRtcStream.h \
    src/yangwebrtc/YangRtcStun.h \
    src/yangwebrtc/YangStreamHandle.h \
    src/yangwebrtc/YangUdpHandle.h \
    src/yangwebrtc/YangUdpHandleH.h \
    src/yangwebrtc/YangVideoRecvTrack.h


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
