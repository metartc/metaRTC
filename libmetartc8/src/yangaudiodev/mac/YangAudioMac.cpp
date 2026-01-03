//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangaudiodev/mac/YangAudioMac.h>
#include <yangutil/sys/YangLog.h>

#include <yangutil/buffer2/YangBuffer2.h>

#include <yangaudio/YangAudioUtil.h>
#include <yangutil/sys/YangMath.h>

#if Yang_OS_MAC
#include <CoreAudio/CoreAudio.h>
#include "YangAudioDeviceMac.h"

typedef struct {
	yangbool isInput;
	yangbool isStart;
	yangbool inited;
	AudioDeviceID deviceId;

	uint32_t sample;
	uint32_t channel;
	uint32_t defaultSample;
	uint32_t defaultChannel;

	uint32_t audioLen;
	AudioDeviceIOProcID deviceIOProcID;

	uint8_t *audioData;
	uint8_t* callbackBuffer;

#if Yang_Enable_MacAudioFloat
	uint8_t* captureBuffer;
#endif

	YangBuffer2 *buffer;
	YangMacAudioCallback *callback;

	YangFrame audioFrame;
	YangAudioResample resample;
} YangAudioMacSession;

static void yang_on_audio(void *psession, uint8_t *data, uint32_t nb) {
#if Yang_Enable_MacAudioFloat
    uint32_t i;
    uint32_t audioCount=nb/4;

    float* fdata;
    int16_t* captureData;
#endif
	YangBuffer2 *buffer;
	YangAudioMacSession *session = (YangAudioMacSession*) psession;

	if (session == NULL || !session->inited)
		return;

	buffer = session->buffer;

#if	Yang_Enable_MacAudioFloat
    fdata=(float*)data;
    captureData=(int16_t*)session->captureBuffer;
	for(i=0;i<audioCount;i++)
		captureData[i]=yang_floattoint16(fdata[i]);

	buffer->putBuffer(buffer->session,session->captureBuffer,audioCount*2);
#else
	buffer->putBuffer(buffer->session,data,nb);
#endif

	while (buffer->size(buffer->session) >= session->audioLen) {
		if (buffer->gutBuffer(buffer->session, session->audioData,
				session->audioLen) == Yang_Ok) {
			session->audioFrame.nb = session->audioLen;
			session->audioFrame.payload = session->audioData;
			session->resample.resample(session->resample.context,
					&session->audioFrame);
			if (session->callback && session->callback->on_audio)
				session->callback->on_audio(session->audioFrame.payload,
						session->audioFrame.nb, session->callback->user);
		} else {
			break;
		}
	}

}

static OSStatus yang_mac_audio_captureCallback(AudioDeviceID deviceId,
		const AudioTimeStamp *stamp, const AudioBufferList *inputData,
		const AudioTimeStamp *inputTime, AudioBufferList *ioData,
		const AudioTimeStamp *ioTime, void *user) {
	uint32_t i;

	YangAudioMacSession *session = (YangAudioMacSession*) user;

	if (session == NULL)
		return 1;

	for (i = 0; i < inputData->mNumberBuffers; i++) {
		yang_on_audio(session, (uint8_t*) inputData->mBuffers[i].mData,
				inputData->mBuffers[i].mDataByteSize);
	}

	return 0;
}

static OSStatus yang_mac_audio_renderCallback(AudioDeviceID deviceId,
		const AudioTimeStamp *stamp, const AudioBufferList *inputData,
		const AudioTimeStamp *inputTime, AudioBufferList *outputData,
		const AudioTimeStamp *ioTime, void *user) {
	uint32_t i,dataCount;
#if  Yang_Enable_MacAudioFloat
	int16_t* data;
	float* renderData;
#endif
	AudioBuffer *buffer;
	YangFrame *audioFrame;
	YangAudioMacSession *session = (YangAudioMacSession*) user;

	if (session == NULL)
		return 1;

	audioFrame = session->callback->getRenderData(session->callback->user);

	if(audioFrame) {
		session->resample.resample(session->resample.context, audioFrame);
		session->buffer->putBuffer(session->buffer->session,
				audioFrame->payload, audioFrame->nb);
	}

	for (i = 0; i < outputData->mNumberBuffers; i++) {
		buffer = (AudioBuffer*)&outputData->mBuffers[i];
#if  Yang_Enable_MacAudioFloat
		data=(int16_t*)session->callbackBuffer;
		renderData=(float*)session->audioData;
		dataCount=buffer->mDataByteSize/4;
#else
		dataCount=buffer->mDataByteSize/2;
#endif
		if (session->isStart &&
			session->buffer->gutBuffer(session->buffer->session,session->callbackBuffer, dataCount*2) == Yang_Ok){

#if	Yang_Enable_MacAudioFloat
			for(uint32_t j=0;j<dataCount;j++)
				renderData[j]=yang_int16tofloat(data[j]);
			yang_memcpy(buffer->mData, session->audioData, buffer->mDataByteSize);
#else
			yang_memcpy(buffer->mData, session->callbackBuffer, buffer->mDataByteSize);
#endif
		} else {
			yang_memset(buffer->mData, 0, buffer->mDataByteSize);
		}
	}

	return 0;
}

