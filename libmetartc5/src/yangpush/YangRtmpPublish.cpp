//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangpush/YangRtmpPublish.h>

#include <yangstream/YangStreamCapture.h>
#include <yangstream/YangStreamType.h>
#include <yangavutil/video/YangNalu.h>
#include <yangavutil/video/YangMeta.h>
#include <yangavutil/video/YangVideoEncoderMeta.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <stdio.h>


YangRtmpPublish::YangRtmpPublish(YangContext *pcontext) {
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

YangRtmpPublish::~YangRtmpPublish() {
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


int32_t YangRtmpPublish::connectServer(int32_t puid){
	return m_pushs.back()->connectServer(m_pushs.back()->context);
}



int32_t YangRtmpPublish::reconnectMediaServer() {
	return m_pushs.back()->reconnect(m_pushs.back()->context);

}
int32_t YangRtmpPublish::publishMsg(YangFrame* msgFrame){

    if(m_pushs.size()>0) {
    	msgFrame->uid=m_pushs.front()->context->streamconfig.uid;
    	return m_pushs.front()->publishMsg(m_pushs.front()->context,msgFrame);
    }
	return 1;
}
int32_t YangRtmpPublish::receiveMsg(YangFrame* msgFrame){

	if(m_context->channeldataRecv.receiveData)
        m_context->channeldataRecv.receiveData(m_context->channeldataRecv.context,msgFrame);
	return Yang_Ok;
}
int32_t YangRtmpPublish::init(int32_t nettype, char* server, int32_t pport,
		char* app,char* stream) {

		int32_t ret = 0;
		YangStreamConfig streamConf;
		memset(&streamConf,0,sizeof(streamConf));

		strcpy(streamConf.app,app);
		streamConf.streamOptType=Yang_Stream_Publish;

		strcpy(streamConf.remoteIp,server);
		streamConf.remotePort=pport;

		strcpy(streamConf.stream,stream);
		streamConf.uid=0;

		if (m_pushs.size() == 0) {
				YangStreamHandle* sh=(YangStreamHandle*)calloc(sizeof(YangStreamHandle),1);
				yang_create_streamHandle(sh,&streamConf,&m_context->avinfo);
				m_pushs.push_back(sh);
			}


		if (m_pushs.back()->isconnected(m_pushs.back()->context))		return Yang_Ok;
		ret = m_pushs.back()->connectServer(m_pushs.back()->context);

		if (ret)		return ret;
		m_pushs.back()->context->streamInit = 1;
		yang_reindex(m_in_audioBuffer);
		yang_reindex(m_in_videoBuffer);
	return Yang_Ok;

}
int32_t YangRtmpPublish::connectMediaServer() {
	if(m_pushs.size()>0) return Yang_Ok;

	return Yang_Ok;
}
int32_t YangRtmpPublish::disConnectMediaServer() {
	if (m_pushs.size() > 0) {
		m_pushs.back()->context->streamInit = 0;
		yang_destroy_streamHandle(m_pushs.back());
		yang_free(m_pushs.back());

		m_pushs.clear();
	}
	return Yang_Ok;
}
void YangRtmpPublish::stop() {
	m_isConvert = 0;
}

void YangRtmpPublish::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

void YangRtmpPublish::setInAudioList(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangRtmpPublish::setInVideoList(YangVideoEncoderBuffer *pbuf) {
	m_in_videoBuffer = pbuf;
}
void YangRtmpPublish::setInVideoMetaData(YangVideoMeta *pvmd) {
	m_vmd = pvmd;
}

void YangRtmpPublish::startLoop() {

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

	uint8_t m_aacHeader[30]={0};
	m_aacHeader[20] = 0xaf;
	m_aacHeader[21] = 0x00;
	m_aacHeader[22] = 0x12;
	m_aacHeader[23] = 0x10;
	m_aacHeader[24] = 0x56;
	m_aacHeader[25] = 0xe5;
	m_aacHeader[26] = 0x00;
	bool isAac=(bool)(m_context->avinfo.audio.audioEncoderType==Yang_AED_AAC);
	bool hasSendAacheader=false;

	while (m_isConvert == 1) {


		if ((m_in_videoBuffer && m_in_videoBuffer->size() == 0)
				&& (m_in_audioBuffer && m_in_audioBuffer->size() == 0)) {
			yang_usleep(2000);
			continue;
		}
		if (m_pushs.size() == 0 || !m_pushs.back()->context->streamInit) {
			yang_usleep(500);
			continue;
		}
		YangStreamHandle* stream=m_pushs.back();

		if(stream->isconnected(stream->context)){
			if(notifyState&&m_transType<Yang_Webrtc){
						if(m_context) m_context->streams.sendRequest(0,0,Yang_Req_Connected);
						notifyState=0;
					}

		}else{
			yang_usleep(500);
			continue;
		}

		if (m_in_audioBuffer && m_in_audioBuffer->size() > 0) {
			if(isAac&&!hasSendAacheader){
				audioFrame.payload =m_aacHeader+20;
				audioFrame.nb=7;
				if(stream->publishAacheader) stream->publishAacheader(stream->context,&audioFrame);
				hasSendAacheader=true;
			}
			audioFrame.payload = m_in_audioBuffer->getAudioRef(&audioFrame);
			data.setAudioData(data.context,&audioFrame);


				ret = stream->publishAudioData(stream->context,&data);
				if (ret&&!stream->isconnected(stream->context)) {
					stream->context->streamInit = 0;
					stream->disConnectServer(stream->context);

				}

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
				ret = stream->publishVideoData(stream->context,&data);



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
			ret = stream->publishVideoData(stream->context,&data);

			if (ret && !stream->isconnected(stream->context)) {
				stream->context->streamInit = 0;
				stream->disConnectServer(stream->context);

			}

		}			//end
	}
	isPublished = 0;
	yang_destroy_streamCapture(&data);
	yang_free(vmd);
}
