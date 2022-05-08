//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangp2p/YangP2pRtc.h>
#include <yangp2p/YangP2pCommon.h>
#include <yangstream/YangStreamCapture.h>
#include <yangstream/YangStreamType.h>
#include <yangavutil/video/YangNalu.h>
#include <yangavutil/video/YangMeta.h>
#include <yangavutil/video/YangVideoEncoderMeta.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangSysMessageI.h>

#include <stdio.h>
void g_p2p_rtcrecv_sendData(void* context,char* data,int32_t nb){
	YangP2pRtc* rtcHandle=(YangP2pRtc*)context;
	rtcHandle->publishMsg(data, nb);
}
void g_p2p_rtcrecv_sslAlert(void* context,int32_t uid,char* type,char* desc){
	if(context==NULL||type==NULL||desc==NULL) return;
	YangP2pRtc* rtc=(YangP2pRtc*)context;

	if(strcmp(type,"warning")==0&&strcmp(desc,"CN")==0){
		rtc->removePeer(uid);
	}
}
void g_p2p_rtcrecv_receiveAudio(void* user,YangFrame *audioFrame){
	if(user==NULL || audioFrame==NULL) return;
	YangP2pRtc* rtcHandle=(YangP2pRtc*)user;
	rtcHandle->receiveAudio(audioFrame);
}
void g_p2p_rtcrecv_receiveVideo(void* user,YangFrame *videoFrame){
	if(user==NULL || videoFrame==NULL) return;
    YangP2pRtc* rtcHandle=(YangP2pRtc*)user;
	rtcHandle->receiveVideo(videoFrame);
}

void g_p2p_rtcrecv_receiveMsg(void* user,YangFrame *msgFrame){
	if(user==NULL) return;

	YangP2pRtc* rtcHandle=(YangP2pRtc*)user;
	rtcHandle->receiveMsg(msgFrame);
}
YangP2pRtc::YangP2pRtc(YangContext *pcontext) {
	m_context = pcontext;

	m_in_videoBuffer = NULL;
	m_in_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_out_audioBuffer = NULL;
	m_isStart = 0;
	m_isConvert = 0;
	m_uidSeq=0;
	m_vmd = NULL;
	m_audioEncoderType = m_context->avinfo.audio.audioEncoderType;
	m_netState = 1;
	m_isInit = 0;
	isPublished = 0;

	m_transType=Yang_Webrtc;
	notifyState=0;
	//m_streamInit = false;


	m_clientUid=-1;
	m_playCount=0;
	m_p2pRtcI=NULL;

	m_context->channeldataSend.context=this;
	m_context->channeldataSend.sendData=g_p2p_rtcrecv_sendData;

	pthread_mutex_init(&m_mutex, NULL);
}

YangP2pRtc::~YangP2pRtc() {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	for(size_t i=0;i<m_pushs.size();i++){
		m_pushs.at(i)->disconnectServer(m_pushs.at(i)->context);
		yang_destroy_rtcstream_handle(m_pushs.at(i));
		yang_free(m_pushs.at(i));
	}
	m_pushs.clear();
	m_context = NULL;
	m_in_videoBuffer = NULL;
	m_in_audioBuffer = NULL;
	m_vmd = NULL;
	pthread_mutex_destroy(&m_mutex);
}

void YangP2pRtc::receiveAudio(YangFrame *audioFrame) {
    if(audioFrame==NULL||!audioFrame->payload) return;
	if(m_out_audioBuffer) m_out_audioBuffer->putPlayAudio(audioFrame);
}

void YangP2pRtc::receiveVideo(YangFrame *videoFrame) {
    if(videoFrame==NULL||videoFrame->payload==NULL) return;

   if(m_out_videoBuffer) m_out_videoBuffer->putEVideo(videoFrame);
}
void YangP2pRtc::receiveMsg(YangFrame* msgFrame){

	if(m_context->channeldataRecv.receiveData)
        m_context->channeldataRecv.receiveData(m_context->channeldataRecv.context,(char*)msgFrame->payload,msgFrame->nb);
}
int32_t YangP2pRtc::publishMsg(char* data,int32_t nb){
	int32_t ret=0;
    YangFrame frame;
    memset(&frame,0,sizeof(YangFrame));
    frame.nb=nb;
    frame.payload=(uint8_t*)data;
	for(size_t i=0;i<m_pushs.size();i++){
		YangRtcHandle* rtc=m_pushs.at(i);

		if(rtc->getState(rtc->context)){
			ret = rtc->publishMsg(rtc->context,&frame);
		}
	}
	return ret;


}
YangVideoDecoderBuffer* YangP2pRtc::getOutVideoList(){
	return m_out_videoBuffer;
}

