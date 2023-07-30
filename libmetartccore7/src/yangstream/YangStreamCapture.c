//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangstream/YangStreamCapture.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/yangtype.h>
typedef struct YangSpsppsConf{
	uint8_t* sps;
	uint8_t* pps;
	int32_t spsLen;
	int32_t ppsLen;
}YangSpsppsConf;
typedef struct{
	YangSpsppsConf *spsppsConf;
	int32_t frametype;
	int32_t videoLen;
	uint8_t* src;

	int64_t videoTimestamp , baseTimestamp;
	int64_t curVideotimestamp ;
	int32_t preTimestamp ;

	int64_t metaTime;
}YangVideoStreamCapture;

typedef struct{
	uint8_t* audioBuffer;
	uint8_t* src;
	int32_t srcLen;
	int32_t audioLen;

	int64_t atime ;
	int64_t unitAudioTime;
	int32_t frametype;
	YangAudioCodec audioType;
}YangAudioStreamCapture;

typedef struct{
	YangFrame audioFrame;
	YangFrame videoFrame;
	YangAudioStreamCapture audio;
	YangVideoStreamCapture video;

}YangStreamCaptureContext;



void yang_streamcapture_initVideo(void* pcontext,int32_t transtype){

}

void yang_streamcapture_initEncType(YangVideoStreamCapture* video,YangVideoCodec videoType,
		int32_t frametype) {
}

void yang_streamcapture_setVideoData(void* pcontext,YangFrame* videoFrame,YangVideoCodec videoType){
	if(pcontext==NULL) return;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;

	context->video.src=videoFrame->payload;
	context->video.videoLen = videoFrame->nb;
	context->video.videoTimestamp=videoFrame->pts;
	if (context->video.preTimestamp == 0) {
		context->video.baseTimestamp = context->video.videoTimestamp;
		context->video.curVideotimestamp = 0;

	} else {
		if (context->video.videoTimestamp <= context->video.preTimestamp)
			return;

		context->video.curVideotimestamp = context->video.videoTimestamp - context->video.baseTimestamp;
	}
	context->video.preTimestamp = context->video.videoTimestamp;
	context->video.frametype=videoFrame->frametype;
}
void yang_streamcapture_initSpspps(YangVideoStreamCapture* video,uint8_t *buf){

	if(video->spsppsConf==NULL){
		video->spsppsConf=yang_calloc(sizeof(YangSpsppsConf),1);//new YangSpsppsConf();
		int32_t spsLen = *(buf + 12) + 1;
		uint8_t *sps = buf + 13;
		int32_t ppsLen = *(sps + spsLen + 1) + 1;
		uint8_t *pps = buf + 13 + spsLen + 2;
		video->spsppsConf->sps=yang_malloc(spsLen);//new uint8_t[spsLen];
		video->spsppsConf->pps=yang_malloc(ppsLen);//new uint8_t[ppsLen];
		video->spsppsConf->spsLen=spsLen;
		video->spsppsConf->ppsLen=ppsLen;
		yang_memcpy(video->spsppsConf->sps,sps,spsLen);
		yang_memcpy(video->spsppsConf->pps,pps,ppsLen);

	}

}
void yang_streamcapture_setVideoMeta(void* pcontext,uint8_t* p,int32_t plen,YangVideoCodec videoType){
	if(pcontext==NULL) return;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;

		context->video.src=p;
		context->video.videoLen= plen;
		yang_streamcapture_initSpspps(&context->video,p);

}
void yang_streamcapture_setVideoFrametype(void* pcontext,int32_t frametype){
	if(pcontext==NULL) return;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	context->video.frametype=frametype;
}
void yang_streamcapture_setMetaTimestamp(void* pcontext,int64_t timestamp){
	if(pcontext==NULL) return;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;

	if (context->video.preTimestamp == 0) {
		context->video.baseTimestamp = timestamp;
		context->video.metaTime = 0;

	} else {
		context->video.metaTime=timestamp-context->video.baseTimestamp;
	}
}

uint8_t* yang_streamcapture_getVideoData(void* pcontext){
	if(pcontext==NULL) return NULL;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	return context->video.src;
}
int32_t yang_streamcapture_getVideoLen(void* pcontext){
	if(pcontext==NULL) return 0;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	return context->video.videoLen;
}
int32_t yang_streamcapture_getVideoFrametype(void* pcontext){
	if(pcontext==NULL) return 0;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	return context->video.frametype;
}
int64_t yang_streamcapture_getVideoTimestamp(void* pcontext){
	if(pcontext==NULL) return 0;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	if(context->video.frametype==YANG_Frametype_Spspps) return context->video.metaTime * 9 / 100;

	return context->video.curVideotimestamp * 9 / 100;
}

void yang_streamcapture_initAudio(void* pcontext,int32_t transType,int32_t sample,int32_t channel,YangAudioCodec audioType){
	if(pcontext==NULL) return;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;



		context->audio.unitAudioTime=(channel==1)?320:sample/50;

}


