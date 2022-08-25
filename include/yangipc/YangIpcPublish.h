//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangP2pPublishJz_H
#define YangP2pPublishJz_H

#include <yangipc/YangIpcEncoder.h>
#include <yangipc/YangIpcPublish.h>
#include <yangipc/YangIpcRtc.h>
#include "yangutil/sys/YangIni.h"
#include <yangutil/buffer2/YangVideoEncoderBuffer2.h>



typedef struct{
	int32_t hasAudio;
	YangEncoderVideo encoder;
	YangIpcRtc rtc;

}YangIpcPublishSession;

typedef struct{
	YangIpcPublishSession session;
	void (*init)(YangIpcPublishSession* session);

	void (*startAudioEncoding)(YangIpcPublishSession* session);
	void (*startVideoEncoding)(YangIpcPublishSession* session);

	void (*stopAudioEncoding)(YangIpcPublishSession* session);
	void (*stopVideoEncoding)(YangIpcPublishSession* session);

	void (*startRtc)(YangIpcPublishSession* session);
	void (*stopRtc)(YangIpcPublishSession* session);

	int32_t (*addPeer)(YangIpcPublishSession* session,char* sdp,char* answer,char* remoteIp,int32_t localPort,int* phasplay);
	void (*sendRequest)(YangIpcPublishSession* session,int32_t puid,uint32_t ssrc,YangRequestType req);

}YangIpcPublish;


void yang_create_ipcPublish(YangIpcPublish* publish,YangAVInfo* avinfo);
void yang_destroy_ipcPublish(YangIpcPublish* publish);

#endif //
