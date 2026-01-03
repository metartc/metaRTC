//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangrtc/YangPeerConnection.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangEndian.h>
#include <yangutil/sys/YangTimestamp.h>

#include <yangpush/YangPushDataSession.h>


typedef struct{
	YangPacketBuffer *videoPushBuffer;

	YangPushData *audioData;
	YangPushData *videoData;

	YangPushAudioData* audioPushData;
	YangPushVideoData* videoPushData;

	YangTimestamp ts ;

}YangPaceSession;


static int32_t yang_resetAudioTime(void* psession){
	YangPaceSession* session=(YangPaceSession*)psession;
	if(session==NULL)
		return 1;

	session->ts.initAudioTime(session->ts.session);
	return Yang_Ok;
}

static int32_t yang_resetVideoTime(void* psession){
	YangPaceSession* session=(YangPaceSession*)psession;

	if(session==NULL)
		return 1;

	session->ts.initVideoTime(session->ts.session);
	return Yang_Ok;
}

static YangPushData* yang_pacer_getFrameAudio(void* psession,YangFrame* frame){
	YangPaceSession* session=(YangPaceSession*)psession;
	YangPushData* pushData;
	if(session==NULL || session->audioPushData==NULL)
		return NULL;

	session->ts.setAudioTime(session->ts.session);
	frame->pts=session->ts.getAudioTimeStamp(session->ts.session);

	pushData=yang_pushAudio_getData(session->audioPushData->session,frame);
	return pushData;
}

static YangPushData* yang_pacer_getFrameVideo(void* psession,YangFrame* frame){
	YangPaceSession* session=(YangPaceSession*)psession;
	YangPushData* pushData;
	if(session==NULL || session->videoPushData==NULL)
		return NULL;

	session->ts.setVideoTime(session->ts.session,frame->pts);
	frame->pts=session->ts.getVideoTimestamp(session->ts.session);
	pushData=yang_pushVideo_getData(session->videoPushData->session,frame);
	return pushData;
}


static int32_t yang_initAudio(void* psession,YangAudioCodec acodec,int32_t sample,int32_t channel){
	YangPaceSession* session=(YangPaceSession*)psession;

	if(session==NULL)
		return 1;
	if(session->audioPushData==NULL)
		session->audioPushData=(YangPushAudioData*)yang_calloc(sizeof(YangPushAudioData),1);

	yang_create_pushAudioData(session->audioPushData,acodec,sample,channel);
	session->audioData=&((YangPushAudioDataSession*)session->audioPushData->session)->audioData;
	session->ts.setAudioSample(session->ts.session,sample);
	session->ts.initAudioTime(session->ts.session);
	return Yang_Ok;
}

static int32_t yang_initVideo(void* psession,YangVideoCodec vcodec,int32_t pktCount){
	YangPaceSession* session=(YangPaceSession*)psession;

	if(session==NULL)
		return 1;
	if(session->videoPushData==NULL)
		session->videoPushData=(YangPushVideoData*)yang_calloc(sizeof(YangPushVideoData),1);

	yang_create_pushVideoData(session->videoPushData,vcodec,pktCount);
	session->videoData=&((YangPushVideoDataSession*)session->videoPushData->session)->videoData;
	session->videoPushBuffer=((YangPushVideoDataSession*)session->videoPushData->session)->pushDataBuffer;
	session->ts.initVideoTime(session->ts.session);
	return Yang_Ok;
}


static int32_t yang_reset(void* psession){
	YangPaceSession* session=(YangPaceSession*)psession;
	if(session==NULL)
		return 1;

	session->ts.initVideoTime(session->ts.session);
	session->ts.initAudioTime(session->ts.session);

	if(session->audioPushData){
		session->audioPushData->reset(session->audioPushData->session);
	}

	if(session->videoPushData){
		session->videoPushData->reset(session->videoPushData->session);
	}
	return Yang_Ok;
}

int32_t yang_create_pacer(YangPacer* pacer){
	YangPaceSession* session;
	if(pacer==NULL)
		return 1;

	session=(YangPaceSession*)yang_calloc(sizeof(YangPaceSession),1);
	pacer->session=session;

	//session->pushData=NULL;
	session->videoPushBuffer=NULL;

	yang_create_timestamp(&session->ts);

	pacer->resetAudioTime=yang_resetAudioTime;
	pacer->resetVideoTime=yang_resetVideoTime;
	pacer->initAudio=yang_initAudio;
	pacer->initVideo=yang_initVideo;
	pacer->getAudioData=yang_pacer_getFrameAudio;
	pacer->getVideoData=yang_pacer_getFrameVideo;

	pacer->reset=yang_reset;

	return Yang_Ok;
}

void yang_destroy_pacer(YangPacer* pacer){
	YangPaceSession* session;

	if(pacer==NULL||pacer->session==NULL)
		return;

	session=(YangPaceSession*)pacer->session;
	yang_destroy_timestamp(&session->ts);
	if(session->audioPushData){
		yang_destroy_pushAudioData(session->audioPushData);
		yang_free(session->audioPushData);
	}

	if(session->videoPushData){
		yang_destroy_pushVideoData(session->videoPushData);
		yang_free(session->videoPushData);
	}

	yang_free(pacer->session);
}


