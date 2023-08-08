#include "YangAudioDeviceMac.h"
#if defined(__APPLE__)
#include <yangutil/sys/YangLog.h>

char * MYCFStringCopyUTF8String(CFStringRef aString) {
    if (aString == NULL) {
        return NULL;
    }
    CFIndex length = CFStringGetLength(aString);
    CFIndex maxSize =
            CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    char *buffer = (char *)malloc(maxSize);
    if (CFStringGetCString(aString, buffer, maxSize,
                           kCFStringEncodingUTF8)) {
        return buffer;
    }
    free(buffer);
    return NULL;
}

void yang_mac_printf(char* s,CFStringRef macstr){
    if(macstr==NULL) return;
    char* str= MYCFStringCopyUTF8String(macstr);
    yang_trace("%s%s",s,str);

    if(str) free(str);
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

int32_t yang_mac_getInputAudioIds(AudioDeviceID** pids){
    AudioObjectPropertyAddress addr = {kAudioHardwarePropertyDevices,kAudioObjectPropertyScopeGlobal,kAudioObjectPropertyElementMaster};

    uint32_t        size = 0;
    uint32_t        count;
    OSStatus      status
            ;
    AudioDeviceID *ids;
    status = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &addr,
                                          0, NULL, &size);
    yang_success(status,"get kAudioObjectSystemObject data size failed");

    ids   =(AudioDeviceID *) malloc(size);
    count = size / sizeof(AudioDeviceID);

    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr,
                                      0, NULL, &size, ids);
    yang_success(status,"get kAudioObjectSystemObject data ids  failed");


    for (uint32_t i = 0; i < count; i++)
    {
        AudioObjectPropertyAddress propertyAddress;
        propertyAddress.mSelector   = kAudioDevicePropertyStreams;
        propertyAddress.mScope      = kAudioDevicePropertyScopeInput;//与输入端连
        uint32_t dataSize             = 0;
        OSStatus status   = AudioObjectGetPropertyDataSize(ids[i], &propertyAddress, 0,NULL,&dataSize);

        if(status!=noErr){
          //  kAudioHardwareBadDeviceError
            yang_error("get input device size fail!");
            continue;
        }
        uint32_t streamCount          = dataSize / sizeof(AudioStreamID);

        if (streamCount > 0)
        {

            size = sizeof(CFStringRef);

            propertyAddress.mSelector = kAudioDevicePropertyDeviceUID;
            propertyAddress.mScope      = kAudioDevicePropertyScopeInput;
            CFStringRef cf_uid   = NULL;
            status = AudioObjectGetPropertyData(ids[i], &propertyAddress, 0, NULL, &size, &cf_uid);
            yang_success(status,"get kAudioObjectSystemObject data uid  failed");
            yang_mac_printf((char*)"device uid=",cf_uid);
            yang_release(cf_uid);
            CFStringRef cf_name   = NULL;
            propertyAddress.mSelector = kAudioDevicePropertyDeviceNameCFString;
            status = AudioObjectGetPropertyData(ids[i], &propertyAddress, 0, NULL, &size, &cf_name);
            yang_success(status,"get kAudioObjectSystemObject data cfname failed");

            yang_mac_printf((char*)"device name=",cf_name);
            yang_release(cf_name);

        }
    }
    *pids=ids;

    return Yang_Ok;
}

