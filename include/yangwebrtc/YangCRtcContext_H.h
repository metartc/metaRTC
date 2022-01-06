/*
 * YangCRtcontext_H.h
 *
 *  Created on: 2021年12月28日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_
#define SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_
#include <yangutil/yangavtype.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangSsl.h>
#include <yangstream/YangStreamType.h>
#include <yangwebrtc/YangUdpHandle.h>
#include <yangwebrtc/YangRtcDtls.h>
#include <yangwebrtc/YangRtcStream.h>

typedef struct{
	void (*receiveAudio)(YangFrame *audioFrame, void *user);
	void (*receiveVideo)(YangFrame *videoFrame, void *user);
} YangRtcReceiveCallback;

typedef struct{
	YangAudioInfo *audio;
	YangVideoInfo *video;
	//YangVideoEncInfo* enc;
	YangSysInfo *sys;
	YangRtcInfo *rtc;
	void (*yang_sendRequest)(int32_t puid, uint32_t ssrc, YangRequestType req);
} YangAVContex;
typedef struct{
	// track_id
	uint32_t ssrc_;
	uint32_t rtx_ssrc_;
	uint32_t fec_ssrc_;
	int32_t twcc_id;
} YangRtcTrack;

typedef struct{
	char *data;
	int32_t nb;
} YangStunData;

typedef struct{
	YangVideoCodec codec;

	YangAudioParam *remote_audio;
	YangVideoParam *remote_video;

	uint32_t audioSsrc;
	uint32_t videoSsrc;
	int32_t state;
	int32_t twcc_id;

	YangStreamConfig *streamConf;
	YangAVContex *context;
	YangCertificate *cer;
	YangSRtp srtp;
	YangStunData stun;
	YangRtcTrack audioTrack;
	YangRtcTrack videoTrack;

	YangUdpHandle *udp;
	YangRtcDtls *dtls;
	YangRtcReceiveCallback recvcb;
	void *user;
} YangRtcContext;

#endif /* SRC_YANGWEBRTC_YANGCRTCCONTEXT_H_H_ */