void YangP2pRtc::setMediaConfig(int32_t puid, YangAudioParam *audio,YangVideoParam *video){
	//if (m_context)		m_context->remoteAudio[puid] = audio;
	//	if (m_context)	m_context->remoteVideo[puid] = video;
	//	yang_post_message(YangM_Sys_NofityMediaParam, puid, NULL);
}


int32_t YangP2pRtc::connectPeer(int32_t nettype, string server,int32_t localPort,int32_t pport,string app,string stream) {
	int32_t ret = 0;
	YangRtcHandle* sh=(YangRtcHandle*)calloc(sizeof(YangRtcHandle),1);
	memset(&sh->streamconfig,0,sizeof(sh->streamconfig));

	strcpy(sh->streamconfig.app,app.c_str());
	sh->streamconfig.streamOptType=Yang_Stream_Both;

	strcpy(sh->streamconfig.remoteIp,server.c_str());
	sh->streamconfig.remotePort=pport;

	m_clientUid=m_uidSeq++;
	strcpy(sh->streamconfig.stream,stream.c_str());
	sh->streamconfig.uid=m_clientUid;

	sh->streamconfig.isServer=0;
	sh->streamconfig.localPort=localPort;

	sh->streamconfig.recvcb.context=this;
	sh->streamconfig.recvcb.receiveAudio=g_p2p_rtcrecv_receiveAudio;
	sh->streamconfig.recvcb.receiveVideo=g_p2p_rtcrecv_receiveVideo;
	sh->streamconfig.recvcb.receiveMsg=g_p2p_rtcrecv_receiveMsg;
	yang_create_rtcstream_handle(sh, &m_context->avinfo,&m_context->stream);

	sh->init(sh,sh->context);


	if (sh->getState(sh->context))		return Yang_Ok;
	ret = sh->connectRtcServer(sh->context);

	if (ret)		return ret;
	m_pushs.push_back(sh);
	if(m_context) m_context->streams.connectNotify(sh->streamconfig.uid,sh->streamconfig.streamOptType, true);
	if(m_pushs.size()==1){
		yang_reindex(m_in_audioBuffer);
		yang_reindex(m_in_videoBuffer);
	}
	if(m_p2pRtcI) m_p2pRtcI->sendKeyframe();
	//if(m_p2pRtcI)  m_p2pRtcI->startCapture();
	return Yang_Ok;

}

int32_t YangP2pRtc::addPeer(char* sdp,char* answer,char* remoteIp,int32_t localPort,int* phasplay) {
	int32_t ret = 0;
	YangRtcHandle* sh=(YangRtcHandle*)calloc(sizeof(YangRtcHandle),1);
	memset(&sh->streamconfig,0,sizeof(sh->streamconfig));
	sh->streamconfig.uid=m_uidSeq++;
	sh->streamconfig.localPort=localPort;
	sh->streamconfig.isServer=1;
	sh->streamconfig.streamOptType=Yang_Stream_Both;
	strcpy(sh->streamconfig.remoteIp,remoteIp);

	sh->streamconfig.sslCallback.context=this;
	sh->streamconfig.sslCallback.sslAlert=g_p2p_rtcrecv_sslAlert;
	sh->streamconfig.recvcb.context=this;
	sh->streamconfig.recvcb.receiveAudio=g_p2p_rtcrecv_receiveAudio;
	sh->streamconfig.recvcb.receiveVideo=g_p2p_rtcrecv_receiveVideo;
	sh->streamconfig.recvcb.receiveMsg=g_p2p_rtcrecv_receiveMsg;

	yang_create_rtcstream_handle(sh, &m_context->avinfo,&m_context->stream);

    sh->init(sh,sh->context);


	if (sh->getState(sh->context))		return Yang_Ok;
	ret = sh->startRtc(sh->context,sdp);


	if (ret)		return ret;
	ret = sh->getAnswerSdp(sh->context,answer);

	m_pushs.push_back(sh);
	if(sh->streamconfig.streamOptType==Yang_Stream_Both||sh->streamconfig.streamOptType==Yang_Stream_Play){
		m_playCount++;
		yang_post_message(YangM_P2p_Play_Start,0,NULL);

	}
	if(m_context) m_context->streams.connectNotify(sh->streamconfig.uid,sh->streamconfig.streamOptType, true);
	if(m_pushs.size()==1){
		yang_reindex(m_in_audioBuffer);
		yang_reindex(m_in_videoBuffer);
	}
	if(m_p2pRtcI) m_p2pRtcI->sendKeyframe();
    *phasplay=sh->streamconfig.streamOptType==Yang_Stream_Both?1:0;
    //if(m_p2pRtcI)  m_p2pRtcI->startCapture();
	return Yang_Ok;
}

