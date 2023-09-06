//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangaudiodev/YangAudioCaptureHandle.h>
#include <yangutil/sys/YangLog.h>

#include "YangAudioAndroid.h"

#if Yang_OS_ANDROID
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

typedef struct{
	yangbool isInit;
	int32_t bufferLen;
	void* user;
	uint8_t* buffer;
	SLObjectItf recorderObject;
	//录音器对象
	SLRecordItf recorderRecord;
	//缓冲队列
	SLAndroidSimpleBufferQueueItf recorderBufferQueue;

}YangOpenslRecord;

typedef struct{
	yangbool isInit;
	int      inputDataCount;
	// output mix interfaces
	SLObjectItf outputMixObject;

	// buffer queue player interfaces
	SLObjectItf bqPlayerObject;
	SLPlayItf bqPlayerPlay;
	SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;

	SLObjectItf pitchObject;
	SLPitchItf bqPitchEngne;
	SLPlaybackRateItf bqPlayerRate;
	SLPitchItf bqPlayerVolume;
}YangOpenslPlay;

typedef struct YangOpensl {
	double time;
	uint32_t outchannels; //输出的声道数量
	uint32_t sampleRate; //采样率

	yangbool isInit;
	yang_audioAndroid_record_callback recordCallback;

	// engine interfaces
	SLObjectItf engineObject;
	SLEngineItf engineEngine;
	YangOpenslPlay play;
	YangOpenslRecord record;
} YangOpensl;

void yang_audioAndroid_bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
void g_yang_bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	// for streaming recording, here we would call Enqueue to give recorder the next buffer to fill
	// but instead, this is a one-time buffer so we stop recording
	if(context==NULL) return;
	YangOpensl* sl=(YangOpensl*)context;
	//YangAudioCaptureHandle* ah=(YangAudioCaptureHandle*)sl->record.user;
	SLresult result=(*sl->record.recorderBufferQueue)->Enqueue(sl->record.recorderBufferQueue, sl->record.buffer,sl->record.bufferLen);
	//SLresult result=(*bq)->Enqueue(sl->record.recorderBufferQueue, sl->record.buffer,sl->record.bufferLen);
	if(result!=SL_RESULT_SUCCESS){
		yang_error("audio capture error(%u)",result);
		return;
	}

	if(sl->recordCallback) sl->recordCallback(sl->record.buffer, sl->record.bufferLen,sl->record.user);
	//ah->putBuffer2(sl->record.buffer, sl->record.bufferLen);

}
// creates the OpenSL ES audio engine
SLresult yang_audioAndroid_openSLCreateEngine(YangOpensl *p)
{
	SLresult result=SL_RESULT_SUCCESS;
	if(p->isInit) return result;
	// create engine
	result = slCreateEngine(&(p->engineObject), 0, NULL, 0, NULL, NULL);
	if(result != SL_RESULT_SUCCESS) goto  engine_end;

	// realize the engine
	result = (*p->engineObject)->Realize(p->engineObject, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS) goto engine_end;

	// get the engine interface, which is needed in order to create other objects
	result = (*p->engineObject)->GetInterface(p->engineObject, SL_IID_ENGINE, &(p->engineEngine));
	if(result != SL_RESULT_SUCCESS) goto  engine_end;
	p->isInit=yangtrue;
	return SL_RESULT_SUCCESS;
	engine_end:
	yang_trace("android open audio sl engine error(%u)",result);
	return result;
}

SLuint32 yang_get_Slsample(SLuint32 psample){
	SLuint32 sample=psample;
	switch(psample){
	case 8000:
		sample = SL_SAMPLINGRATE_8;
		break;
	case 11025:
		sample = SL_SAMPLINGRATE_11_025;
		break;
	case 16000:
		sample = SL_SAMPLINGRATE_16;
		break;
	case 22050:
		sample = SL_SAMPLINGRATE_22_05;
		break;
	case 24000:
		sample = SL_SAMPLINGRATE_24;
		break;
	case 32000:
		sample = SL_SAMPLINGRATE_32;
		break;
	case 44100:
		sample = SL_SAMPLINGRATE_44_1;
		break;
	case 48000:
		sample = SL_SAMPLINGRATE_48;
		break;
	case 64000:
		sample = SL_SAMPLINGRATE_64;
		break;
	case 88200:
		sample = SL_SAMPLINGRATE_88_2;
		break;
	case 96000:
		sample = SL_SAMPLINGRATE_96;
		break;
	case 192000:
		sample = SL_SAMPLINGRATE_192;
		break;
	default:
		break;
		return -1;
	}
	return sample;
}

