//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_
#define SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_

#include <yangrtc/YangRtcDtlsH.h>
#include <yangrtc/YangRtcStream.h>
#include <yangrtc/YangUdpHandleH.h>
#include <yangrtc/YangBandwidth.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangSsl.h>
#include <yangstream/YangStreamType.h>



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
	YangUdpHandle *udp;

#if Yang_HaveDtls
	YangCertificate *cer;
	YangRtcDtls *dtls;
	YangSRtp srtp;
#endif

	YangStunData stun;
	YangRtcTrack audioTrack;
	YangRtcTrackVector videoTracks;

	YangBandwidth bandwidth;
	YangRemoteRecvStats rrStats;
#if Yang_Using_TWCC
	YangTwcc twcc;
#endif


} YangRtcContext;

#endif /* SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_ */
