//
// Copyright (c) 2019-2026 yanggaofeng
//


#include <yangpush/YangPushDataSession.h>

#include <yangutil/yangavinfo.h>

#include <yangutil/sys/YangLog.h>

void yang_pushData_reset(YangPushData* pushData){
	YangPacketBuffer* pushDataBuffer;

	if(pushData==NULL || pushData->pushData==NULL)
		return;

	pushDataBuffer=((YangPushDataSession*)pushData->pushData)->pushDataBuffer;

	pushDataBuffer->getIndex=pushDataBuffer->putIndex=0;
	pushDataBuffer->vsize=0;
}

YangPushData* yang_pushAudio_getData(void* psession,YangFrame* audioFrame){
	int32_t err=Yang_Ok;
	YangPushAudioDataSession* session=(YangPushAudioDataSession*)psession;

	if(session==NULL || audioFrame==NULL)
		return NULL;

	err=session->audioPacket.on_audio(session->audioPacket.session,audioFrame);

	return err==Yang_Ok?&session->audioData:NULL;
}

static int32_t yang_pushAudioData(void* psession,YangFrame* audioFrame,YangPushCallback* callback){
	YangPushData* pushData;

	if(psession==NULL || audioFrame==NULL || callback==NULL)
		return 1;

	pushData=yang_pushAudio_getData(psession,audioFrame);

	if(pushData==NULL)
		return 1;

	callback->onAudioData(callback->session,pushData);

	return Yang_Ok;
}

static void yang_audio_reset(void* psession){
	YangPushAudioDataSession* session=(YangPushAudioDataSession*)psession;
	if(session==NULL)
		return;
	yang_pushData_reset(&session->audioData);
	if(session->audioPacket.session)
		session->audioPacket.reset(session->audioPacket.session);
}

int32_t yang_create_pushAudioData(YangPushAudioData* audioData,YangAudioCodec codec,uint32_t sample,uint32_t channel){
	uint32_t rtpCount=50;
	uint32_t payloadSize=1024;
	YangPushAudioDataSession* session=NULL;

	if(audioData==NULL)
		return 1;

	if(codec==Yang_AED_PCMA ||codec==Yang_AED_PCMU)
		payloadSize=256;

	session=(YangPushAudioDataSession*)yang_calloc(sizeof(YangPushAudioDataSession),1);
	audioData->session=session;

	session->pushDataBuffer=yang_create_packetBuffer(rtpCount,payloadSize);

	yang_create_pushData(&session->audioData,session->pushDataBuffer);

	yang_create_pushDataAudio(&session->audioPacket,(YangPushDataSession*)session->audioData.pushData);

	audioData->pushAudioData=yang_pushAudioData;
	audioData->reset=yang_audio_reset;
	return Yang_Ok;
}

void yang_destroy_pushAudioData(YangPushAudioData* audioData){
	YangPushAudioDataSession* session;
	if(audioData==NULL||audioData->session==NULL)
		return;

	session=(YangPushAudioDataSession*)audioData->session;

	yang_destroy_pushData(&session->audioData);
	yang_destroy_packetBuffer(session->pushDataBuffer);
	yang_free(session->pushDataBuffer);

	yang_destroy_pushDataAudio(&session->audioPacket);

	yang_free(audioData->session);
}

YangPushData* yang_pushVideo_getData(void* psession,YangFrame* videoFrame){
	int32_t err=Yang_Ok;
	int32_t keyframePos=-1;
	YangPushVideoDataSession* session=(YangPushVideoDataSession*)psession;

	if(session==NULL || videoFrame==NULL)
		return NULL;

	if (videoFrame->frametype == YANG_Frametype_I){

		if(session->codec==Yang_VED_H264)
			keyframePos=yang_nalu_getH264KeyframePos(videoFrame->payload,videoFrame->nb);
		else if(session->codec==Yang_VED_H265)
			keyframePos=yang_nalu_getH265KeyframePos(videoFrame->payload,videoFrame->nb);

		if(keyframePos==-1)
			return NULL;

		if(session->codec==Yang_VED_H264)
			yang_meta_createH264(session->conf,videoFrame->payload,videoFrame->nb);
		else if(session->codec==Yang_VED_H265)
			yang_meta_createH265(session->conf,videoFrame->payload,videoFrame->nb);

		session->videoPacket.on_meta(session->videoPacket.session,session->conf,videoFrame->pts);

		videoFrame->payload = videoFrame->payload + keyframePos + 4;
		videoFrame->nb = videoFrame->nb - (keyframePos + 4);
	}

	err=session->videoPacket.on_video(session->videoPacket.session,videoFrame);
	//((YangPushDataSession*)session->videoData.pushData)->length=0;

	return err==Yang_Ok?&session->videoData:NULL;
}

static int32_t yang_pushVideoData(void* psession,YangFrame* videoFrame,YangPushCallback* callback){
	YangPushData* pushData;
	YangPushVideoDataSession* session=(YangPushVideoDataSession*)psession;

 	if(session==NULL || videoFrame==NULL || callback==NULL)
 		return 1;

 	pushData=yang_pushVideo_getData(session,videoFrame);

 	if(pushData==NULL)
 		return 1;

 	callback->onVideoData(callback->session,pushData);

 	return Yang_Ok;
 }

static void yang_video_reset(void* psession){
	YangPushVideoDataSession* session=(YangPushVideoDataSession*)psession;
	if(session==NULL)
		return;

	yang_pushData_reset(&session->videoData);
	if(session->videoPacket.session)
		session->videoPacket.reset(session->videoPacket.session);
}

int32_t yang_create_pushVideoData(YangPushVideoData* videodata,YangVideoCodec codec,uint32_t pktCount){
	uint32_t rtpCount=pktCount;
	uint32_t payloadSize=kRtpPacketSize;
	YangPushVideoDataSession* session;

	if(videodata==NULL)
		return 1;

	session=(YangPushVideoDataSession*)yang_calloc(sizeof(YangPushVideoDataSession),1);

	videodata->session=session;
	session->pushDataBuffer=yang_create_packetBuffer(rtpCount,payloadSize);

	session->codec=codec;
	session->conf=(YangH2645Conf*)yang_calloc(sizeof(YangH2645Conf),1);
	yang_create_pushData(&session->videoData,session->pushDataBuffer);

	if(codec==Yang_VED_H264)
		yang_create_pushVideoDataH264(&session->videoPacket,(YangPushDataSession*)session->videoData.pushData);
	else if(codec==Yang_VED_H265)
		yang_create_pushVideoDataH265(&session->videoPacket,(YangPushDataSession*)session->videoData.pushData);

	videodata->reset=yang_video_reset;
	videodata->pushVideoData=yang_pushVideoData;
	return Yang_Ok;
}

void yang_destroy_pushVideoData(YangPushVideoData* videoData){
	YangPushVideoDataSession* session;

	if(videoData==NULL || videoData->session==NULL)
		return;
	session=(YangPushVideoDataSession*)videoData->session;

	yang_destroy_pushData(&session->videoData);

	if(session->codec==Yang_VED_H264)
		yang_destroy_pushVideoDataH264(&session->videoPacket);
	else if(session->codec==Yang_VED_H265)
		yang_destroy_pushVideoDataH265(&session->videoPacket);


	yang_destroy_packetBuffer(session->pushDataBuffer);
	yang_free(session->pushDataBuffer);

	yang_free(session->conf);

	yang_free(videoData->session);
}
