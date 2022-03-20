//
// Copyright (c) 2019-2022 yanggaofeng
//


#include "YangAudioAndroid.h"
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
typedef struct YangOpensl {

    // engine interfaces
    SLObjectItf engineObject;
    SLEngineItf engineEngine;

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


    // buffer indexes
    int      inputDataCount;
    double time;
    uint32_t outchannels; //输出的声道数量
    uint32_t sampleRate; //采样率

} YangOpensl;

void yang_audioAndroid_bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

// creates the OpenSL ES audio engine
 SLresult yang_audioAndroid_openSLCreateEngine(YangOpensl *p)
{
    SLresult result;
    // create engine
    result = slCreateEngine(&(p->engineObject), 0, NULL, 0, NULL, NULL);
    if(result != SL_RESULT_SUCCESS) goto  engine_end;

    // realize the engine
    result = (*p->engineObject)->Realize(p->engineObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS) goto engine_end;

    // get the engine interface, which is needed in order to create other objects
    result = (*p->engineObject)->GetInterface(p->engineObject, SL_IID_ENGINE, &(p->engineEngine));
    if(result != SL_RESULT_SUCCESS) goto  engine_end;

    engine_end:
    return result;
}

// opens the OpenSL ES device for output
SLresult yang_audioAndroid_openSLPlayOpen(YangOpensl *p)
{
    SLresult result;
    SLuint32 sample = p->sampleRate;
    SLuint32  channels = p->outchannels;

    if(channels) {
        // configure audio source
        SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

        switch(sample){
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

        const SLInterfaceID ids[] = {SL_IID_VOLUME};
        const SLboolean req[] = {SL_BOOLEAN_FALSE};
        result = (*p->engineEngine)->CreateOutputMix(p->engineEngine, &(p->outputMixObject), 1, ids, req);
        if(result != SL_RESULT_SUCCESS) return result;

        // realize the output mix
        result = (*p->outputMixObject)->Realize(p->outputMixObject, SL_BOOLEAN_FALSE);

        int speakers;
        if(channels > 1)
            speakers = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        else speakers = SL_SPEAKER_FRONT_CENTER;

        SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, channels, sample,
                                       SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                       (SLuint32)speakers, SL_BYTEORDER_LITTLEENDIAN};

        SLDataSource audioSrc = {&loc_bufq, &format_pcm};

        // configure audio sink
        SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, p->outputMixObject};
        SLDataSink audioSnk = {&loc_outmix, NULL};

        // create audio player
        const SLInterfaceID ids1[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE,SL_IID_PLAYBACKRATE,SL_IID_VOLUME};
        const SLboolean req1[] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE};
        result = (*p->engineEngine)->CreateAudioPlayer(p->engineEngine, &(p->bqPlayerObject), &audioSrc, &audioSnk,
                                                       3, ids1, req1);
        if(result != SL_RESULT_SUCCESS) return result;

        // realize the player
        result = (*p->bqPlayerObject)->Realize(p->bqPlayerObject, SL_BOOLEAN_FALSE);
        if(result != SL_RESULT_SUCCESS) return result;

        // get the play interface
        result = (*p->bqPlayerObject)->GetInterface(p->bqPlayerObject, SL_IID_PLAY, &(p->bqPlayerPlay));
        if(result != SL_RESULT_SUCCESS) return result;

        // get the play rate
        result = (*p->bqPlayerObject)->GetInterface(p->bqPlayerObject, SL_IID_VOLUME, &(p->bqPlayerVolume));
        if(result != SL_RESULT_SUCCESS) return result;

        // get the play volume
        result = (*p->bqPlayerObject)->GetInterface(p->bqPlayerObject, SL_IID_PLAYBACKRATE, &(p->bqPlayerRate));
        if(result != SL_RESULT_SUCCESS) return result;

        // get the buffer queue interface
        result = (*p->bqPlayerObject)->GetInterface(p->bqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                    &(p->bqPlayerBufferQueue));
        if(result != SL_RESULT_SUCCESS) return result;

        // register callback on the buffer queue
        result = (*p->bqPlayerBufferQueue)->RegisterCallback(p->bqPlayerBufferQueue, yang_audioAndroid_bqPlayerCallback, p);
        if(result != SL_RESULT_SUCCESS) return result;

        // set the player's state to playing
        result = (*p->bqPlayerPlay)->SetPlayState(p->bqPlayerPlay, SL_PLAYSTATE_PLAYING);
        return result;

    }

    return SL_RESULT_SUCCESS;
}

