//
// Copyright (c) 2019-2025 yanggaofeng
//
#include <yangaudiodev/mac/YangAudioMac.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/buffer2/YangBuffer2.h>

#include <yangavutil/audio/YangAudioUtil.h>
#include <yangutil/sys/YangMath.h>
#if Yang_OS_IOS

#include "YangAudioDeviceIos.h"
#include <AudioUnit/AudioUnit.h>

typedef struct{
	yangbool isStart;
	yangbool inited;

	uint32_t sample;
	uint32_t channel;
	uint32_t defaultSample;
	uint32_t defaultChannel;
	uint32_t audioLen;
#if Yang_Enable_MacAudioFloat
	uint8_t* captureBuffer;
#endif

	uint8_t* audioData;
	uint8_t* callbackBuffer;

	YangMacAudioCallback *callback;

	YangBuffer2 *buffer;

	YangIosAudio audio;
	YangFrame audioFrame;
	YangAudioResample resample;
}YangAudioMacSession;

static void yang_on_audio(void *psession,uint8_t *data,uint32_t nb){
#if Yang_Enable_MacAudioFloat
    uint32_t i;
    uint32_t audioCount=nb/4;

    float* fdata;
    int16_t* captureData;
#endif

	YangBuffer2* buffer;
	YangAudioMacSession *session=(YangAudioMacSession*)psession;

	if(session==NULL || !session->inited)
		return;

	buffer=session->buffer;

#if	Yang_Enable_MacAudioFloat
    fdata=(float*)data;
    captureData=(int16_t*)session->captureBuffer;
	for(i=0;i<audioCount;i++)
		captureData[i]=yang_floattoint16(fdata[i]);

	buffer->putBuffer(buffer->session,session->captureBuffer,audioCount*2);
#else
	buffer->putBuffer(buffer->session,data,nb);
#endif

	while(buffer->size(buffer->session)>=session->audioLen){
		if(buffer->gutBuffer(buffer->session,session->audioData,session->audioLen)==Yang_Ok){
			session->audioFrame.nb=session->audioLen;
			session->audioFrame.payload=session->audioData;
			session->resample.resample(session->resample.context,&session->audioFrame);
			if(session->callback&&session->callback->on_audio)
				session->callback->on_audio(session->audioFrame.payload,session->audioFrame.nb,session->callback->user);
		}else{
			break;
		}
	}
}

static OSStatus yang_ios_capture_callback(void *user,AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp,
		UInt32 inBusNumber, UInt32 inNumberFrames,AudioBufferList *inputData) {
	OSStatus status;
    uint32_t i;
	YangAudioMacSession *session = (YangAudioMacSession*)user;

	AudioBufferList bufferList;

	if(session==NULL)
		return 1;

	bufferList.mNumberBuffers = 1;
	bufferList.mBuffers[0].mNumberChannels = session->channel;

	bufferList.mBuffers[0].mData = session->callbackBuffer;
#if Yang_Enable_MacAudioFloat
    bufferList.mBuffers[0].mDataByteSize = inNumberFrames*session->channel * sizeof(float);
#else
    bufferList.mBuffers[0].mDataByteSize = inNumberFrames*session->channel * sizeof(int16_t);
#endif

	// 从音频单元获取音频数据
	status = AudioUnitRender(session->audio.audioUnit,
			ioActionFlags,inTimeStamp,
			1,inNumberFrames, &bufferList);

	if(status!=noErr)
		return status;

	for (i = 0; i < bufferList.mNumberBuffers; i++){
		yang_on_audio(session,(uint8_t*)bufferList.mBuffers[i].mData,
				bufferList.mBuffers[i].mDataByteSize);
	}

	return 0;
}

static OSStatus yang_ios_render_callback(void *user,AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp,
		UInt32 inBusNumber, UInt32 inNumberFrames,AudioBufferList *inputData) {
	uint32_t i,dataCount;
#if  Yang_Enable_MacAudioFloat
	int16_t* data;
	float* renderData;
#endif
	AudioBuffer *buffer;
	YangFrame* audioFrame;
	YangAudioMacSession *session = (YangAudioMacSession*)user;

	if(session==NULL)
		return 1;
#if  Yang_Enable_MacAudioFloat
	data=(int16_t*)session->callbackBuffer;
	renderData=(float*)session->audioData;
#endif

	audioFrame = session->callback->getRenderData(session->callback->user);
	dataCount=inNumberFrames*session->channel;

	if(audioFrame) {
		session->resample.resample(session->resample.context, audioFrame);
		session->buffer->putBuffer(session->buffer->session,
				audioFrame->payload, audioFrame->nb);
	}

	for (i = 0; i < inputData -> mNumberBuffers; i++) {
		buffer = &inputData -> mBuffers[i];

		if(session->isStart && session->buffer->gutBuffer(session->buffer->session,session->callbackBuffer,dataCount*2)==Yang_Ok){
#if	Yang_Enable_MacAudioFloat
			for(uint32_t j=0;j<dataCount;j++)
				renderData[j]=yang_int16tofloat(data[j]);
			yang_memcpy(buffer->mData, session->audioData, buffer->mDataByteSize);
#else
			yang_memcpy(buffer->mData, session->callbackBuffer, buffer->mDataByteSize);
#endif
		}else{
			yang_memset(buffer->mData, 0, buffer->mDataByteSize);
		}
	}

	return 0;
}