int32_t yang_mac_getOutputAudioIds(AudioDeviceID** pids){
    AudioObjectPropertyAddress addr = {kAudioHardwarePropertyDevices,kAudioObjectPropertyScopeGlobal,kAudioObjectPropertyElementMaster};

    uint32_t        size = 0;
    uint32_t        count;
    OSStatus      status;
    AudioDeviceID *ids;

    status = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &addr, 0, NULL, &size);
    yang_success(status,"get kAudioObjectSystemObject data size  failed");

    ids   =(AudioDeviceID *) malloc(size);
    count = size / sizeof(AudioDeviceID);
    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr,
                                      0, NULL, &size, ids);
    yang_success(status,"get input ids failed");



    for (uint32_t i = 0; i < count; i++)
    {
        AudioObjectPropertyAddress propertyAddress;
        propertyAddress.mScope  = kAudioDevicePropertyScopeOutput;
        propertyAddress.mSelector   = kAudioDevicePropertyStreams;
        uint32_t   dataSize   = 0;
        OSStatus   status    = AudioObjectGetPropertyDataSize(ids[i], &propertyAddress, 0,NULL,&dataSize);
        yang_success(status,"get input device size failed");
        uint32_t  streamCount = dataSize / sizeof(AudioStreamID);

        if (streamCount > 0)
        {

            size = sizeof(CFStringRef);
            yang_trace("Device is output device\n");
            AudioObjectPropertyAddress propertyAddressInfo;
            propertyAddress.mScope      = kAudioDevicePropertyScopeOutput;
            propertyAddressInfo.mSelector = kAudioDevicePropertyDeviceUID;
            CFStringRef cf_uid   = NULL;
            status = AudioObjectGetPropertyData(ids[i], &propertyAddressInfo, 0, NULL, &size, &cf_uid);
            yang_success(status,"get input uid failed");
            yang_mac_printf((char*)"device uid=",cf_uid);
            yang_release(cf_uid);

            propertyAddress.mSelector = kAudioDevicePropertyDeviceNameCFString;
            CFStringRef cf_name   = NULL;
            status = AudioObjectGetPropertyData(ids[i], &propertyAddress, 0, NULL, &size, &cf_name);
            yang_success(status,"get devide name failed");


            yang_mac_printf((char*)"device name=",cf_name);
            yang_release(cf_name);
        }
    }
    *pids=ids;
    return Yang_Ok;
}

int32_t yang_mac_initAudioFormat(bool isInput,AudioUnit punit,uint32_t *sample,uint32_t channel)
{
    AudioStreamBasicDescription desc;
    OSStatus status=noErr;
    uint32_t size = sizeof(desc);

    status = AudioUnitGetProperty(punit, kAudioUnitProperty_StreamFormat,
                isInput?kAudioUnitScope_Input:kAudioUnitScope_Output, isInput?1:0, &desc, &size);
    yang_success(status,"get input format failed");

    yang_trace("get config sample=%f,channel=%u,desc.mBitsPerChannel=%u",desc.mSampleRate,desc.mChannelsPerFrame,desc.mBitsPerChannel);
    yang_trace("desc.mBytesPerFrame=%u,desc.mFramesPerPacket=%u",desc.mBytesPerFrame,desc.mFramesPerPacket);
    //desc.mSampleRate=sample;
   // if(!isInput){
      //  desc.mSampleRate=sample;
   // }
    *sample=(uint32_t)desc.mSampleRate;

    desc.mChannelsPerFrame=channel;
    desc.mBytesPerPacket  = desc.mBytesPerFrame = (desc.mBitsPerChannel / 8) * channel;

    status = AudioUnitSetProperty(punit, kAudioUnitProperty_StreamFormat,
                isInput?kAudioUnitScope_Output:kAudioUnitScope_Input, isInput?1:0, &desc, size);
    yang_success(status,"set capture  format failed");


    if (desc.mFormatID != kAudioFormatLinearPCM) {
        yang_error( "capture audio format is not PCM");
        return 1;
    }

    return Yang_Ok;
}


int32_t yang_mac_audio_initBuffer(AudioUnit  punit,AudioDeviceID  deviceId,AudioBufferList** bufferList)
{
    uint32_t buf_size = 0;
    uint32_t size = 0;
    uint32_t frames = 0;
    OSStatus status;

    AudioObjectPropertyAddress addr = {
        kAudioDevicePropertyStreamConfiguration,
        kAudioDevicePropertyScopeInput,
        kAudioObjectPropertyElementMaster};

    status = AudioObjectGetPropertyDataSize(deviceId, &addr, 0, NULL, &buf_size);
    yang_success(status,"capture audio get list size");

    size = sizeof(frames);
    status = AudioUnitGetProperty(punit, kAudioDevicePropertyBufferFrameSize,
                kAudioUnitScope_Global, 0, &frames, &size);
    yang_success(status,"capture audio buffer get frame size fail");
    AudioBufferList* buf_list = (AudioBufferList*)yang_calloc(sizeof(AudioBufferList),1);

    status = AudioObjectGetPropertyData(deviceId, &addr, 0, NULL,
                      &buf_size, buf_list);
    yang_success(status,"capture audio buffer allocate fail");

    for (uint32_t i = 0; i < buf_list->mNumberBuffers; i++) {
        size = buf_list->mBuffers[i].mDataByteSize;
        buf_list->mBuffers[i].mData = malloc(size);
    }
    *bufferList=buf_list;
    return Yang_Ok;
}

