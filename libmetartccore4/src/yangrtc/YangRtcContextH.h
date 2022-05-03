//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_
#define SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_
#include <yangrtc/YangAVContext.h>
#include <yangrtc/YangRtcDtlsH.h>
#include <yangrtc/YangRtcStream.h>
#include <yangrtc/YangUdpHandleH.h>

#include <yangutil/yangavtype.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangSsl.h>
#include <yangstream/YangStreamType.h>



typedef struct{
	uint32_t ssrc;
	uint32_t rtx_ssrc;
	uint32_t fec_ssrc;
	int32_t twcc_id;
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
	int32_t twcc_id;
	YangStreamConfig *streamConf;
	YangAVContext *avcontext;
#if Yang_HaveDtls
	YangCertificate *cer;
	YangRtcDtls *dtls;
	YangSRtp srtp;
#endif

	YangStunData stun;
	YangRtcTrack audioTrack;
	YangRtcTrackVector videoTracks;
	YangUdpHandle *udp;
	void *user;
} YangRtcContext;

#endif /* SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_ */
