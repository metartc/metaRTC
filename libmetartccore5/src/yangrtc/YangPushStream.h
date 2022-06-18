//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_
#define YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_

#include <yangrtc/YangPushStreamH.h>
#include <yangrtc/YangRtcContext.h>
#include <yangrtp/YangPublishNackBuffer.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtp/YangRtcpXr.h>
#ifdef __cplusplus
extern "C"{
#endif


void yang_create_rtcpush(YangRtcPush* pub,uint32_t audioSsrc, uint32_t videoSsrc);
void yang_destroy_rtcpush(YangRtcPush* pub);

#ifdef __cplusplus
}
#endif

#endif /* YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_ */