static OSStatus yang_notification_callback(AudioObjectID id, UInt32 num_addresses,
              const AudioObjectPropertyAddress addresses[], void *data)
{
    (void)id;
    (void)num_addresses;
    (void)addresses;
    (void)data;
    return noErr;
}

int32_t yang_mac_audio_initHooks(bool isInput,AudioUnit  punit,AudioDeviceID  deviceId,void* user)
{
    OSStatus status=noErr;
    AURenderCallbackStruct callback_info = {.inputProc = yang_input_callback,
                                            .inputProcRefCon = user};
    AudioObjectPropertyAddress addr = {kAudioDevicePropertyDeviceIsAlive,
                                       kAudioObjectPropertyScopeGlobal,
                                       kAudioObjectPropertyElementMaster};

    status=AudioObjectAddPropertyListener(deviceId, &addr,yang_notification_callback, user);
    yang_success(status,"auido init hook set disconnect callback fail");

    addr.mSelector=kAudioStreamPropertyAvailablePhysicalFormats;
    status=AudioObjectAddPropertyListener(deviceId, &addr,yang_notification_callback, user);
    yang_success(status,"auido init hook set format change callback fail");


    if (isInput) {
        AudioObjectPropertyAddress addr = {kAudioHardwarePropertyDefaultInputDevice,
                                           kAudioObjectPropertyScopeGlobal,
                                           kAudioObjectPropertyElementMaster};

        status = AudioObjectAddPropertyListener(kAudioObjectSystemObject,&addr,yang_notification_callback, user);
        yang_success(status,"auido init hook set device change callback fail");

    }

    status = AudioUnitSetProperty(punit, isInput?kAudioOutputUnitProperty_SetInputCallback:kAudioUnitProperty_SetRenderCallback,
                                  kAudioUnitScope_Global, 0, &callback_info, sizeof(callback_info));
    yang_success(status,"auido init hook set input callback fail");


   return Yang_Ok;
}

int32_t yang_mac_audio_uninit(bool isInput,AudioUnit  punit,AudioDeviceID  deviceId,void* user){
    OSStatus status;

    status = AudioUnitUninitialize(punit);

    if(status!=noErr) yang_error("mac audio capture coreaudio_uninit uninitialize failed");
    AURenderCallbackStruct callback_info = {.inputProc = NULL,
                                            .inputProcRefCon = NULL};

    AudioObjectPropertyAddress addr = {kAudioDevicePropertyDeviceIsAlive,
                                       kAudioObjectPropertyScopeGlobal,
                                       kAudioObjectPropertyElementMaster};

    AudioObjectRemovePropertyListener(deviceId, &addr,
                                      yang_notification_callback, user);

    addr.mSelector = kAudioStreamPropertyAvailablePhysicalFormats;
    AudioObjectRemovePropertyListener(deviceId, &addr,
                                      yang_notification_callback, user);

    if (isInput) {
        addr.mSelector = kAudioHardwarePropertyDefaultInputDevice;
        AudioObjectRemovePropertyListener(kAudioObjectSystemObject,
                                          &addr, yang_notification_callback,
                                          user);
    }

    AudioUnitSetProperty(punit, isInput?kAudioOutputUnitProperty_SetInputCallback:kAudioUnitProperty_SetRenderCallback,
                             kAudioUnitScope_Global, 0, &callback_info, sizeof(callback_info));


    status = AudioComponentInstanceDispose(punit);
    yang_success(status, "coreaudio_uninit dispose fail");


    return Yang_Ok;
}
#endif
