//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_
#define SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_

#include <yangice/YangRtcSocket.h>
#include <yangrtc/YangBandwidth.h>

#include <yangssl/YangSsl.h>
#include <yangssl/YangRtcDtls.h>

#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangVector.h>

#include <yangutil/yangavinfotype.h>
#include <yangstream/YangStreamType.h>

typedef struct{
	uint32_t ssrc;
	uint32_t rtxSsrc;
	uint32_t fecSsrc;
	uint32_t twccId;
} YangRtcTrack;

yang_vector_declare(YangRtcTrack)

typedef struct{
	int32_t twccId;
	uint32_t audioSsrc;
	uint32_t videoSsrc;

	yangbool enableAudioTrack;
	yangbool enableVideoTrack;
	uint8_t audioPayloadType;
	uint8_t opusPayloadType;
	uint8_t pcmaPayloadType;
	uint8_t pcmuPayloadType;

	uint8_t h264PayloadType;
	uint8_t h265PayloadType;

	YangRtcDirection audioDirection;
	YangRtcDirection videoDirection;



	YangAudioCodec audioEncodeCodec;
	YangVideoCodec videoEncodeCodec;

	YangAudioCodec audioDecodeCodec;
	YangVideoCodec videoDecodeCodec;

	YangRtcConnectionState state;


	YangPeerInfo *peerInfo;
	YangPeerCallback *peerCallback;

	YangRtcSocket *sock;

#if Yang_Enable_Dtls
	YangCertificate *cert;
	YangRtcDtls *dtls;
	YangSRtp srtp;
#endif

	YangRtcTrack audioTrack;
	YangRtcTrackVector videoTracks;

	YangBandwidth bandwidth;
	YangRtcStats stats;
#if Yang_Enable_TWCC
	YangRtcTwcc twcc;
#endif

} YangRtcContext;

#endif /* SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_ */