int32_t YangP2pRtc::removePeer(int32_t puid){

	pthread_mutex_lock(&m_mutex);
	m_removeList.push_back(puid);
	pthread_mutex_unlock(&m_mutex);
	return Yang_Ok;
}

int32_t YangP2pRtc::erasePeer(int32_t uid){

	for(size_t i=0;i<m_pushs.size();i++){

		if(m_pushs.at(i)->streamconfig.uid==uid){
			YangStreamOptType streamOpt=m_pushs.at(i)->streamconfig.streamOptType;
			yang_destroy_rtcstream_handle(m_pushs.at(i));
			yang_free(m_pushs.at(i));
			m_pushs.erase(m_pushs.begin()+i);
			if(m_context) m_context->streams.connectNotify(uid,streamOpt, false);
			if(streamOpt==Yang_Stream_Both||streamOpt==Yang_Stream_Play){
				m_playCount--;
				if(m_playCount<1)   yang_post_message(YangM_P2p_Play_Stop,0,NULL);
	            if(m_p2pRtcI) m_p2pRtcI->removePlayBuffer(uid,m_playCount);
			}

			return Yang_Ok;
		}
	}
	//if(m_pushs.size()==0&&m_p2pRtcI)  m_p2pRtcI->stopCapture();
	return Yang_Ok;
}

void YangP2pRtc::removeStream(){
	//size_t i = 0;
	for(;m_removeList.size()>0;){
		int32_t puid=m_removeList.front();
		m_removeList.erase(m_removeList.begin());
		erasePeer(puid);

	}
}

int32_t YangP2pRtc::connectMediaServer() {
	if(m_pushs.size()>0) return Yang_Ok;

	return Yang_Ok;
}
int32_t YangP2pRtc::disConnectPeer() {
	removePeer(m_clientUid);
	m_clientUid=-1;
	return Yang_Ok;
}
void YangP2pRtc::stop() {
	m_isConvert = 0;
}

void YangP2pRtc::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

