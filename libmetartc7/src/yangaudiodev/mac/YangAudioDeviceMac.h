//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef YANGAUDIODEVICEMAC_H
#define YANGAUDIODEVICEMAC_H
#include <yangutil/yangtype.h>
#if Yang_OS_APPLE

//#include <CoreFoundation/CFString.h>
//#include <AudioUnit/AudioUnit.h>
#include <CoreAudio/CoreAudio.h>
#include <yangutil/sys/YangLog.h>




#define yang_release(x) if(x){CFRelease(x);x=NULL;}
#define yang_success(a,b) if(a!=noErr){yang_error("%s(%d)",b,(int)a);return 1;}


int32_t yang_mac_audio_getDefaultDevice(AudioDeviceID* pids,bool isInput);
int32_t yang_mac_audio_init(bool isInput,AudioDeviceID deviceId,uint32_t *sample,uint32_t channel);
void yang_mac_audio_uninit(bool isInput,AudioDeviceID deviceId);
#endif
#endif // YANGAUDIODEVICEMAC_H
