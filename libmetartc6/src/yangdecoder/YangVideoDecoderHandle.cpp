//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangdecoder/YangVideoDecoderHandle.h>
#include "malloc.h"
#include <yangutil/yang_unistd.h>

#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include "stdlib.h"

#include "yangdecoder/YangDecoderFactory.h"

YangVideoDecoderHandle::YangVideoDecoderHandle(YangContext *pcontext) {
	isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_videoBuffer = NULL;
	m_decs = NULL;
	m_out_videoBuffer = NULL;
	m_context = pcontext;
}

YangVideoDecoderHandle::~YangVideoDecoderHandle(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_context = NULL;
	m_in_videoBuffer = NULL;
	m_out_videoBuffer = NULL;

	if (m_decs) {
		delete m_decs;
		m_decs = NULL;

	}

}

void YangVideoDecoderHandle::stop() {
	stopLoop();
}

void YangVideoDecoderHandle::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}



YangVideoBuffer* YangVideoDecoderHandle::getOutVideoBuffer() {

	return m_out_videoBuffer;
}



void YangVideoDecoderHandle::addVideoStream(uint8_t *ps, int32_t puid,
		int32_t pisAdd) {    
	if (pisAdd) {
		YangVideoCodec aet = ps[0] == 0x17 ? Yang_VED_264 : Yang_VED_265;
		YangDecoderFactory df;
		m_decs=df.createVideoDecoder(aet, &m_context->avinfo.video);
		m_decs->m_uid = puid;
		m_decs->init();
		int32_t wid = 0, hei = 0, fps = 10;

		m_decs->parseRtmpHeader(ps, 128, &wid, &hei, &fps);
		if(m_out_videoBuffer){
			m_out_videoBuffer->m_uid = puid;
			m_out_videoBuffer->init(wid, hei, m_context->avinfo.video.videoDecoderFormat);
			m_out_videoBuffer->m_frames = fps;
			yang_trace("\ndecode:width==%d,height==%d,fps==%d\n",wid,hei,fps);
			yang_trace("\nvideoBuffer:width==%d,height==%d,fps==%d\n",m_out_videoBuffer->m_width,m_out_videoBuffer->m_height,fps);
		}


	}
}

void YangVideoDecoderHandle::init() {
	isInit = 1;
}

void YangVideoDecoderHandle::setInVideoBuffer(YangVideoDecoderBuffer *pbuf) {
	m_in_videoBuffer = pbuf;
}
void YangVideoDecoderHandle::setOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_out_videoBuffer = pbuf;
	if(m_context&&m_context->streams.m_playBuffer)  m_context->streams.m_playBuffer->setInVideoBuffer(pbuf);
}

void YangVideoDecoderHandle::onAudioData(YangFrame* pframe){

}

void YangVideoDecoderHandle::onVideoData(YangFrame* pframe){
	if(m_out_videoBuffer) {
		m_out_videoBuffer->putVideo(pframe);
	}
}


void YangVideoDecoderHandle::startLoop() {
	m_isConvert = 1;
	uint8_t *srcVideo = new uint8_t[YANG_VIDEO_ENCODE_BUFFER_LEN];
	uint8_t *temp;
	temp = srcVideo + 4;

	YangYuvType yuvType=YangI420;//YangYv12;
	YangFrame videoFrame;
	memset(&videoFrame,0,sizeof(YangFrame));
	int err=0;
	while (m_isConvert == 1) {
		if (!m_in_videoBuffer) {
			yang_usleep(2000);
			continue;
		}


		if (m_in_videoBuffer && m_in_videoBuffer->size() == 0) {
			yang_usleep(2000);
			continue;
		}


		videoFrame.payload=temp;
		if (m_in_videoBuffer)
			m_in_videoBuffer->getEVideo(&videoFrame);
		else
			continue;



		if (videoFrame.frametype == YANG_Frametype_Spspps) {
			if (m_decs == NULL) {
				addVideoStream(temp, videoFrame.uid, 1);
			}
			continue;
		}

		//compatible flash/flex h264 encode
		//		if (temp[0] == 0 && temp[1] == 0 && temp[2] == 0 && temp[3] == 0x02) {
		//			nalLen = temp[9];
		//			headLen = nalLen + 10 + 4;
		//		} else {
		//			headLen = 0;
		//		}

		if (m_decs && m_decs->m_state == 0) {
			if (!videoFrame.frametype)
				continue;
			m_decs->m_state = 1;
		}


		*(temp) = 0x00;
		*(temp +1) = 0x00;
		*(temp + 2) = 0x00;
		*(temp + 3) = 0x01;


		if (m_decs) {
			if((err=m_decs->decode(&videoFrame,yuvType,this))!=Yang_Ok){
				if(err==-1) continue;
				if(err>0){
					if(err==2&&m_context&&m_context->sendRtcMessage.sendRtcMessage)
								m_context->sendRtcMessage.sendRtcMessage(m_context->sendRtcMessage.context,0,YangRTC_Decoder_Error );
					yang_error("decode video fail..");
				}
			}

		}

	}

	temp = NULL;
	yang_deleteA(srcVideo);
}

void YangVideoDecoderHandle::stopLoop() {
	m_isConvert = 0;

}
