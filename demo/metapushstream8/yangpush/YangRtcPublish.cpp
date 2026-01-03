//
// Copyright (c) 2019-2025 yanggaofeng
//
#include <yangpush/YangRtcPublish.h>
#include <yangstream/YangStreamType.h>
#include <yangstream/YangStreamCapture.h>

#include <yangutil/sys/YangLog.h>

#include <yangrtc/YangWhip.h>

#include <yangvideo/YangNalu.h>
#include <yangvideo/YangMeta.h>



YangRtcPublish::YangRtcPublish(YangContext *pcontext) {
	m_context = pcontext;

	m_in_videoBuffer = NULL;
	m_in_audioBuffer = NULL;
	m_isStart = 0;
	m_isConvert = 0;
	m_vmd = NULL;
	m_audioEncoderType = m_context->avinfo.audio.audioEncoderType;
	m_netState = 1;
	m_isInit = 0;
	isPublished = 0;

	m_transType=Yang_Webrtc;
	notifyState=0;


}

YangRtcPublish::~YangRtcPublish() {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_context = NULL;
	m_in_videoBuffer = NULL;
	m_in_audioBuffer = NULL;
	m_vmd = NULL;
}

void YangRtcPublish::setMediaConfig(int32_t  uid,YangAudioParam* audio,YangVideoParam* video){

}
void YangRtcPublish::sendRequest(int32_t  uid,uint32_t  ssrc,YangRequestType req){
    if(m_context->streams)
            m_context->streams->sendRequest(uid,ssrc,req);
}
int32_t YangRtcPublish::publishMsg(YangFrame* msgFrame){

    if(m_pushs.size()>0) {
        msgFrame->uid=m_pushs.front()->m_peer.peerInfo.uid;
    	return m_pushs.front()->on_message(msgFrame);
    }
	return 1;
}

int32_t YangRtcPublish::init(char* url,yangbool isWhip) {

    int32_t ret = 0;

    YangPeerInfo peerInfo;

    yang_avinfo_initPeerInfo(&peerInfo,&m_context->avinfo);
    peerInfo.uid=0;

    peerInfo.direction=YangSendonly;

    YangPeerConnection8* peer=new YangPeerConnection8(&peerInfo,NULL,NULL,this,NULL);
    peer->addAudioTrack(Yang_AED_OPUS);
    peer->addVideoTrack(Yang_VED_H264);
    peer->addTransceiver(YangMediaAudio,peerInfo.direction);
    peer->addTransceiver(YangMediaVideo,peerInfo.direction);

    m_pushs.push_back(peer);
    if(isWhip)
         ret = yang_whip_connectWhipWhepServer(&peer->m_peer,url);
    else
        ret = yang_whip_connectSfuServer(&peer->m_peer,url,m_context->avinfo.sys.mediaServer);

    if (ret)		return ret;

    yang_reindex(m_in_audioBuffer);
    yang_reindex(m_in_videoBuffer);
    return Yang_Ok;

}
int32_t YangRtcPublish::connectMediaServer() {
	if(m_pushs.size()>0) return Yang_Ok;

	return Yang_Ok;
}

int32_t YangRtcPublish::disConnectMediaServer() {
	if (m_pushs.size() > 0) {
		yang_delete(m_pushs.back());
		m_pushs.clear();
	}
	return Yang_Ok;
}

void YangRtcPublish::stop() {
	m_isConvert = 0;
}

void YangRtcPublish::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

void YangRtcPublish::setInAudioList(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangRtcPublish::setInVideoList(YangVideoEncoderBuffer *pbuf) {
	m_in_videoBuffer = pbuf;
}
void YangRtcPublish::setInVideoMetaData(YangVideoMeta *pvmd) {
	m_vmd = pvmd;
}

void YangRtcPublish::startLoop() {

    int32_t ret = Yang_Ok;
    YangPushData* pushData;
    YangAudioCodec audioCodec =	(YangAudioCodec) m_context->avinfo.audio.audioEncoderType;
    YangVideoCodec videoCodec =	(YangVideoCodec) m_context->avinfo.video.videoEncoderType;
    YangRtcPacer* pacer=new YangRtcPacer();
    YangFrame audioFrame;
    YangFrame videoFrame;
    YangPushData audioData,videoData;
    yang_memset(&audioFrame,0,sizeof(YangFrame));
    yang_memset(&videoFrame,0,sizeof(YangFrame));
    yang_memset(&audioData,0,sizeof(YangPushData));
    yang_memset(&videoData,0,sizeof(YangPushData));

    pacer->initAudio(audioCodec,m_context->avinfo.audio.sample,m_context->avinfo.audio.channel);
    pacer->initVideo(videoCodec,1024);

    m_isConvert = 1;
    isPublished = 1;
    notifyState=1;

    YangPeerConnection8* stream=NULL;
	while (m_isConvert == 1) {


		if ((m_in_videoBuffer && m_in_videoBuffer->size() == 0)
				&& (m_in_audioBuffer && m_in_audioBuffer->size() == 0)) {
			yang_usleep(2000);
			continue;
		}
		if (m_pushs.size() == 0) {
			yang_usleep(500);
			continue;
		}
		stream=m_pushs.back();

		if(stream->isConnected()){
			if(notifyState&&m_transType<Yang_Webrtc){
						if(m_context) m_context->streams->sendRequest(0,0,Yang_Req_Connected);
						notifyState=0;
					}

		}else{
			yang_usleep(500);
			continue;
		}

        if (m_in_audioBuffer && m_in_audioBuffer->size() > 0) {

                audioFrame.payload = m_in_audioBuffer->getAudioRef(&audioFrame);
                pushData=pacer->getAudioData(&audioFrame);

                ret = stream->on_audio(pushData);
                if (ret&&!stream->isConnected()) {
                    stream->close();
                }

            }

            if (m_in_videoBuffer && m_in_videoBuffer->size() > 0) {

                videoFrame.payload = m_in_videoBuffer->getEVideoRef(&videoFrame);

                pushData=pacer->getVideoData(&videoFrame);
                ret = stream->on_video(pushData);

                if (ret && !stream->isConnected()) {
                    stream->close();

                }


            }			//end
	}
	isPublished = 0;
    yang_delete(pacer);
}
