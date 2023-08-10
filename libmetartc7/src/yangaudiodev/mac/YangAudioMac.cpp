//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "YangAudioMac.h"
#include <yangutil/sys/YangLog.h>
#if defined(__APPLE__)
#include <CoreFoundation/CFString.h>

#include "YangAudioDeviceMac.h"

OSStatus yang_input_callback(void *data,
                   AudioUnitRenderActionFlags *action_flags,
                   const AudioTimeStamp *ts_data, UInt32 bus_num,
                   UInt32 frames, AudioBufferList *ioData)
{
    YangAudioMac *macAudio = (YangAudioMac*)data;

    OSStatus status=noErr;
    if(macAudio->m_isInput){
        status = AudioUnitRender(macAudio->m_unit, action_flags, ts_data, bus_num, frames,
                       macAudio->m_bufferList);
        if(status!=noErr){
            yang_error("auido input_callback audio retrieval fail(%d)",status);
            return status;
        }


        for (uint32_t i = 0; i < macAudio->m_bufferList->mNumberBuffers; i++){

            macAudio->on_audio((uint8_t*)macAudio->m_bufferList->mBuffers[i].mData,macAudio->m_bufferList->mBuffers[i].mDataByteSize);
        }
    }else{
        for (uint32_t i = 0; i < ioData -> mNumberBuffers; i++) {
                    AudioBuffer *buffer = &ioData -> mBuffers[i];
                     YangFrame* frame=&macAudio->m_audioFrame;
                     if(macAudio->m_callback&&macAudio->m_callback->getRenderData){
                         if(macAudio->m_callback->getRenderData(frame,macAudio->m_callback->user)==Yang_Ok){
                             uint32_t nb = (uint32_t)yang_min(buffer->mDataByteSize, (uint32_t)frame->nb);
                             memcpy(buffer->mData, frame->payload, nb);
                             buffer->mDataByteSize = nb;
                         }else{
                             buffer->mDataByteSize = 0;
                             *action_flags |= kAudioUnitRenderAction_OutputIsSilence;
                         }
                     }

            }

    }

    return noErr;
}

YangAudioMac::YangAudioMac(bool isInput)
{
    m_isInput=isInput;
    m_bufferList=NULL;
    m_isStart=false;
    m_inited=false;
    m_callback=NULL;
    m_sample=48000;
    m_channel=2;
    memset(&m_audioFrame,0,sizeof(m_audioFrame));
}

YangAudioMac::~YangAudioMac(){

    stop();
    if(m_inited){
      yang_mac_audio_uninit(m_isInput,m_unit,m_deviceId,this);
    }
    if (m_bufferList) {
        for (uint32_t i = 0; i < m_bufferList->mNumberBuffers; i++)
            yang_free(m_bufferList->mBuffers[i].mData);

        yang_free(m_bufferList);
    }
}


int32_t YangAudioMac::start(){
    OSStatus status;
    if(!m_inited) return 1;
    if(m_isStart) return Yang_Ok;
   // kAudioUnitErr_Uninitialized
    status = AudioOutputUnitStart(m_unit);
    yang_success(status,"mac audio capture start failed");

    m_isStart=true;
    return Yang_Ok;
}

int32_t YangAudioMac::stop(){
    OSStatus status;
    if(!m_isStart) return Yang_Ok;
    status = AudioOutputUnitStop(m_unit);
    yang_success(status,"mac audio capture stop failed");
    m_isStart=false;
    return Yang_Ok;
}

void YangAudioMac::setInputCallback(YangMacAudioCallback* callback){
    m_callback=callback;
}
void YangAudioMac::on_audio(uint8_t* data,uint32_t nb){
    if(m_callback&&m_callback->on_audio)
        m_callback->on_audio(data,nb,m_callback->user);
}
uint32_t YangAudioMac::getSample(){
    return m_sample;
}
uint32_t YangAudioMac::getChannel(){
    return m_channel;
}

int32_t YangAudioMac::init(){
    if(m_inited) return Yang_Ok;
    if(yang_mac_audio_getDefaultDevice(&m_deviceId,m_isInput)!=Yang_Ok){
        return 1;
   }

   AudioComponentDescription desc = {
        .componentType = kAudioUnitType_Output,
        .componentSubType = kAudioUnitSubType_HALOutput};

   AudioComponent component = AudioComponentFindNext(NULL, &desc);
    if (!component) {
        yang_error( "coreaudio unit find component failed");
        return 1;
    }

    OSStatus status = AudioComponentInstanceNew(component, &m_unit);
    yang_success(status,"instance audio capture unit failed");

    uint32_t enable_int = 1;
    status=AudioUnitSetProperty(m_unit, kAudioOutputUnitProperty_EnableIO,
                kAudioUnitScope_Input, 1, &enable_int, sizeof(enable_int));
    yang_success(status,"unit set input enable failed");

    enable_int = m_isInput?0:1;
    status= AudioUnitSetProperty(m_unit, kAudioOutputUnitProperty_EnableIO,
                     kAudioUnitScope_Output, 0, &enable_int, sizeof(enable_int));
    yang_success(status,"unit set output enable failed");

    status = AudioUnitSetProperty(m_unit, kAudioOutputUnitProperty_CurrentDevice,
                kAudioUnitScope_Global, 0, &m_deviceId,sizeof(m_deviceId));
    yang_success(status,"set input property deviceId failed");

    if(yang_mac_initAudioFormat(m_isInput,m_unit,&m_sample,m_channel)!=Yang_Ok){
        yang_error( "yang_mac_initAudioFormat failed");
        return 1;
    }

    if(yang_mac_audio_initBuffer(m_unit,m_deviceId,&m_bufferList)!=Yang_Ok){
        yang_error( "yang_mac_audio_initBuffer failed");
        return 1;
    }

    if(yang_mac_audio_initHooks(m_isInput,m_unit,m_deviceId,this)!=Yang_Ok){
        yang_error( "yang_mac_audio_initHooks failed");
        return 1;
    }
    status = AudioUnitInitialize(m_unit);
     yang_success(status,"mac audio input unit initialize failed");

    m_inited=true;

    return Yang_Ok;
}

#endif
