//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "YangAudioMac.h"
#include <yangutil/sys/YangLog.h>
#if Yang_OS_APPLE
#include "YangAudioDeviceMac.h"

static OSStatus yang_mac_audio_callback(AudioDeviceID,
                                      const AudioTimeStamp*,
                                      const AudioBufferList* inputData,
                                      const AudioTimeStamp* inputTime,
                                      AudioBufferList* ioData,
                                      const AudioTimeStamp* ioTime,
                                      void* user) {
  YangAudioMac* macAudio = (YangAudioMac*)user;
  if(macAudio==NULL) return 1;

  if(macAudio->m_isInput){

       for (uint32_t i = 0; i < inputData->mNumberBuffers; i++){
           macAudio->on_audio((uint8_t*)inputData->mBuffers[i].mData,inputData->mBuffers[i].mDataByteSize);
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

              }
          }
      }
  }


  return 0;
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

}


int32_t YangAudioMac::start(){
    OSStatus status;
    if(!m_inited) return 1;
    if(m_isStart) return Yang_Ok;
    AudioDeviceCreateIOProcID(m_deviceId, yang_mac_audio_callback, this, &_deviceIOProcID);
    status=AudioDeviceStart(m_deviceId, _deviceIOProcID);
   yang_success(status,"mac audio capture start failed");

    m_isStart=true;
    return Yang_Ok;
}

int32_t YangAudioMac::stop(){
    OSStatus status;
    if(!m_isStart) return Yang_Ok;
    status=AudioDeviceStop(m_deviceId, _deviceIOProcID);
    status= AudioDeviceDestroyIOProcID(m_deviceId, _deviceIOProcID);
    yang_mac_audio_uninit(m_isInput,m_deviceId);
    // yang_success(status, "coreaudio_uninit dispose fail");
    m_isStart=false;
    return Yang_Ok;
}

void YangAudioMac::setInputCallback(YangMacAudioCallback* callback){
    m_callback=callback;
}
void YangAudioMac::on_audio(uint8_t* data,uint32_t nb){
    if(!m_inited) return;
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
    int32_t err=Yang_Ok;
    if(m_inited) return Yang_Ok;
    if(yang_mac_audio_getDefaultDevice(&m_deviceId,m_isInput)!=Yang_Ok){
        return 1;
   }


    if(yang_mac_audio_getDefaultDevice(&m_deviceId,m_isInput)!=Yang_Ok){
        return 1;
   }

     err=yang_mac_audio_init(m_isInput,m_deviceId,&m_sample,m_channel);
     yang_success(err,"mac audio input unit initialize failed");

    m_inited=true;

    return err;
}

#endif
