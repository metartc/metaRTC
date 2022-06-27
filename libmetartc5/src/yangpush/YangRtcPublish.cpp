//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangpush/YangRtcPublish.h>

#include <yangstream/YangStreamCapture.h>
#include <yangstream/YangStreamType.h>
#include <yangavutil/video/YangNalu.h>
#include <yangavutil/video/YangMeta.h>
#include <yangavutil/video/YangVideoEncoderMeta.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <stdio.h>
void g_pushstream_sendData(void* context,YangFrame* msgFrame){
	YangRtcPublish* push=(YangRtcPublish*)context;
	push->publishMsg(msgFrame);
}

void g_pushstream_receiveMsg(void* user,YangFrame *msgFrame){
	if(user==NULL) return;
	YangRtcPublish* rtcHandle=(YangRtcPublish*)user;
	rtcHandle->receiveMsg(msgFrame);

}

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

	m_context->channeldataSend.context=this;
    m_context->channeldataSend.sendData=g_pushstream_sendData;
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


int32_t YangRtcPublish::connectServer(int32_t puid){
	return m_pushs.back()->connectSfuServer(&m_pushs.back()->peer);
}



int32_t YangRtcPublish::reconnectMediaServer() {
	return m_pushs.back()->connectSfuServer(&m_pushs.back()->peer);
}

int32_t YangRtcPublish::publishMsg(YangFrame* msgFrame){

    if(m_pushs.size()>0) {
    	msgFrame->uid=m_pushs.front()->peer.streamconfig.uid;
    	return m_pushs.front()->on_message(&m_pushs.front()->peer,msgFrame);
    }
	return 1;
}

int32_t YangRtcPublish::receiveMsg(YangFrame* msgFrame){

	if(m_context->channeldataRecv.receiveData)
        m_context->channeldataRecv.receiveData(m_context->channeldataRecv.context,msgFrame);
	return Yang_Ok;
}

int32_t YangRtcPublish::init(int32_t nettype, char* server, int32_t pport,
		char* app,char* stream) {
	YangPeerConnection* sh=(YangPeerConnection*)calloc(sizeof(YangPeerConnection),1);
	int32_t ret = 0;
	strcpy(sh->peer.streamconfig.app,app);
	sh->peer.streamconfig.streamOptType=Yang_Stream_Publish;

	strcpy(sh->peer.streamconfig.remoteIp,server);
	sh->peer.streamconfig.remotePort=pport;

	strcpy(sh->peer.streamconfig.stream,stream);
	sh->peer.streamconfig.uid=0;

	sh->peer.streamconfig.localPort=m_context->avinfo.sys.rtcLocalPort;

	sh->peer.streamconfig.recvCallback.context=this;
	sh->peer.streamconfig.recvCallback.receiveMsg=g_pushstream_receiveMsg;

	memcpy(&sh->peer.streamconfig.rtcCallback,&m_context->rtcCallback,sizeof(YangRtcCallback));
	sh->peer.avinfo=&m_context->avinfo;
	yang_create_peerConnection(sh);
	sh->init(&sh->peer);
	m_pushs.push_back(sh);



	if (sh->isConnected(&sh->peer))		return Yang_Ok;

	ret = m_pushs.back()->connectSfuServer(&m_pushs.back()->peer);

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

		yang_destroy_peerConnection(m_pushs.back());
		yang_free(m_pushs.back());

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
	YangPeerConnection* stream=NULL;
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

		if(stream->isConnected(&stream->peer)){
			if(notifyState&&m_transType<Yang_Webrtc){
						if(m_context) m_context->streams.sendRequest(0,0,Yang_Req_Connected);
						notifyState=0;
					}

		}else{
			yang_usleep(500);
			continue;
		}

		if (m_in_audioBuffer && m_in_audioBuffer->size() > 0) {

			audioFrame.payload = m_in_audioBuffer->getAudioRef(&audioFrame);
			data.setAudioData(data.context,&audioFrame);

			//for (i = 0; i < m_pushs.size(); i++) {
                ret = stream->on_audio(&stream->peer,data.getAudioFrame(data.context));
				if (ret&&!stream->isConnected(&stream->peer)) {
					stream->close(&stream->peer);
					//yang_post_message(YangM_Sys_PushMediaServerError,m_pushs.back()->m_uid,NULL);
				}
			//}
		}

		if (m_in_videoBuffer && m_in_videoBuffer->size() > 0) {

			videoFrame.payload = m_in_videoBuffer->getEVideoRef(&videoFrame);

			if (videoFrame.frametype == YANG_Frametype_I) {

				if (m_vmd) {
					data.setVideoMeta(data.context,m_vmd->livingMeta.buffer,
							m_vmd->livingMeta.bufLen, videoType);
					//if (ret)	continue;
				} else {
					if (!vmd->isInit) {
						if (videoType == Yang_VED_264) {
							yang_createH264Meta(vmd, &videoFrame);
							yang_getConfig_Flv_H264(&vmd->mp4Meta,
									vmd->livingMeta.buffer,
									&vmd->livingMeta.bufLen);
						} else if (videoType == Yang_VED_265) {
							yang_createH265Meta(vmd, &videoFrame);
							yang_getConfig_Flv_H265(&vmd->mp4Meta,
									vmd->livingMeta.buffer,
									&vmd->livingMeta.bufLen);
						}
					}

					data.setVideoMeta(data.context,vmd->livingMeta.buffer,vmd->livingMeta.bufLen, videoType);


				}
				data.setVideoFrametype(data.context,YANG_Frametype_Spspps);
				data.setMetaTimestamp(data.context,videoFrame.pts);
                ret = stream->on_video(&stream->peer,data.getVideoFrame(data.context));



				if (!m_context->avinfo.enc.createMeta) {

					memset(&nalu, 0, sizeof(YangH264NaluData));
					if (videoType == Yang_VED_264)
						yang_parseH264Nalu(&videoFrame, &nalu);
					else
						yang_parseH265Nalu(&videoFrame, &nalu);


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
            ret = stream->on_video(&stream->peer,data.getVideoFrame(data.context));

			if (ret && !stream->isConnected(&stream->peer)) {
				stream->close(&stream->peer);

			}

		}			//end
	}
	isPublished = 0;
	yang_destroy_streamCapture(&data);
	yang_free(vmd);
}
