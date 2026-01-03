//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef YANGAUDIOMAC_H
#define YANGAUDIOMAC_H
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>

#if Yang_OS_APPLE

typedef struct{
	void* user;
	void (*on_audio)(uint8_t* data,uint32_t nb,void* user);
	YangFrame* (*getRenderData)(void* user);
}YangMacAudioCallback;

typedef struct{
	void* session;
	int32_t (*init)(void* session);
	int32_t (*start)(void* session);
	int32_t (*stop)(void* session);
	void (*setAudioCallback)(void* session,YangMacAudioCallback* callback);
	void (*on_audio)(void* session,uint8_t* data,uint32_t nb);
	uint32_t (*getSample)(void* session);
	uint32_t (*getChannel)(void* session);
}YangAudioMac;

#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_create_audioMac(YangAudioMac* audio,yangbool isInput,uint32_t sample,uint32_t channel);
void yang_destroy_audioMac(YangAudioMac* audio);

#ifdef __cplusplus
}
#endif

#endif
#endif // YANGAUDIOMAC_H
