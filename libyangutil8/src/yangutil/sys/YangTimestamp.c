//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangutil/sys/YangTimestamp.h>

typedef struct{
	uint64_t atime ;
	uint64_t unitAudioTime;

	uint64_t baseTimestamp;
	uint64_t preTimestamp ;
	uint64_t curVideotimestamp ;
}YangTsSession;

static void yang_initVideoTime(void* psession){
	YangTsSession* context=(YangTsSession*)psession;

	if(psession==NULL)
		return;

	context->preTimestamp = 0;
}

static void yang_setVideoData(void* psession,uint64_t ts){
	YangTsSession* context=(YangTsSession*)psession;

	if(psession==NULL)
		return;

	if (context->preTimestamp == 0) {
		context->baseTimestamp = ts;
		context->curVideotimestamp = 0;

	} else {
		if (ts <= context->preTimestamp)
			return;

		context->curVideotimestamp = ts - context->baseTimestamp;
	}

	context->preTimestamp = ts;

}

static uint64_t yang_getVideoTimestamp(void* psession){
	YangTsSession* context=(YangTsSession*)psession;

	if(psession==NULL)
		return 0;

	return context->curVideotimestamp * 9 / 100;
}

static uint64_t yang_getAudioTimestamp(void* psession){
	YangTsSession* context=(YangTsSession*)psession;

	if(psession==NULL)
		return 0;

	return context->atime;
}

static void yang_initAudioTime(void* psession){
	YangTsSession* context=(YangTsSession*)psession;

	if(psession==NULL)
		return;

	context->atime = 0;
}

static void yang_setAudioSample(void* pcontext,uint32_t sample){
	YangTsSession* context=(YangTsSession*)pcontext;

	if(pcontext==NULL)
		return;

	context->unitAudioTime=sample/50;
}

static void yang_setAudioData(void* psession){
	YangTsSession* context=(YangTsSession*)psession;

	if(psession==NULL)
		return;

	context->atime+=context->unitAudioTime;
}

int32_t yang_create_timestamp(YangTimestamp* yts){
	if( yts== NULL)
		return 1;

	if(yts->session == NULL)
		yts->session= (YangTsSession*)yang_calloc(sizeof(YangTsSession),1);

	yts->initVideoTime=yang_initVideoTime;
	yts->setVideoTime=yang_setVideoData;
	yts->getVideoTimestamp=yang_getVideoTimestamp;
	yts->initAudioTime=yang_initAudioTime;
	yts->setAudioSample=yang_setAudioSample;
	yts->setAudioTime=yang_setAudioData;
	yts->getAudioTimeStamp=yang_getAudioTimestamp;

	return Yang_Ok;
}

void yang_destroy_timestamp(YangTimestamp* yts){
	if(yts==NULL || yts->session == NULL)
		return;

	yang_free(yts->session);
}
