#ifndef YANGAUDIODEVICEMAC_H
#define YANGAUDIODEVICEMAC_H

#if defined(__APPLE__)
#include <yangutil/yangtype.h>
#include <CoreFoundation/CFString.h>
#include <AudioUnit/AudioUnit.h>
#include <CoreAudio/CoreAudio.h>
#include <yangutil/sys/YangLog.h>




#define yang_release(x) if(x){CFRelease(x);x=NULL;}
#define yang_success(a,b) if(a!=noErr){yang_error("%s(%d)",b,(int)a);return 1;}



void yang_mac_printf(char* s,CFStringRef macstr);
int32_t yang_mac_getInputAudioIds(AudioDeviceID** pids);
int32_t yang_mac_getOutputAudioIds(AudioDeviceID** pids);
int32_t yang_mac_initAudioFormat(bool isinput,AudioUnit punit,uint32_t* sample,uint32_t channel);
int32_t yang_mac_audio_getDefaultDevice(AudioDeviceID* pids,bool isInput);
int32_t yang_mac_audio_initBuffer(AudioUnit  punit,AudioDeviceID  deviceId,AudioBufferList** bufferList);
int32_t yang_mac_audio_initHooks(bool isInput,AudioUnit  punit,AudioDeviceID  deviceId,void* user);
int32_t yang_mac_audio_uninit(bool isInput,AudioUnit  punit,AudioDeviceID  deviceId,void* user);
OSStatus yang_input_callback(void *data,
                  AudioUnitRenderActionFlags *action_flags,
                  const AudioTimeStamp *ts_data, UInt32 bus_num,
                  UInt32 frames, AudioBufferList *ignored_buffers);
#endif
#endif // YANGAUDIODEVICEMAC_H
