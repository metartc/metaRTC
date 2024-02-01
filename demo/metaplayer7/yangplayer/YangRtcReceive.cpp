//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangRtcReceive.h"


#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangThread.h>

void g_rtcrecv_receiveAudio(void* user,YangFrame *audioFrame){
	if(user==NULL) return;
	YangRtcReceive* rtcHandle=(YangRtcReceive*)user;
	rtcHandle->receiveAudio(audioFrame);
}
void g_rtcrecv_receiveVideo(void* user,YangFrame *videoFrame){
	if(user==NULL) return;
	YangRtcReceive* rtcHandle=(YangRtcReceive*)user;
	rtcHandle->receiveVideo(videoFrame);
}

void g_rtcrecv_receiveMsg(void* user,YangFrame *msgFrame){
	if(user==NULL) return;
	YangRtcReceive* rtcHandle=(YangRtcReceive*)user;
	rtcHandle->receiveMsg(msgFrame);

}
int32_t g_rtcrecv_sendRtcMessage(void* user,int puid,YangRtcMessageType mess){
	if(user==NULL) return 1;
	YangRtcReceive* rtcHandle=(YangRtcReceive*)user;
	if(rtcHandle->m_recv->sendRtcMessage) rtcHandle->m_recv->sendRtcMessage(&rtcHandle->m_recv->peer,mess);
	return Yang_Ok;
}

YangRtcReceive::YangRtcReceive(YangContext* pcontext,YangSysMessageI* pmessage) {
	m_message=pmessage;
	m_context=pcontext;
	m_isStart = 0;
	m_out_videoBuffer = NULL;
	m_out_audioBuffer = NULL;
	m_isReceived = 0;
	m_loops = 0;
	m_headLen = 1; //pcontext->audio.audioDecoderType == 0 ? 2 : 1;
	m_recv = NULL;
    m_url=NULL;
	m_waitState = 0;
	yang_thread_mutex_init(&m_lock,NULL);
	yang_thread_cond_init(&m_cond_mess,NULL);

	m_context->sendRtcMessage.context=this;
	m_context->sendRtcMessage.sendRtcMessage=g_rtcrecv_sendRtcMessage;

}

