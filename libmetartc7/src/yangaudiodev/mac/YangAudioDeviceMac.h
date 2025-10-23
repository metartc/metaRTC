//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef YANGAUDIODEVICEMAC_H
#define YANGAUDIODEVICEMAC_H
#include <yangutil/yangtype.h>
#if Yang_OS_MAC

#include <CoreAudio/CoreAudio.h>
#include <yangutil/sys/YangLog.h>

#define Yang_Enable_MacAudioFloat 1

#define yang_release(x) if(x){CFRelease(x);x=NULL;}
#define yang_success(a,b) if(a!=noErr){yang_error("%s(%d)",b,(int)a);return 1;}


int32_t yang_mac_audio_getDefaultDevice(AudioDeviceID* pids,yangbool isInput);
int32_t yang_mac_audio_init(yangbool isInput,AudioDeviceID deviceId,uint32_t *sample,uint32_t channel);
void yang_mac_audio_uninit(yangbool isInput,AudioDeviceID deviceId);
#endif
#endif // YANGAUDIODEVICEMAC_H
