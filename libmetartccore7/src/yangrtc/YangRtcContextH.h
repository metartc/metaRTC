//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_
#define SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_

#include <yangice/YangRtcSocket.h>
#include <yangrtc/YangBandwidth.h>

#include <yangssl/YangSsl.h>
#include <yangssl/YangRtcDtls.h>

#include <yangutil/sys/YangSRtp.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangVector.h>

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
	yangbool enableAudioTrack;
	yangbool enableVideoTrack;

	YangAudioCodec audioCodec;
	YangVideoCodec videoCodec;

	YangRtcDirection direction;

	uint32_t audioSsrc;
	uint32_t videoSsrc;
	int32_t state;
	int32_t twccId;

	YangStreamConfig *streamConfig;
	YangAVInfo *avinfo;
	YangRtcSocket *sock;

#if Yang_Enable_Dtls
	YangCertificate *cer;
	YangRtcDtls *dtls;
	YangSRtp srtp;
#endif

	YangStunData stun;
	YangRtcTrack audioTrack;
	YangRtcTrackVector videoTracks;

	YangBandwidth bandwidth;
	YangRtcStats stats;
#if Yang_Enable_TWCC
	YangRtcTwcc twcc;
#endif


} YangRtcContext;

#endif /* SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_ */
