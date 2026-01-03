//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef YANGAUDIODEVICEMAC_H
#define YANGAUDIODEVICEMAC_H
#include <yangutil/yangtype.h>
#if Yang_OS_APPLE

#include <AudioUnit/AudioUnit.h>
#include <yangutil/sys/YangLog.h>

#define Yang_Enable_MacAudioFloat 0

typedef struct{
	yangbool isInput;
	AudioUnit audioUnit;
	struct AURenderCallbackStruct callback;
}YangIosAudio;

#define yang_release(x) if(x){CFRelease(x);x=NULL;}
#define yang_success(a,b) if(a!=noErr){yang_error("%s(%d)",b,(int)a);return 1;}

int32_t yang_ios_init_AudioSession(uint32_t *sample,int32_t audioType);
int32_t yang_ios_audio_init(YangIosAudio* audio,uint32_t sample,uint32_t channel);
void yang_ios_audio_uninit(YangIosAudio* audio);

#endif
#endif // YANGAUDIODEVICEMAC_H
