#include "YangRtcReceive.h"

#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>

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
		m_recv->disconnectServer();
	stop();

	yang_delete(m_recv);
}
void YangRtcReceive::setBuffer(YangAudioEncoderBuffer *al,
		YangVideoDecoderBuffer *vl) {
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
	uint8_t *temp = videoFrame->payload;
    int videoLen=videoFrame->nb;

    if(temp[0] == 0x27&&temp[1] == 0x01){
        videoFrame->payload = temp + 5;
        videoFrame->nb -= 5;
        videoFrame->frametype = YANG_Frametype_P;
        if(yang_hasH264Pframe(videoFrame->payload)) m_out_videoBuffer->putEVideo(videoFrame);
        return;
    }

    if (temp[0] == 0x17) {
    	if(temp[1] == 0x00){
			videoFrame->frametype = YANG_Frametype_Spspps;
			m_out_videoBuffer->putEVideo(videoFrame);
			return;
    	}
    	if(temp[1] == 0x01){
            videoFrame->payload=temp+5;
            videoFrame->nb=videoLen-5;

            yang_parseH264Nalu(videoFrame,&m_nalu);

            if(m_nalu.spsppsPos>-1){
                uint8_t meta[200] = { 0 };
                videoFrame->payload=meta;
                yang_getH264SpsppseNalu(videoFrame,temp+5+m_nalu.spsppsPos);
                videoFrame->frametype = YANG_Frametype_Spspps;
                m_out_videoBuffer->putEVideo(videoFrame);
            }
            if(m_nalu.keyframePos>-1){
                videoFrame->payload = temp + 5+m_nalu.keyframePos;
                videoFrame->nb = videoLen-5-m_nalu.keyframePos;
                videoFrame->frametype =YANG_Frametype_I;
                m_out_videoBuffer->putEVideo(videoFrame);

            }
    	}
    }

    if( temp[0] == 0x2c&&temp[1] == 0x01){
        videoFrame->payload = temp + 5;
        videoFrame->nb -= 5;
        videoFrame->frametype = YANG_Frametype_P;
         m_out_videoBuffer->putEVideo(videoFrame);
        return;
    }
    if ( temp[0] == 0x1c ) {
        	if(temp[1] == 0x00){
    			videoFrame->frametype = YANG_Frametype_Spspps;
    			m_out_videoBuffer->putEVideo(videoFrame);
    			return;
        	}
        	if(temp[1] == 0x01){
                videoFrame->payload=temp+5;
                videoFrame->nb=videoLen-5;

                yang_parseH265Nalu(videoFrame,&m_nalu);

                if(m_nalu.spsppsPos>-1){
                    uint8_t meta[200] = { 0 };
                    videoFrame->payload=meta;
                    yang_getH265SpsppseNalu(videoFrame,temp+5+m_nalu.spsppsPos);
                    videoFrame->frametype = YANG_Frametype_Spspps;
                    m_out_videoBuffer->putEVideo(videoFrame);
                }
                if(m_nalu.keyframePos>-1){
                    videoFrame->payload = temp + 5+m_nalu.keyframePos;
                    videoFrame->nb = videoLen-5-m_nalu.keyframePos;
                    videoFrame->frametype =YANG_Frametype_I;
                    m_out_videoBuffer->putEVideo(videoFrame);

                }
        	}
        }

}

int32_t YangRtcReceive::init(int32_t puid, string localIp, int32_t localPort,
		string server, int32_t pport, string app, string stream) {
	m_conf.localIp = localIp;
	m_conf.localPort = localPort;
	m_conf.serverIp = server;
	m_conf.serverPort = pport;
	m_conf.app = app;
	m_conf.stream = stream;
	m_conf.uid = puid;
	m_conf.streamOptType = Yang_Stream_Play;
	if (!m_recv)
		m_recv = YangRtcHandle::createRtcHandle(m_context);
	m_recv->setReceiveCallback(this);

	//m_recv->setMediaParam(NULL, NULL, &m_rtcinfo);

	m_recv->init(&m_conf);
	return Yang_Ok;

}
void YangRtcReceive::stop() {
	m_loops = 0;
	if (m_recv)
		m_recv->disconnectServer();
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
	if ((err=m_recv->connectRtcServer())!=Yang_Ok) {
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
		m_recv->disconnectServer();
	m_isReceived = 0;
	pthread_mutex_unlock(&m_lock);
}