// opens the OpenSL ES device for output
SLresult yang_audioAndroid_openSLPlayOpen(YangOpensl *p)
{
	if(p==NULL) return 1;
	if(p->play.isInit) return SL_RESULT_SUCCESS;
	SLresult result;
	SLuint32 sample = p->sampleRate;
	SLuint32  channels = p->outchannels;

	if(channels) {
		// configure audio source
		SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
		sample=yang_get_Slsample(p->sampleRate);

		const SLInterfaceID ids[] = {SL_IID_VOLUME};
		const SLboolean req[] = {SL_BOOLEAN_FALSE};
		result = (*p->engineEngine)->CreateOutputMix(p->engineEngine, &(p->play.outputMixObject), 1, ids, req);
		if(result != SL_RESULT_SUCCESS) return result;

		// realize the output mix
		result = (*p->play.outputMixObject)->Realize(p->play.outputMixObject, SL_BOOLEAN_FALSE);

		SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, channels, sample,
				SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
				channels==1?SL_SPEAKER_FRONT_CENTER:SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN};

		SLDataSource audioSrc = {&loc_bufq, &format_pcm};

		// configure audio sink
		SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, p->play.outputMixObject};
		SLDataSink audioSnk = {&loc_outmix, NULL};

		// create audio player
		const SLInterfaceID ids1[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE,SL_IID_PLAYBACKRATE,SL_IID_VOLUME};
		const SLboolean req1[] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE};
		result = (*p->engineEngine)->CreateAudioPlayer(p->engineEngine, &(p->play.bqPlayerObject), &audioSrc, &audioSnk,
				3, ids1, req1);
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		// realize the player
		result = (*p->play.bqPlayerObject)->Realize(p->play.bqPlayerObject, SL_BOOLEAN_FALSE);
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		// get the play interface
		result = (*p->play.bqPlayerObject)->GetInterface(p->play.bqPlayerObject, SL_IID_PLAY, &(p->play.bqPlayerPlay));
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		// get the play rate
		result = (*p->play.bqPlayerObject)->GetInterface(p->play.bqPlayerObject, SL_IID_VOLUME, &(p->play.bqPlayerVolume));
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		// get the play volume
		result = (*p->play.bqPlayerObject)->GetInterface(p->play.bqPlayerObject, SL_IID_PLAYBACKRATE, &(p->play.bqPlayerRate));
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		// get the buffer queue interface
		result = (*p->play.bqPlayerObject)->GetInterface(p->play.bqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
				&(p->play.bqPlayerBufferQueue));
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		// register callback on the buffer queue
		result = (*p->play.bqPlayerBufferQueue)->RegisterCallback(p->play.bqPlayerBufferQueue, yang_audioAndroid_bqPlayerCallback, p);
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		// set the player's state to playing
		result = (*p->play.bqPlayerPlay)->SetPlayState(p->play.bqPlayerPlay, SL_PLAYSTATE_PLAYING);
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		p->play.isInit=yangtrue;
		return result;

	}

	return SL_RESULT_SUCCESS;
	cleanup:
	yang_error("android open audio sl play error(%u)",result);
	return result;
}

