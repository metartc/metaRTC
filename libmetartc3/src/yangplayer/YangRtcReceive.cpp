//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangRtcReceive.h"

#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>

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
	m_waitState = 0;
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_cond_mess,NULL);
//	m_lock = PTHREAD_MUTEX_INITIALIZER;
//	m_cond_mess = PTHREAD_COND_INITIALIZER;
	m_recvCallback.receiveAudio=g_rtcrecv_receiveAudio;
		m_recvCallback.receiveVideo=g_rtcrecv_receiveVideo;
		m_recvCallback.context=this;

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
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_cond_mess);
}

void YangRtcReceive::disConnect() {
	if (m_recv)
		m_recv->disconnectServer(m_recv->context);
	stop();
	yang_destroy_rtcstream_handle(m_recv);
	yang_free(m_recv);

}
void YangRtcReceive::setBuffer(YangAudioEncoderBuffer *al,YangVideoDecoderBuffer *vl) {
	m_out_audioBuffer = al;
	m_out_videoBuffer = vl;
}
void YangRtcReceive::setMediaConfig(int32_t puid, YangAudioParam *audio,YangVideoParam *video){

}
void YangRtcReceive::receiveAudio(YangFrame *audioFrame) {
    if(audioFrame==NULL||!audioFrame->payload) return;
	m_out_audioBuffer->putPlayAudio(audioFrame);
}
void YangRtcReceive::receiveVideo(YangFrame *videoFrame) {
    if(videoFrame==NULL||videoFrame->payload==NULL) return;
    m_out_videoBuffer->putEVideo(videoFrame);


}

int32_t YangRtcReceive::init(int32_t puid, string localIp, int32_t localPort,
		string server, int32_t pport, string app, string stream) {
	//m_conf.localIp = localIp;
	//memset(m_conf.localIp,0,sizeof(m_conf.localIp));
	memset(&m_conf,0,sizeof(m_conf));
	strcpy(m_conf.localIp,localIp.c_str());
	m_conf.localPort = localPort;
	strcpy(m_conf.serverIp,server.c_str());
	m_conf.serverPort = pport;
	strcpy(m_conf.app,app.c_str());
	strcpy(m_conf.stream,stream.c_str());
	m_conf.uid = puid;
	m_conf.streamOptType = Yang_Stream_Play;
	if (!m_recv){
		m_recv=(YangRtcHandle*)calloc(sizeof(YangRtcHandle),1);
		yang_create_rtcstream_handle(m_recv, &m_context->avinfo,&m_context->stream);
	}
	m_recv->recvcb=&m_recvCallback;
	m_recv->init(m_recv,m_recv->context,&m_conf);
	return Yang_Ok;

}
void YangRtcReceive::stop() {
	m_loops = 0;
	if (m_recv)
		m_recv->disconnectServer(m_recv->context);
	//pthread_mutex_unlock(&m_lock);

	if (m_waitState) {
		pthread_mutex_lock(&m_lock);
		pthread_cond_signal(&m_cond_mess);
		pthread_mutex_unlock(&m_lock);

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
	int err=Yang_Ok;
	if ((err=m_recv->connectRtcServer(m_recv->context))!=Yang_Ok) {
		m_loops=0;
		if(m_message) m_message->failure(err);
	}else{
		if(m_message) m_message->success();
	}

	pthread_mutex_lock(&m_lock);
	while (m_loops == 1) {
		m_waitState = 1;

		pthread_cond_wait(&m_cond_mess, &m_lock);
		m_waitState = 0;
	}	            		//end while

	if (m_recv)
		m_recv->disconnectServer(m_recv->context);
	m_isReceived = 0;
	pthread_mutex_unlock(&m_lock);
}