void YangP2pRtc::setInAudioList(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangP2pRtc::setInVideoList(YangVideoEncoderBuffer *pbuf) {
	m_in_videoBuffer = pbuf;
}
void YangP2pRtc::setInVideoMetaData(YangVideoMeta *pvmd) {
	m_vmd = pvmd;
}
void YangP2pRtc::setOutAudioList(YangAudioEncoderBuffer *pbuf){
	m_out_audioBuffer=pbuf;
}
void YangP2pRtc::setOutVideoList(YangVideoDecoderBuffer *pbuf){
	m_out_videoBuffer=pbuf;
}

int32_t YangP2pRtc::publishVideoData(YangStreamCapture* data){
	int32_t ret=0;

	for(size_t i=0;i<m_pushs.size();i++){
		YangRtcHandle* rtc=m_pushs.at(i);

		if(rtc->getState(rtc->context)){
			ret = rtc->publishVideo(rtc->context,data);
		}
	}
	return ret;
}
int32_t YangP2pRtc::publishAudioData(YangStreamCapture* data){
	int32_t ret=0;
	for(size_t i=0;i<m_pushs.size();i++){
		YangRtcHandle* rtc=m_pushs.at(i);
		if(rtc->getState(rtc->context)){
			ret = rtc->publishAudio(rtc->context,data);
		}
	}
	return ret;
}
void YangP2pRtc::startLoop() {

	isPublished = 0;
	m_isConvert = 1;

	YangStreamCapture data;
	memset(&data,0,sizeof(YangStreamCapture));
	yang_create_streamCapture(&data);
	YangFrame audioFrame;
	YangFrame videoFrame;
	memset(&audioFrame,0,sizeof(YangFrame));
	memset(&videoFrame,0,sizeof(YangFrame));
	data.initAudio(data.context,m_context->avinfo.sys.transType,m_context->avinfo.audio.sample, m_context->avinfo.audio.channel,
			(YangAudioCodec) m_context->avinfo.audio.audioEncoderType);
	data.initVideo(data.context,m_context->avinfo.sys.transType);
	YangVideoCodec videoType =	(YangVideoCodec) m_context->avinfo.video.videoEncoderType;

	int32_t ret = Yang_Ok;
	isPublished = 1;
	notifyState=1;
	YangVideoMeta* vmd=NULL;
	if(m_context->avinfo.enc.createMeta==0){
		vmd=(YangVideoMeta*)calloc(sizeof( YangVideoMeta),1);
	}

	YangH264NaluData nalu;

	while (m_isConvert == 1) {
		if ((m_in_videoBuffer && m_in_videoBuffer->size() == 0)
				&& (m_in_audioBuffer && m_in_audioBuffer->size() == 0)) {
			yang_usleep(2000);
			continue;
		}
		if (m_pushs.size() == 0) {
			yang_usleep(2000);
			continue;
		}

		if(m_removeList.size()>0){
			pthread_mutex_lock(&m_mutex);
			removeStream();
			pthread_mutex_unlock(&m_mutex);
			continue;
		}

		if (m_in_audioBuffer && m_in_audioBuffer->size() > 0) {
			audioFrame.payload = m_in_audioBuffer->getAudioRef(&audioFrame);
			data.setAudioData(data.context,&audioFrame);
			ret = publishAudioData(&data);
		}

		if (m_in_videoBuffer && m_in_videoBuffer->size() > 0) {

			videoFrame.payload = m_in_videoBuffer->getEVideoRef(&videoFrame);

			if (videoFrame.frametype == YANG_Frametype_I) {

				if (m_vmd) {
					data.setVideoMeta(data.context,m_vmd->livingMeta.buffer,m_vmd->livingMeta.bufLen, videoType);
				} else {
					if (!vmd->isInit) {
						if (videoType == Yang_VED_264) {
							yang_createH264Meta(vmd, &videoFrame);
							yang_getConfig_Flv_H264(&vmd->mp4Meta,vmd->livingMeta.buffer,&vmd->livingMeta.bufLen);
						} else if (videoType == Yang_VED_265) {
							yang_createH265Meta(vmd, &videoFrame);
							yang_getConfig_Flv_H265(&vmd->mp4Meta,vmd->livingMeta.buffer,&vmd->livingMeta.bufLen);
						}
					}
					data.setVideoMeta(data.context,vmd->livingMeta.buffer,vmd->livingMeta.bufLen, videoType);
				}
				data.setVideoFrametype(data.context,YANG_Frametype_Spspps);
				data.setMetaTimestamp(data.context,videoFrame.pts);
				ret = publishVideoData(&data);

				if (!m_context->avinfo.enc.createMeta) {
					memset(&nalu, 0, sizeof(YangH264NaluData));
					yang_parseH264Nalu(&videoFrame, &nalu);
					if (nalu.keyframePos > -1) {
						videoFrame.payload += nalu.keyframePos + 4;
						videoFrame.nb -= (nalu.keyframePos + 4);

					} else {
						videoFrame.payload = NULL;
						continue;
					}
				}
			}

			data.setVideoData(data.context,&videoFrame, videoType);

			if((ret = publishVideoData(&data))!=Yang_Ok){

			}

		}			//end
	}
	isPublished = 0;
	yang_destroy_streamCapture(&data);
	yang_free(vmd);
}