YangRtcReceive::~YangRtcReceive() {
	disConnect();
	if (m_loops) {
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	yang_delete(m_recv);


	m_out_audioBuffer = NULL;
	m_out_videoBuffer = NULL;
	m_message=NULL;
	yang_thread_mutex_destroy(&m_lock);
	yang_thread_cond_destroy(&m_cond_mess);
    yang_free(m_url);
}

void YangRtcReceive::disConnect() {
	if (m_recv)
		m_recv->close(&m_recv->peer);
	stop();
	yang_destroy_peerConnection(m_recv);
	yang_free(m_recv);

}
void YangRtcReceive::setBuffer(YangAudioEncoderBuffer *al,YangVideoDecoderBuffer *vl) {
	m_out_audioBuffer = al;
	m_out_videoBuffer = vl;
}

void YangRtcReceive::setMediaConfig(int32_t puid, YangAudioParam *audio,YangVideoParam *video){

}

int32_t YangRtcReceive::publishMsg(YangFrame* msgFrame){
    if(m_recv) {
    	return m_recv->on_message(&m_recv->peer,msgFrame);
    }
	return 1;
}
int32_t YangRtcReceive::receiveMsg(YangFrame* msgFrame){


	return Yang_Ok;
}

void YangRtcReceive::receiveAudio(YangFrame *audioFrame) {
    if(audioFrame==NULL||!audioFrame->payload) return;
	m_out_audioBuffer->putPlayAudio(audioFrame);

}
void YangRtcReceive::receiveVideo(YangFrame *videoFrame) {
    if(videoFrame==NULL||videoFrame->payload==NULL) return;
    m_out_videoBuffer->putEVideo(videoFrame);
}

int32_t YangRtcReceive::init(int32_t puid, char* localIp,
		char* server, int32_t pport, char* app, char* stream) {
	if (!m_recv) m_recv=(YangPeerConnection*)calloc(sizeof(YangPeerConnection),1);
	//memset(&m_recv->peer.streamconfig,0,sizeof(m_recv->peer.streamconfig));
	m_recv->peer.streamconfig.localPort = m_context->avinfo.rtc.rtcLocalPort;
	strcpy(m_recv->peer.streamconfig.remoteIp,server);
	m_recv->peer.streamconfig.remotePort = pport;
	strcpy(m_recv->peer.streamconfig.app,app);
	strcpy(m_recv->peer.streamconfig.stream,stream);
	m_recv->peer.streamconfig.uid = puid;
    m_recv->peer.streamconfig.direction = YangRecvonly;

	m_recv->peer.streamconfig.recvCallback.context=this;
	m_recv->peer.streamconfig.recvCallback.receiveAudio=g_rtcrecv_receiveAudio;
	m_recv->peer.streamconfig.recvCallback.receiveVideo=g_rtcrecv_receiveVideo;
	m_recv->peer.streamconfig.recvCallback.receiveMsg=g_rtcrecv_receiveMsg;

	m_recv->peer.streamconfig.recvCallback.context=this;

	memcpy(&m_recv->peer.streamconfig.rtcCallback,&m_context->rtcCallback,sizeof(YangRtcCallback));
	m_recv->peer.avinfo=&m_context->avinfo;
	yang_create_peerConnection(m_recv);
    m_recv->init(&m_recv->peer);
    m_recv->addAudioTrack(&m_recv->peer,Yang_AED_OPUS);
    m_recv->addVideoTrack(&m_recv->peer,Yang_VED_H264);
    m_recv->addTransceiver(&m_recv->peer,m_recv->peer.streamconfig.direction);
	return Yang_Ok;
}



int32_t YangRtcReceive::init(int32_t puid,char* url){
    if (!m_recv) m_recv=(YangPeerConnection*)calloc(sizeof(YangPeerConnection),1);
    //memset(&m_recv->peer.streamconfig,0,sizeof(m_recv->peer.streamconfig));
    m_recv->peer.streamconfig.localPort = m_context->avinfo.rtc.rtcLocalPort++;

    m_recv->peer.streamconfig.uid = puid;
    m_recv->peer.streamconfig.direction = YangRecvonly;

    m_recv->peer.streamconfig.recvCallback.context=this;
    m_recv->peer.streamconfig.recvCallback.receiveAudio=g_rtcrecv_receiveAudio;
    m_recv->peer.streamconfig.recvCallback.receiveVideo=g_rtcrecv_receiveVideo;
    m_recv->peer.streamconfig.recvCallback.receiveMsg=g_rtcrecv_receiveMsg;

    m_recv->peer.streamconfig.recvCallback.context=this;

    memcpy(&m_recv->peer.streamconfig.rtcCallback,&m_context->rtcCallback,sizeof(YangRtcCallback));
    m_recv->peer.avinfo=&m_context->avinfo;
    yang_create_peerConnection(m_recv);
    m_recv->addAudioTrack(&m_recv->peer,Yang_AED_OPUS);
    m_recv->addVideoTrack(&m_recv->peer,Yang_VED_H264);
    m_recv->addTransceiver(&m_recv->peer,m_recv->peer.streamconfig.direction);
    int len=strlen(url);
    if(m_url==NULL) m_url=(char*)yang_malloc(len+1);
    memcpy(m_url,url,len);
    m_url[len]=0;
    return Yang_Ok;
}


void YangRtcReceive::stop() {
	m_loops = 0;
	if (m_recv)
		m_recv->close(&m_recv->peer);

	if (m_waitState) {
		yang_thread_mutex_lock(&m_lock);
		yang_thread_cond_signal(&m_cond_mess);
		yang_thread_mutex_unlock(&m_lock);

	}
}
void YangRtcReceive::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangRtcReceive::startLoop() {
	yang_reindex(m_out_audioBuffer);
	yang_reindex(m_out_videoBuffer);
	m_loops = 1;
	m_isReceived = 1;
    int32_t err=m_url?m_recv->connectWhipWhepServer(&m_recv->peer,m_url):m_recv->connectSfuServer(&m_recv->peer);
    if (err!=Yang_Ok) {
		m_loops=0;
		if(m_message) m_message->failure(err);
	}else{
		if(m_message) m_message->success();
	}
    yang_free(m_url);
	yang_thread_mutex_lock(&m_lock);
	while (m_loops == 1) {
		m_waitState = 1;

		yang_thread_cond_wait(&m_cond_mess, &m_lock);
		m_waitState = 0;
	}	            		//end while

	if (m_recv)
		m_recv->close(&m_recv->peer);
	m_isReceived = 0;
	yang_thread_mutex_unlock(&m_lock);
}