static int32_t yang_start(void *psession){
	OSStatus status;
	YangAudioMacSession *session=(YangAudioMacSession*)psession;
	if(session==NULL)
		return 1;

	if(!session->inited)
		return 1;

	if(session->isStart)
		return Yang_Ok;

	status=AudioOutputUnitStart(session->audio.audioUnit);
	yang_success(status,"mac audio capture start failed");

	session->isStart=yangtrue;
	return Yang_Ok;
}

static int32_t yang_stop(void *psession){
	OSStatus status;
	YangAudioMacSession *session=(YangAudioMacSession*)psession;
	if(session==NULL)
		return 1;

	if(!session->isStart)
		return Yang_Ok;

	status=AudioOutputUnitStop(session->audio.audioUnit);

	if(status!=noErr){
		yang_error("audio unit stop error!");
	}


	// yang_success(status, "coreaudio_uninit dispose fail");
	session->isStart=yangfalse;
	return Yang_Ok;
}

static void yang_setInputCallback(void *psession,YangMacAudioCallback *callback){
	YangAudioMacSession *session=(YangAudioMacSession*)psession;
	if(session==NULL)
		return;

	session->callback=callback;
}

static uint32_t yang_getSample(void *psession){
	YangAudioMacSession *session=(YangAudioMacSession*)psession;
	if(session==NULL)
		return 1;
	return session->defaultSample;
}

static uint32_t yang_getChannel(void *psession){
	YangAudioMacSession *session=(YangAudioMacSession*)psession;
	if(session==NULL)
		return 1;

	return session->defaultChannel;
}

static int32_t yang_init(void *psession){
	int32_t err=Yang_Ok;
	YangAudioMacSession *session=(YangAudioMacSession*)psession;
	if(session==NULL)
		return 1;

	if(session->inited)
		return Yang_Ok;

	if(yang_ios_init_AudioSession(&session->defaultSample,session->audio.isInput?0:1)!=Yang_Ok){
		return 1;
	}

	session->audioLen=session->defaultSample*session->defaultChannel*2/50;

	if(session->audio.isInput){
		session->resample.init(session->resample.context, session->defaultSample,
			session->defaultChannel, session->sample, session->channel, 20);
	}else{
		session->resample.init(session->resample.context,  session->sample, session->channel,session->defaultSample,
				session->defaultChannel, 20);
	}

	err=yang_ios_audio_init(&session->audio,session->defaultSample,session->defaultChannel);
	//yang_success(err,"ios audio input unit initialize failed");
	if(err!=Yang_Ok){
		yang_error("ios audio init fail!(%d)",err);
		return err;
	}

	session->inited=yangtrue;

	return err;
}

int32_t yang_create_audioMac(YangAudioMac *audio,yangbool isInput,uint32_t sample,uint32_t channel){
	YangAudioMacSession *session;
	if(audio==NULL)
		return 1;

	session=(YangAudioMacSession*)yang_calloc(sizeof(YangAudioMacSession),1);
	audio->session=session;

	session->isStart=yangfalse;
	session->inited=yangfalse;
	session->audio.isInput=isInput;
	session->audio.callback.inputProcRefCon=session;
	session->audio.callback.inputProc=isInput?yang_ios_capture_callback:yang_ios_render_callback;

	session->callback=NULL;

	session->defaultSample=48000;
	session->defaultChannel=isInput?1:2;
	session->sample=sample;
	session->channel=channel;

	session->audioLen=session->defaultSample*session->defaultChannel*2/50;

	yang_memset(&session->audioFrame,0,sizeof(session->audioFrame));
#if Yang_Enable_MacAudioFloat
	session->captureBuffer=NULL;
	if(isInput)
		session->captureBuffer=(uint8_t*)malloc(48000*4*2);
#endif
	session->callbackBuffer=(uint8_t*)malloc(48000*4*2);
	session->audioData=(uint8_t*)yang_malloc(48000*4*2);
	session->buffer=(YangBuffer2*)yang_calloc(sizeof(YangBuffer2),1);
	yang_create_buffer2(session->buffer,480000*8);
	yang_create_audioresample(&session->resample);

	audio->getChannel=yang_getChannel;
	audio->getSample=yang_getSample;
	audio->init=yang_init;
	audio->on_audio=yang_on_audio;
	audio->setAudioCallback=yang_setInputCallback;
	audio->start=yang_start;
	audio->stop=yang_stop;
	return Yang_Ok;
}

void yang_destroy_audioMac(YangAudioMac *audio){
	YangAudioMacSession *session;
	if(audio==NULL)
		return;

	session=(YangAudioMacSession*)audio->session;
	yang_stop(session);
	if(session->inited)
		yang_ios_audio_uninit(&session->audio);

	yang_destroy_audioresample(&session->resample);

	yang_destroy_buffer2(session->buffer);
	yang_free(session->buffer);

	yang_free(session->audioData);
	yang_free(session->callbackBuffer);

#if Yang_Enable_MacAudioFloat
	yang_free(session->captureBuffer);
#endif

	yang_free(audio->session);
}

#endif
