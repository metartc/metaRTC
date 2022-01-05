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
INCLUDEPATH += $$HOME_BASE/libmetartc2/src

unix{
  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE/bin/lib_debug
    }else{

        DESTDIR += $$HOME_BASE/bin/lib_release
    }
}

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/yangavutil/YangMeta.c \
    src/yangavutil/YangNalu.c \
    src/yangavutil/YangYuvUtil.c \
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
    src/yangutil/sys/YangAmf.c \
    src/yangutil/sys/YangAvtype.c \
    src/yangutil/sys/YangBuffer.c \
    src/yangutil/sys/YangCLog.c \
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
    src/yangwebrtc/YangCRtcStun.c \
    src/yangwebrtc/YangH264RecvTrack.c \
    src/yangwebrtc/YangH264RtpEncode.c \
    src/yangwebrtc/YangH265RecvTrack.c \
    src/yangwebrtc/YangH265RtpEncode.c \
    src/yangwebrtc/YangRecvTrack.c \
    src/yangwebrtc/YangRtcAudioRecvTrack.c \
    src/yangwebrtc/YangRtcConnection.c \
    src/yangwebrtc/YangRtcContext.c \
    src/yangwebrtc/YangRtcDtls.c \
    src/yangwebrtc/YangRtcPlayStream.c \
    src/yangwebrtc/YangRtcPublishStream.c \
    src/yangwebrtc/YangRtcSession.c \
    src/yangwebrtc/YangUdpHandle.c \
    src/yangwebrtc/YangVideoRecvTrack.c


HEADERS +=

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
