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
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/gmssl
INCLUDEPATH += $$HOME_BASE/libmetartccore4/src

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
    src/yangavutil/YangSpspps.c \
    src/yangavutil/YangYuvUtil.c \
    src/yangp2p/YangP2pConnection.c \
    src/yangp2p/YangP2pSdp.c \
    src/yangp2p/YangP2pServer.c \
    src/yangp2p/YangUdpServer.c \
    src/yangrtc/YangAec.c \
    src/yangrtc/YangBandwidth.c \
    src/yangrtc/YangDatachannel.c \
    src/yangrtc/YangH264RecvTrack.c \
    src/yangrtc/YangH264RtpEncode.c \
    src/yangrtc/YangH265RecvTrack.c \
    src/yangrtc/YangH265RtpEncode.c \
    src/yangrtc/YangMetaConnection.c \
    src/yangrtc/YangPeerConnection.c \
    src/yangrtc/YangRecvTrack.c \
    src/yangrtc/YangRtcAudioRecvTrack.c \
    src/yangrtc/YangRtcConnection.c \
    src/yangrtc/YangRtcContext.c \
    src/yangrtc/YangRtcDtls.c \
    src/yangrtc/YangRtcHandle.c \
    src/yangrtc/YangRtcPlayStream.c \
    src/yangrtc/YangRtcPublishStream.c \
    src/yangrtc/YangRtcSession.c \
    src/yangrtc/YangRtcStats.c \
    src/yangrtc/YangRtcStun.c \
    src/yangrtc/YangStreamHandle.c \
    src/yangrtc/YangUdpHandle.c \
    src/yangrtc/YangVideoRecvTrack.c \
    src/yangrtmp/YangRtmp.c \
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
    src/yangsdp/YangAnswerSdp.c \
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
    src/yangsrs/YangSrsConnection.c \
    src/yangsrs/YangSrsSdp.c \
    src/yangstream/YangStream.c \
    src/yangstream/YangStreamCapture.c \
    src/yangstream/YangStreamRtc.c \
    src/yangstream/YangStreamRtmp.c \
    src/yangutil/sys/YangAmf.c \
    src/yangutil/sys/YangAvtype.c \
    src/yangutil/sys/YangBuffer.c \
    src/yangutil/sys/YangCLog.c \
    src/yangutil/sys/YangCString.c \
    src/yangutil/sys/YangCTimer.c \
    src/yangutil/sys/YangEndian.c \
    src/yangutil/sys/YangFile.c \
    src/yangutil/sys/YangHttpSocket.c \
    src/yangutil/sys/YangLibHandle.c \
    src/yangutil/sys/YangMath.c \
    src/yangutil/sys/YangSRtp.c \
    src/yangutil/sys/YangSctp.c \
    src/yangutil/sys/YangSocket.c \
    src/yangutil/sys/YangSsl.c \
    src/yangutil/sys/YangSsrc.c \
    src/yangutil/sys/YangTime.c \
    src/yangutil/sys/YangUrl.c \
    src/yangutil/sys/YangVector.c \
    src/yangutil/sys/YangWebsocket.c \
    src/yangzlm/YangZlmConnection.c \
    src/yangzlm/YangZlmSdp.c


HEADERS += \
    src/yangp2p/YangP2pConnection.h \
    src/yangp2p/YangP2pSdp.h \
    src/yangp2p/YangUdpServer.h \
    src/yangp2p/YangUdpServerH.h \
    src/yangrtc/YangDatachannel.h \
    src/yangrtc/YangH264RecvTrack.h \
    src/yangrtc/YangH264RtpEncode.h \
    src/yangrtc/YangH265RecvTrack.h \
    src/yangrtc/YangH265RtpEncode.h \
    src/yangrtc/YangRecvTrack.h \
    src/yangrtc/YangRtcAudioRecvTrack.h \
    src/yangrtc/YangRtcConnection.h \
    src/yangrtc/YangRtcContext.h \
    src/yangrtc/YangRtcContextH.h \
    src/yangrtc/YangRtcDtls.h \
    src/yangrtc/YangRtcDtlsH.h \
    src/yangrtc/YangRtcEncodeCommon.h \
    src/yangrtc/YangRtcPlayStream.h \
    src/yangrtc/YangRtcPublishStream.h \
    src/yangrtc/YangRtcSession.h \
    src/yangrtc/YangRtcSessionH.h \
    src/yangrtc/YangRtcStream.h \
    src/yangrtc/YangRtcStun.h \
    src/yangrtc/YangStreamHandle.h \
    src/yangrtc/YangUdpHandle.h \
    src/yangrtc/YangUdpHandleH.h \
    src/yangrtc/YangVideoRecvTrack.h \
    src/yangrtmp/YangRtmp.h \
    src/yangrtmp/YangRtmp2.h \
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
    src/yangsdp/YangAnswerSdp.h \
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
    src/yangsrs/YangSrsConnection.h \
    src/yangsrs/YangSrsSdp.h \
    src/yangstream/YangStreamRtc.h \
    src/yangstream/YangStreamRtmp.h \
    src/yangutil/sys/YangSctp.h \
    src/yangzlm/YangZlmConnection.h \
    src/yangzlm/YangZlmSdp.h


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
