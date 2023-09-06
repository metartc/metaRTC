//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "YangAudioDeviceMac.h"
#if Yang_OS_APPLE
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




int32_t yang_mac_audio_getDefaultDevice(AudioDeviceID* pids,bool isInput){

    AudioObjectPropertyAddress addr = {
        .mScope = kAudioObjectPropertyScopeGlobal,
        .mElement = kAudioObjectPropertyElementMaster};
    uint32_t        size = sizeof(AudioDeviceID);
    OSStatus      status = noErr;
    AudioDeviceID ids;
    CFStringRef qual = NULL;
    uint32_t qual_size = 0;
    addr.mSelector=isInput?kAudioHardwarePropertyDefaultInputDevice:kAudioHardwarePropertyDefaultOutputDevice;
    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, qual_size, &qual,&size, &ids);
    yang_success(status,"get kAudioHardwarePropertyDefaultInputDevice data size failed failed");

    yang_trace("deault %s device size=%d,id=%u",isInput?"input":"output",size,ids);

    *pids=ids;
    yang_release(qual);
    return Yang_Ok;
}


int32_t yang_mac_audio_init(bool isInput,AudioDeviceID deviceId,uint32_t *sample,uint32_t channel)
{
    OSStatus status=noErr;
    AudioStreamBasicDescription desc;
    memset(&desc,0,sizeof(desc));
    uint32_t size = sizeof(desc);
    AudioObjectPropertyAddress propertyAddress = {
        kAudioDevicePropertyStreamFormat, isInput?kAudioDevicePropertyScopeInput:kAudioDevicePropertyScopeOutput, 0};


    memset(&desc, 0, sizeof(desc));
    status=AudioObjectGetPropertyData(deviceId, &propertyAddress, 0, NULL, &size, &desc);

    yang_success(status,"get output format failed");

    yang_trace("get config sample=%f,channel=%u,renderfmt->mBitsPerChannel=%u",desc.mSampleRate,desc.mChannelsPerFrame,desc.mBitsPerChannel);
    yang_trace("mBytesPerFrame=%u,rmFramesPerPacket=%u",desc.mBytesPerFrame,desc.mFramesPerPacket);

    *sample=(uint32_t)desc.mSampleRate;

    desc.mChannelsPerFrame=channel;
    desc.mBytesPerPacket  = desc.mBytesPerFrame = (desc.mBitsPerChannel / 8) * channel;
    uint32_t bufferCount=(*sample)*channel*4/50;



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

void yang_mac_audio_uninit(bool isInput,AudioDeviceID deviceId){
    AudioObjectPropertyAddress propertyAddress = {
        kAudioDevicePropertyStreamFormat, isInput?kAudioDevicePropertyScopeInput:kAudioDevicePropertyScopeOutput, 0};
    AudioObjectRemovePropertyListener(
          deviceId, &propertyAddress, &yang_objectListenerProc, NULL);
}



#endif