// close the OpenSL IO and destroy the audio engine
void yang_audioAndroid_destroy(YangOpensl *p)
{
    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (p->bqPlayerObject != NULL) {
        (*p->bqPlayerObject)->Destroy(p->bqPlayerObject);
        p->bqPlayerObject = NULL;
        p->bqPlayerPlay = NULL;
        p->bqPlayerBufferQueue = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (p->outputMixObject != NULL) {
        (*p->outputMixObject)->Destroy(p->outputMixObject);
        p->outputMixObject = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (p->engineObject != NULL) {
        (*p->engineObject)->Destroy(p->engineObject);
        p->engineObject = NULL;
        p->engineEngine = NULL;
    }
}

// open the android audio device for input and/or output
YangOpensl* yang_audioAndroid_open(uint32_t sr, uint32_t inchannels, uint32_t outchannels, uint32_t bufferframes)
{
    YangOpensl *p;
    //分配内存空间并初始化
    p = (YangOpensl *) calloc(1,sizeof(YangOpensl));
    //采样率
    p->sampleRate = sr;

    //创建引擎对象及接口
    if(yang_audioAndroid_openSLCreateEngine(p) != SL_RESULT_SUCCESS) {
    	yang_audioAndroid_destroy(p);
        return NULL;
    }
    p->inputDataCount = 0;
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
    p->inputDataCount --;
    // free(p->inputBuffer);
}
int32_t yang_audioAndroid_getInputdataCount(void* context){
	YangOpensl *p=(YangOpensl*)context;
	return p->inputDataCount;
}
// puts a buffer of size samples to the device
uint32_t yang_audioAndroid_audioOut(void* context, uint16_t *buffer,uint32_t size)
{
    YangOpensl *p=(YangOpensl*)context;
    (*p->bqPlayerBufferQueue)->Enqueue(p->bqPlayerBufferQueue,buffer, size);
    p->inputDataCount ++;
    return 0;
}

int yang_audioAndroid_setPlayRate(void* context,int rateChange){
    YangOpensl *p=(YangOpensl*)context;
    SLmillibel value;
    SLresult result;
    if (!p) return -1;
    if (!p->bqPlayerRate) return -1;
    result = (*p->bqPlayerRate)->GetRate(p->bqPlayerRate,&value);
    if (result != SL_RESULT_SUCCESS)return -1;
    if (rateChange<0){
        value -= 100;
    } else
        value += 100;
    if (value < 500) value = 500;
    if (value > 2000) value = 2000;
    result = (*p->bqPlayerRate)->SetRate(p->bqPlayerRate,value);
    if (result == SL_RESULT_SUCCESS){
        return 0;
    } else
        return -1;
}
int32_t yang_create_audioAndroid(YangAudioAndroid* aa,int32_t sample,int32_t channels,int32_t frames){
    aa->context=yang_audioAndroid_open(sample,channels,channels,frames);
    YangOpensl* opensl=(YangOpensl*)aa->context;
    if(yang_audioAndroid_openSLCreateEngine(opensl)!= SL_RESULT_SUCCESS){
        return yang_error_wrap(1,"openSLCreateEngine fail!");
    }
    if(yang_audioAndroid_openSLPlayOpen(opensl)!= SL_RESULT_SUCCESS){
        return yang_error_wrap(1,"openSLPlayOpen fail!");
    }
    aa->setPlayRate=yang_audioAndroid_setPlayRate;
    aa->audioOut=yang_audioAndroid_audioOut;
    aa->getTimestamp=yang_audioAndroid_getTimestamp;
    aa->getInputdataCount=yang_audioAndroid_getInputdataCount;
    return Yang_Ok;

}
void yang_destroy_audioAndroid(YangAudioAndroid* aa){
    if(aa==NULL||aa->context==NULL) return;
    YangOpensl* opensl=(YangOpensl*)aa->context;
    yang_audioAndroid_destroy(opensl);
    yang_free(aa->context);
}
