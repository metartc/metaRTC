//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_
#define SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_

#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangRtcDtls.h>
#include <yangrtc/YangRtcUdp.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangSsl.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/sys/YangVector.h>


typedef struct{
	uint32_t ssrc;
	uint32_t rtxSsrc;
	uint32_t fecSsrc;
	uint32_t twccId;
} YangRtcTrack;

yang_vector_declare(YangRtcTrack)

typedef struct{
	char *data;
	int32_t nb;
} YangStunData;

typedef struct{
	YangVideoCodec codec;
	uint32_t audioSsrc;
	uint32_t videoSsrc;
	int32_t state;
	int32_t twccId;

	YangStreamConfig *streamConfig;
	YangAVInfo *avinfo;
	YangRtcUdp *udp;

#if Yang_HaveDtls
	YangCertificate *cer;
	YangRtcDtls *dtls;
	YangSRtp srtp;
#endif

	YangStunData stun;
	YangRtcTrack audioTrack;
	YangRtcTrackVector videoTracks;

	YangBandwidth bandwidth;
	YangRtcStats stats;
#if Yang_Using_TWCC
	YangRtcTwcc twcc;
#endif


} YangRtcContext;

#endif /* SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_ */