// opens the OpenSL ES device for output
SLresult yang_audioAndroid_openSLRecordOpen(YangOpensl *p)
{
	if(p==NULL) return 1;
	if(p->record.isInit) return SL_RESULT_SUCCESS;
	SLresult result;
	SLuint32 sample = p->sampleRate;
	SLuint32  channels = p->outchannels;

	if(channels) {
		// configure audio source
		//SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
		sample=yang_get_Slsample(p->sampleRate);
		/**
		 * 设置IO设备（麦克风）
		 */
		SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT,
				SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
		SLDataSource audioSrc = {&loc_dev, NULL};

		/**
		 * 设置buffer队列
		 */
		SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

		SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, channels, sample,
				SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
				channels==1?SL_SPEAKER_FRONT_CENTER:SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN};

		SLDataSink audioSnk = {&loc_bq, &format_pcm};
		const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
		const SLboolean req[1] = {SL_BOOLEAN_TRUE};
		/**
		 * 创建录制器
		 */
		result = (*p->engineEngine)->CreateAudioRecorder(p->engineEngine, &p->record.recorderObject, &audioSrc,
				&audioSnk, 1, id, req);
		if (SL_RESULT_SUCCESS != result) {
			goto cleanup;
		}
		result = (*p->record.recorderObject)->Realize(p->record.recorderObject, SL_BOOLEAN_FALSE);
		if (SL_RESULT_SUCCESS != result) {
			goto cleanup;
		}
		result = (*p->record.recorderObject)->GetInterface(p->record.recorderObject, SL_IID_RECORD, &p->record.recorderRecord);
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		result = (*p->record.recorderObject)->GetInterface(p->record.recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
				&p->record.recorderBufferQueue);
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		result = (*p->record.recorderBufferQueue)->Enqueue(p->record.recorderBufferQueue, p->record.buffer,
				p->record.bufferLen);
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		result = (*p->record.recorderBufferQueue)->RegisterCallback(p->record.recorderBufferQueue, g_yang_bqRecorderCallback, p);
		if(result != SL_RESULT_SUCCESS) goto cleanup;

		/**
		 * 开始录音
		 */
		 result=(*p->record.recorderRecord)->SetRecordState(p->record.recorderRecord, SL_RECORDSTATE_RECORDING);
		 if(result != SL_RESULT_SUCCESS) goto cleanup;
		 p->record.isInit=yangtrue;
		return result;

	}

	return SL_RESULT_SUCCESS;
	cleanup:
	yang_error("android open audio sl capture error(%u)",result);
	return result;
}
// close the OpenSL IO and destroy the audio engine
void yang_audioAndroid_destroy(YangOpensl *p)
{
	if(p==NULL||!p->isInit) return;
	if(p->record.isInit){
		(*p->record.recorderRecord)->SetRecordState(p->record.recorderRecord, SL_RECORDSTATE_STOPPED);

		yang_free(p->record.buffer);
	}

	if(p->play.isInit){
		// destroy buffer queue audio player object, and invalidate all associated interfaces
		if (p->play.bqPlayerObject != NULL) {
			(*p->play.bqPlayerObject)->Destroy(p->play.bqPlayerObject);
			p->play.bqPlayerObject = NULL;
			p->play.bqPlayerPlay = NULL;
			p->play.bqPlayerBufferQueue = NULL;
		}

		// destroy output mix object, and invalidate all associated interfaces
		if (p->play.outputMixObject != NULL) {
			(*p->play.outputMixObject)->Destroy(p->play.outputMixObject);
			p->play.outputMixObject = NULL;
		}
	}
	// destroy engine object, and invalidate all associated interfaces
	if (p->isInit && p->engineObject != NULL) {
		(*p->engineObject)->Destroy(p->engineObject);
		p->engineObject = NULL;
		p->engineEngine = NULL;
	}
}

// open the android audio device for input and/or output
YangOpensl* yang_audioAndroid_open(uint32_t sr, uint32_t inchannels, uint32_t outchannels)
{
	YangOpensl *p;
	//分配内存空间并初始化
	p = (YangOpensl *) yang_calloc(1,sizeof(YangOpensl));
	//采样率
	p->sampleRate = sr;

	//创建引擎对象及接口
	if(yang_audioAndroid_openSLCreateEngine(p) != SL_RESULT_SUCCESS) {
		yang_audioAndroid_destroy(p);
		return NULL;
	}
	p->play.inputDataCount = 0;
	//输出声道数
	p->outchannels = outchannels;
	if(yang_audioAndroid_openSLPlayOpen(p) != SL_RESULT_SUCCESS) {
		yang_audioAndroid_destroy(p);
		return NULL;
	}
	return p;
}



// returns timestamp of the processed stream
double yang_audioAndroid_getTimestamp(void* context)
{
	YangOpensl *p=(YangOpensl*)context;
	return p->time;
}