static int32_t yang_start(void *psession) {
	OSStatus status;
	YangAudioMacSession *session = (YangAudioMacSession*) psession;
	if (session == NULL)
		return 1;

	if (!session->inited)
		return 1;

	if (session->isStart)
		return Yang_Ok;


	status = AudioDeviceStart(session->deviceId, session->deviceIOProcID);
	yang_success(status, "mac audio capture start failed");

	session->isStart = true;
	return Yang_Ok;
}

static int32_t yang_stop(void *psession) {
	OSStatus status;
	YangAudioMacSession *session = (YangAudioMacSession*) psession;
	if (session == NULL)
		return 1;

	if (!session->isStart)
		return Yang_Ok;

	status = AudioDeviceStop(session->deviceId, session->deviceIOProcID);

	session->isStart = false;
	yang_success(status, "coreaudio_uninit dispose fail");

	return Yang_Ok;
}

static void yang_setInputCallback(void *psession,
		YangMacAudioCallback *callback) {
	YangAudioMacSession *session = (YangAudioMacSession*) psession;
	if (session == NULL)
		return;

	session->callback = callback;
}

static uint32_t yang_getSample(void *psession) {
	YangAudioMacSession *session = (YangAudioMacSession*) psession;
	if (session == NULL)
		return 1;
	return session->defaultSample;
}

static uint32_t yang_getChannel(void *psession) {
	YangAudioMacSession *session = (YangAudioMacSession*) psession;
	if (session == NULL)
		return 1;

	return session->defaultChannel;
}

static int32_t yang_init(void *psession) {
	int32_t err = Yang_Ok;
	YangAudioMacSession *session = (YangAudioMacSession*) psession;
	if (session == NULL)
		return 1;

	if (session->inited)
		return Yang_Ok;

	if (yang_mac_audio_getDefaultDevice(&session->deviceId,
			session->isInput)!=Yang_Ok) {
		return 1;
	}

	//   if(yang_mac_audio_getDefaultDevice(&session->deviceId,session->isInput)!=Yang_Ok){
	//    return 1;
	//  }

	err = yang_mac_audio_init(session->isInput, session->deviceId,
			&session->defaultSample, session->defaultChannel);
	yang_success(err, "mac audio input unit initialize failed");

	AudioDeviceCreateIOProcID(session->deviceId,
			session->isInput ?
					yang_mac_audio_captureCallback :
					yang_mac_audio_renderCallback, session,
			&session->deviceIOProcID);

	session->audioLen = session->defaultSample * session->defaultChannel * 2 / 50;

	if(session->isInput){
		session->resample.init(session->resample.context, session->defaultSample,
			session->defaultChannel, session->sample, session->channel, 20);
	}else{
		session->resample.init(session->resample.context,  session->sample, session->channel,session->defaultSample,
				session->defaultChannel, 20);
	}

	session->inited = true;

	return err;
}

int32_t yang_create_audioMac(YangAudioMac *audio, yangbool isInput,
		uint32_t sample, uint32_t channel) {
	YangAudioMacSession *session;
	if (audio == NULL)
		return 1;

	session = (YangAudioMacSession*) yang_calloc(sizeof(YangAudioMacSession),1);
	audio->session = session;

	session->isStart = false;
	session->inited = false;
	session->isInput = isInput;

	// session->bufferList=NULL;
	session->audioData = NULL;
	session->callback = NULL;

	session->defaultSample = 48000;
	session->defaultChannel = isInput?1:2;

	session->sample = sample;
	session->channel = channel;

	session->audioLen = session->defaultSample * session->defaultChannel * 2
			/ 50;

	yang_memset(&session->audioFrame, 0, sizeof(session->audioFrame));
#if Yang_Enable_MacAudioFloat
	session->captureBuffer=NULL;
	if(isInput)
		session->captureBuffer=(uint8_t*)malloc(48000*4*2);
#endif
	session->callbackBuffer=(uint8_t*)malloc(48000*4*2);
	session->audioData = (uint8_t*) yang_malloc(48000 * 4 * 2);

	session->buffer = (YangBuffer2*) yang_calloc(sizeof(YangBuffer2), 1);
	yang_create_buffer2(session->buffer, 480000*8);
	yang_create_audioresample(&session->resample);

	audio->getChannel = yang_getChannel;
	audio->getSample = yang_getSample;
	audio->init = yang_init;
	audio->on_audio = yang_on_audio;
	audio->setAudioCallback = yang_setInputCallback;
	audio->start = yang_start;
	audio->stop = yang_stop;
	return Yang_Ok;
}

void yang_destroy_audioMac(YangAudioMac *audio) {
	OSStatus status;
	YangAudioMacSession *session;
	if (audio == NULL)
		return;

	session = (YangAudioMacSession*) audio->session;
	yang_stop(session);

	status = AudioDeviceDestroyIOProcID(session->deviceId,
			session->deviceIOProcID);

	if(status!=noErr)
		yang_error("AudioDeviceDestroyIOProcID error(%d)",status);

	yang_mac_audio_uninit(session->isInput, session->deviceId);

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
