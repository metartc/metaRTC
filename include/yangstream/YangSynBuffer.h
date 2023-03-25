//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef YANGPLAYER_INCLUDE_YANGAVSYN_H_
#define YANGPLAYER_INCLUDE_YANGAVSYN_H_
#include <yangutil/sys/YangTime.h>

#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include <yangavutil/audio/YangAudioMix.h>
#include <yangutil/buffer/YangVideoBuffer.h>


enum YangSynType{
	YANG_SYNC_AUDIO_MASTER, /* default choice */
	YANG_SYNC_VIDEO_MASTER,
	YANG_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};
#define Yang_Max_Audio_Intervaltime 25
#define Yang_Max_Video_Intervaltime 35
#define Yang_Video_Base_Update_Interval 600000//10*60*1000  //10m
#define Yang_Audio_Base_Update_Interval 5000//10*60*1000  //10m
#define Yang_Video_Cache_time 35
#define Yang_Audio_Cache_time 5



typedef struct{
	void* session;
    uint8_t* (*getVideoRef)(void* session,YangFrame* pframe);
    uint8_t* (*getAudioRef)(void* session,YangFrame* audioFrame);
    void (*setInAudioBuffer)(void* session,YangAudioPlayBuffer *paudioList);
    void (*setInVideoBuffer)(void* session,YangVideoBuffer *pbuf);
    void (*setAudioClock)(void* session,int32_t paudioclock);
    void (*setVideoClock)(void* session,int32_t pvideoclock);
    void (*setUid)(void* session,int32_t uid);
    int32_t (*getUid)(void* session);
    int32_t (*getAudioSize)(void* session);
    int32_t (*getVideoSize)(void* session);
    void (*resetVideoClock)(void* psession);
    void (*resetAudioClock)(void* psession);
    int32_t (*width)(void* psession);
    int32_t (*height)(void* psession);
}YangSynBuffer;



void yang_create_synBuffer(YangSynBuffer* syn);
void yang_destroy_synBuffer(YangSynBuffer* syn);

#endif /* YANGPLAYER_INCLUDE_YANGAVSYN_H_ */
