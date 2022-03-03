//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGWEBRTC_YANGAVCONTEXT_H_
#define INCLUDE_YANGWEBRTC_YANGAVCONTEXT_H_
#include <yangutil/yangavtype.h>
#include <yangutil/yangavinfotype.h>
#include <yangstream/YangStreamType.h>
typedef struct{
	void (*receiveAudio)(YangFrame *audioFrame, void *user);
	void (*receiveVideo)(YangFrame *videoFrame, void *user);
} YangRtcReceiveCallback;

typedef struct{
	YangAVInfo *avinfo;
	void (*sendRequest)(int32_t puid, uint32_t ssrc, YangRequestType req,void* user);
	void (*setPlayMediaConfig)(	YangAudioParam *remote_audio,YangVideoParam *remote_video,void* user);
	YangRtcReceiveCallback recvcb;
} YangAVContext;


#endif /* INCLUDE_YANGWEBRTC_YANGAVCONTEXT_H_ */