// this callback handler is called every time a buffer finishes playing
void yang_audioAndroid_bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	YangOpensl *p = (YangOpensl *) context;
	p->play.inputDataCount --;
	// free(p->inputBuffer);
}
int32_t yang_audioAndroid_getInputdataCount(void* context){
	YangOpensl *p=(YangOpensl*)context;
	return p->play.inputDataCount;
}
// puts a buffer of size samples to the device
uint32_t yang_audioAndroid_audioOut(void* context, uint16_t *buffer,uint32_t size)
{
	YangOpensl *p=(YangOpensl*)context;
	SLresult result=SL_RESULT_SUCCESS;
	if((result=(*p->play.bqPlayerBufferQueue)->Enqueue(p->play.bqPlayerBufferQueue,buffer, size))!=SL_RESULT_SUCCESS){
		yang_error("android play error(%u)",result);
	}
	p->play.inputDataCount ++;
	return result;
}

int yang_audioAndroid_setPlayRate(void* context,int rateChange){
	YangOpensl *p=(YangOpensl*)context;
	SLmillibel value;
	SLresult result;
	if (!p) return -1;
	if (!p->play.bqPlayerRate) return -1;
	result = (*p->play.bqPlayerRate)->GetRate(p->play.bqPlayerRate,&value);
	if (result != SL_RESULT_SUCCESS) return -1;
	if (rateChange<0){
		value -= 100;
	} else
		value += 100;
	if (value < 500) value = 500;
	if (value > 2000) value = 2000;
	result = (*p->play.bqPlayerRate)->SetRate(p->play.bqPlayerRate,value);
	if (result == SL_RESULT_SUCCESS){
		return Yang_Ok;
	} else
		return ERROR_SYS_AudioRender;
}

int32_t yang_create_audioAndroid_play(YangAudioAndroid* aa,int32_t sample,int32_t channels){
	if(aa->context==NULL) aa->context=yang_audioAndroid_open(sample,channels,channels);
	YangOpensl* opensl=(YangOpensl*)aa->context;

	//opensl->record.isInit=yangfalse;
	opensl->play.isInit=yangfalse;
	if(yang_audioAndroid_openSLCreateEngine(opensl)!= SL_RESULT_SUCCESS){
		return yang_error_wrap(ERROR_SYS_AudioRender,"openSLCreateEngine fail!");
	}
	if(yang_audioAndroid_openSLPlayOpen(opensl)!= SL_RESULT_SUCCESS){
		return yang_error_wrap(ERROR_SYS_AudioRender,"openSLPlayOpen fail!");
	}
	aa->setPlayRate=yang_audioAndroid_setPlayRate;
	aa->audioOut=yang_audioAndroid_audioOut;
	aa->getTimestamp=yang_audioAndroid_getTimestamp;
	aa->getInputdataCount=yang_audioAndroid_getInputdataCount;
	return Yang_Ok;

}
int32_t yang_create_audioAndroid_record(YangAudioAndroid* aa,yang_audioAndroid_record_callback callback,void* user,int32_t sample,int32_t channels){
	if(aa->context==NULL) aa->context=yang_audioAndroid_open(sample,channels,channels);
	YangOpensl* opensl=(YangOpensl*)aa->context;

	opensl->record.isInit=yangfalse;
	//opensl->play.isInit=yangfalse;
	if(yang_audioAndroid_openSLCreateEngine(opensl)!= SL_RESULT_SUCCESS){
		return yang_error_wrap(ERROR_SYS_AudioCapture,"openSLCreateEngine fail!");
	}
	opensl->record.bufferLen=sample*channels*2/50;
	if(opensl->record.buffer==NULL) opensl->record.buffer=(uint8_t*)yang_calloc(opensl->record.bufferLen,1);
	opensl->record.user=user;

	opensl->recordCallback=callback;
	if(yang_audioAndroid_openSLRecordOpen(opensl)!= SL_RESULT_SUCCESS){
		return yang_error_wrap(ERROR_SYS_AudioCapture,"openSLPlayOpen fail!");
	}

	return Yang_Ok;
}

void yang_destroy_audioAndroid(YangAudioAndroid* aa){
	if(aa==NULL||aa->context==NULL) return;
	YangOpensl* opensl=(YangOpensl*)aa->context;
	yang_audioAndroid_destroy(opensl);
	yang_free(aa->context);
}
#endif
