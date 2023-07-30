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
INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/thirdparty/include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/gmssl
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include/mbedtls3
INCLUDEPATH += $$HOME_BASE/libmetartccore7/src

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
    src/yangice/YangIce.c \
    src/yangice/YangRtcSocket.c \
    src/yangice/YangRtcStun.c \
    src/yangice/YangTurnConnection.c \
    src/yangjson/YangJson.c \
    src/yangjson/YangJsonImpl.c \
    src/yangp2p/YangP2pConnection.c \
    src/yangp2p/YangP2pConnection2.c \
    src/yangrtc/YangBandwidth.c \
    src/yangrtc/YangDatachannel.c \
    src/yangrtc/YangPeerConnection.c \
    src/yangrtc/YangPlayStream.c \
    src/yangrtc/YangPlayTrack.c \
    src/yangrtc/YangPlayTrackAudio.c \
    src/yangrtc/YangPlayTrackH264.c \
    src/yangrtc/YangPlayTrackH265.c \
    src/yangrtc/YangPlayTrackVideo.c \
    src/yangrtc/YangPush.c \
    src/yangrtc/YangPushAudio.c \
    src/yangrtc/YangPushH264.c \
    src/yangrtc/YangPushH265.c \
    src/yangrtc/YangPushStream.c \
    src/yangrtc/YangRtcConnection.c \
    src/yangrtc/YangRtcContext.c \
    src/yangrtc/YangRtcRtcp.c \
    src/yangrtc/YangRtcStats.c \
    src/yangrtc/YangRtcTwcc.c \
    src/yangrtc/YangStreamUrl.c \
    src/yangrtp/YangPublishNackBuffer.c \
    src/yangrtp/YangReceiveNackBuffer.c \
    src/yangrtp/YangRtcp.c \
    src/yangrtp/YangRtcpApp.c \
    src/yangrtp/YangRtcpBye.c \
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
    src/yangssl/YangDtlsMbedtls.c \
    src/yangssl/YangDtlsOpenssl.c \
    src/yangssl/YangSslMbedtls.c \
    src/yangssl/YangSslOpenssl.c \
    src/yangstream/YangStream.c \
    src/yangstream/YangStreamCapture.c \
    src/yangstream/YangStreamRtc.c \
    src/yangutil/buffer2/YangMediaBuffer2.c \
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
    src/yangutil/sys/YangSRtp.c \
    src/yangutil/sys/YangSctp.c \
    src/yangutil/sys/YangSocket.c \
    src/yangutil/sys/YangSocketIP.c \
    src/yangutil/sys/YangSsrc.c \
    src/yangutil/sys/YangThread.c \
    src/yangutil/sys/YangTime.c \
    src/yangutil/sys/YangUrl.c \
    src/yangutil/sys/YangVector.c \
    src/yangutil/sys/YangWebsocket.c \
    src/yangwhip/YangWhip.c \
    src/yangzlm/YangZlmConnection.c


HEADERS += \
    src/yangice/YangIce.h \
    src/yangice/YangRtcSocket.h \
    src/yangice/YangRtcStun.h \
    src/yangice/YangTurnConnection.h \
    src/yangjson/YangJsonImpl.h \
    src/yangmqtt/YangMqttImpl.h \
    src/yangp2p/YangP2pConnection.h \
    src/yangp2p/YangP2pConnection2.h \
    src/yangrtc/YangBandwidth.h \
    src/yangrtc/YangDatachannel.h \
    src/yangrtc/YangPlay.h \
    src/yangrtc/YangPlayStream.h \
    src/yangrtc/YangPlayTrack.h \
    src/yangrtc/YangPlayTrackAudio.h \
    src/yangrtc/YangPlayTrackH.h \
    src/yangrtc/YangPlayTrackH264.h \
    src/yangrtc/YangPlayTrackH265.h \
    src/yangrtc/YangPlayTrackVideo.h \
    src/yangrtc/YangPush.h \
    src/yangrtc/YangPushAudio.h \
    src/yangrtc/YangPushH.h \
    src/yangrtc/YangPushH264.h \
    src/yangrtc/YangPushH265.h \
    src/yangrtc/YangPushStream.h \
    src/yangrtc/YangPushStreamH.h \
    src/yangrtc/YangRtcConnection.h \
    src/yangrtc/YangRtcContext.h \
    src/yangrtc/YangRtcContextH.h \
    src/yangrtc/YangRtcRtcp.h \
    src/yangrtc/YangRtcSession.h \
    src/yangrtc/YangRtcStats.h \
    src/yangrtc/YangRtcStream.h \
    src/yangrtc/YangRtcTwcc.h \
    src/yangrtc/YangStreamUrl.h \
    src/yangrtp/YangPublishNackBuffer.h \
    src/yangrtp/YangReceiveNackBuffer.h \
    src/yangrtp/YangRtcp.h \
    src/yangrtp/YangRtcpApp.h \
    src/yangrtp/YangRtcpBye.h \
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
    src/yangssl/YangRtcDtls.h \
    src/yangstream/YangStreamRtc.h \
    src/yangutil/buffer2/YangSortBuffer.h \
    src/yangutil/sys/YangSctp.h \
    src/yangzlm/YangZlmConnection.h


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
