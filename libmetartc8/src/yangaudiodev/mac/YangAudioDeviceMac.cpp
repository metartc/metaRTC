//
// Copyright (c) 2019-2026 yanggaofeng
//
#include "YangAudioDeviceMac.h"
#if Yang_OS_MAC
#include <yangutil/sys/YangLog.h>

static OSStatus yang_objectListenerProc(
    AudioObjectID id,
    UInt32 num_addresses,
    const AudioObjectPropertyAddress addresses[],
    void* user) {
  //YangMacAudioSession* session = (YangMacAudioSession*)clientData;

  (void)id;
  (void)num_addresses;
  (void)addresses;
  (void)user;

  return 0;
}

int32_t yang_mac_audio_getDefaultDevice(AudioDeviceID* pids,yangbool isInput){

    AudioObjectPropertyAddress addr = {
        .mScope = kAudioObjectPropertyScopeGlobal,
        .mElement = kAudioObjectPropertyElementMain};//kAudioObjectPropertyElementMaster};
    uint32_t  size = sizeof(AudioDeviceID);
    OSStatus  status = noErr;
    AudioDeviceID ids;

    addr.mSelector=isInput?kAudioHardwarePropertyDefaultInputDevice:kAudioHardwarePropertyDefaultOutputDevice;

    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, NULL,&size, &ids);
    yang_success(status,"get kAudioHardwarePropertyDefaultInputDevice data size failed");

    if(ids==kAudioDeviceUnknown){
    	yang_error("get mac audio device failed");
    	return 1;
    }

    yang_trace("\ndefault %s device size=%d,id=%u",isInput?"input":"output",size,ids);

    *pids=ids;
    return Yang_Ok;
}


int32_t yang_mac_audio_init(yangbool isInput,AudioDeviceID deviceId,uint32_t *psample,uint32_t channel)
{
    OSStatus status=noErr;
    uint32_t sample=48000;
   // uint32_t channel=2;
    AudioStreamBasicDescription desc;
    memset(&desc,0,sizeof(desc));
    uint32_t size = sizeof(desc);
    AudioObjectPropertyAddress propertyAddress = {
        kAudioDevicePropertyStreamFormat, isInput?kAudioDevicePropertyScopeInput:kAudioDevicePropertyScopeOutput, 0};

    memset(&desc, 0, sizeof(desc));
    status=AudioObjectGetPropertyData(deviceId, &propertyAddress, 0, NULL, &size, &desc);

	if(status!=noErr){
		yang_error("get mac %s format failed,sample=%d,channel=%d",isInput?(char*)"capture":(char*)"render",sample,channel);
		return 1;
	}
	*psample=sample;

    yang_trace("\nget config sample=%f,channel=%u,renderfmt->mBitsPerChannel=%u",desc.mSampleRate,desc.mChannelsPerFrame,desc.mBitsPerChannel);
    yang_trace("mBytesPerFrame=%u,rmFramesPerPacket=%u",desc.mBytesPerFrame,desc.mFramesPerPacket);

    sample=(uint32_t)desc.mSampleRate;

    desc.mChannelsPerFrame=channel;
    desc.mBytesPerPacket  = desc.mBytesPerFrame = (desc.mBitsPerChannel / 8) * channel;

    uint32_t bufferCount=sample*channel*4/50;
    propertyAddress.mSelector = kAudioDevicePropertyBufferSizeRange;

    AudioValueRange range;
     size = sizeof(range);
    status=AudioObjectGetPropertyData(deviceId, &propertyAddress, 0, NULL, &size, &range);
    yang_success(status,"get output format range failed");
    if (range.mMinimum > bufferCount) {
      bufferCount = range.mMinimum;
    } else if (range.mMaximum < bufferCount) {
      bufferCount = range.mMaximum;
    }


    propertyAddress.mSelector = kAudioDevicePropertyBufferSize;
    size = sizeof(bufferCount);
    status=AudioObjectSetPropertyData(deviceId, &propertyAddress, 0, NULL, size, &bufferCount);
     yang_success(status,"set output format buffer size failed");

    propertyAddress.mSelector = kAudioDeviceProcessorOverload;
    AudioObjectAddPropertyListener(deviceId, &propertyAddress, &yang_objectListenerProc, NULL);

    return Yang_Ok;
}

void yang_mac_audio_uninit(yangbool isInput,AudioDeviceID deviceId){
    AudioObjectPropertyAddress propertyAddress = {
        kAudioDevicePropertyStreamFormat, isInput?kAudioDevicePropertyScopeInput:kAudioDevicePropertyScopeOutput, 0};
    AudioObjectRemovePropertyListener(
          deviceId, &propertyAddress, &yang_objectListenerProc, NULL);
}



#endif