void yang_streamcapture_setAudioData(void* pcontext,YangFrame* audioFrame){
	if(pcontext==NULL) return;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	context->audio.src = audioFrame->payload;
	context->audio.srcLen = audioFrame->nb;
	context->audio.audioLen = audioFrame->nb;
	context->audio.atime+=context->audio.unitAudioTime;



}
void yang_streamcapture_setAudioMetaData(void* pcontext,uint8_t* p,int32_t plen){
	if (pcontext == NULL)
		return;
	YangStreamCaptureContext *context = (YangStreamCaptureContext*) pcontext;

		context->audio.src = p;
		context->audio.srcLen = plen;
		context->audio.audioLen = plen;


}
void yang_streamcapture_setAudioFrametype(void* pcontext,int32_t frametype){
	if(pcontext==NULL) return;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	context->audio.frametype=frametype;
}

uint8_t* yang_streamcapture_getAudioData(void* pcontext){
	if(pcontext==NULL) return NULL;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	return context->audio.src ;
}
int32_t yang_streamcapture_getAudioLen(void* pcontext){
	if(pcontext==NULL) return 0;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	return context->audio.audioLen;
}
int64_t yang_streamcapture_getAudioTimestamp(void* pcontext){
	if(pcontext==NULL) return 0;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	return  context->audio.atime;
}
YangAudioCodec yang_streamcapture_getAudioType(void* pcontext){
	if(pcontext==NULL) return Yang_AED_OPUS;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	return context->audio.audioType;
}
YangFrame* yang_streamcapture_getVideoFrame(void* pcontext){
	if(pcontext==NULL) return NULL;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	context->videoFrame.payload=context->video.src;
	context->videoFrame.nb=context->video.videoLen;
	context->videoFrame.pts=yang_streamcapture_getVideoTimestamp(pcontext);
	context->videoFrame.frametype=context->video.frametype;
	return &context->videoFrame;
}
YangFrame* yang_streamcapture_getAudioFrame(void* pcontext){
	if(pcontext==NULL) return NULL;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)pcontext;
	context->audioFrame.payload=context->audio.src ;
	context->audioFrame.nb=context->audio.audioLen;
	context->audioFrame.pts= context->audio.atime;
	return &context->audioFrame;
}
void yang_create_streamCapture(YangStreamCapture* stream){
	if(stream==NULL) return;
	stream->context=yang_calloc(sizeof(YangStreamCaptureContext),1);
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)stream->context;
	context->video.videoTimestamp = 0;
	context->video.baseTimestamp = 0;
	context->video.curVideotimestamp = 0;
	context->video.preTimestamp = 0;

	context->video.src = NULL;


	context->video.videoLen = 0;
	context->video.frametype = 1;
	context->video.metaTime = 0;
	context->video.spsppsConf = NULL;



	context->audio.atime = 0;


	context->audio.unitAudioTime = 960;
	context->audio.src = NULL;
	context->audio.srcLen = 0;

	context->audio.frametype = 1;
	context->audio.audioLen = 0;
	context->audio.audioType = Yang_AED_AAC;

	stream->initAudio=yang_streamcapture_initAudio;

	stream->setAudioData=yang_streamcapture_setAudioData;
	stream->setAudioFrametype=yang_streamcapture_setAudioFrametype;
	stream->setAudioMetaData=yang_streamcapture_setAudioMetaData;

	stream->getAudioData=yang_streamcapture_getAudioData;
	stream->getAudioLen=yang_streamcapture_getAudioLen;
	stream->getAudioTimestamp=yang_streamcapture_getAudioTimestamp;
	stream->getAudioType=yang_streamcapture_getAudioType;

	stream->initVideo=yang_streamcapture_initVideo;

	stream->setMetaTimestamp=yang_streamcapture_setMetaTimestamp;
	stream->setVideoData=yang_streamcapture_setVideoData;
	stream->setVideoFrametype=yang_streamcapture_setVideoFrametype;
	stream->setVideoMeta=yang_streamcapture_setVideoMeta;

	stream->getVideoLen=yang_streamcapture_getVideoLen;
	stream->getVideoTimestamp=yang_streamcapture_getVideoTimestamp;
	stream->getVideoData=yang_streamcapture_getVideoData;
	stream->getVideoFrametype=yang_streamcapture_getVideoFrametype;
	stream->getAudioFrame=yang_streamcapture_getAudioFrame;
	stream->getVideoFrame=yang_streamcapture_getVideoFrame;

}

void yang_destroy_streamCapture(YangStreamCapture* stream){
	if(stream==NULL||stream->context==NULL) return;
	YangStreamCaptureContext* context=(YangStreamCaptureContext*)stream->context;

	if(context->video.spsppsConf){
		yang_free(context->video.spsppsConf->sps);
		yang_free(context->video.spsppsConf->pps);
		yang_free(context->video.spsppsConf);
	}

	context->audio.audioBuffer = NULL;
	context->audio.src = NULL;

	yang_free(stream->context);

}



