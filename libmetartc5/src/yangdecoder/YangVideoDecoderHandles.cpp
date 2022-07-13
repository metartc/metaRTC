//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangdecoder/YangVideoDecoderHandles.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include "malloc.h"
#include <yangutil/yang_unistd.h>

#include "stdlib.h"

#include "yangdecoder/YangDecoderFactory.h"

YangVideoDecoderHandles::YangVideoDecoderHandles(YangContext *pcontext) {
	isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_videoBuffer = NULL;
	m_decs = new vector<YangVideoDecoder*>();
	m_out_videoBuffer = NULL;
	m_context = pcontext;

	pthread_mutex_init(&m_mutex, NULL);
}


YangVideoDecoderHandles::~YangVideoDecoderHandles(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_context = NULL;
	m_in_videoBuffer = NULL;
	m_out_videoBuffer = NULL;
	size_t i = 0;
	if (m_decs) {
		for (i = 0; i < m_decs->size(); i++) {
			delete m_decs->at(i);
		}
		m_decs->clear();
		delete m_decs;
		m_decs = NULL;

	}
	pthread_mutex_destroy(&m_mutex);

}

void YangVideoDecoderHandles::stop() {
	stopLoop();
}

void YangVideoDecoderHandles::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

int32_t YangVideoDecoderHandles::getDecoderIndex(int32_t puid) {
	for (int32_t i = 0; i < (int) m_decs->size(); i++) {
		if (m_decs->at(i)->m_uid == puid)
			return i;
	}
	return -1;
}
YangVideoDecoder* YangVideoDecoderHandles::getDecoder(int32_t puid) {
	for (int32_t i = 0; i < (int) m_decs->size(); i++) {
		if (m_decs->at(i)->m_uid == puid)
			return m_decs->at(i);
	}
	return NULL;
}

YangVideoBuffer* YangVideoDecoderHandles::getVideoBuffer(int32_t puid) {
	for (int32_t i = 0; i < (int) m_out_videoBuffer->size(); i++) {
		if (m_out_videoBuffer->at(i)->m_uid == puid)
			return m_out_videoBuffer->at(i);
	}
	return NULL;
}

void YangVideoDecoderHandles::removeAllStream() {
	if (m_out_videoBuffer->size() == 0)
		return;
	int32_t i = 0;
	for (i = 0; i < (int) m_out_videoBuffer->size(); i++) {
		delete m_out_videoBuffer->at(i);
		m_out_videoBuffer->at(i) = NULL;

	}
	m_out_videoBuffer->clear();
	if (m_decs->size() == 0)
		return;
	for (i = 0; i < (int) m_decs->size(); i++) {
		delete m_decs->at(i);
		m_decs->at(i) = NULL;
	}
	m_decs->clear();
}


void YangVideoDecoderHandles::removeStream(){
	int32_t i = 0;
	for(;m_removeList.size()>0;){
                int32_t puid=m_removeList.front();
                m_removeList.erase(m_removeList.begin());

			for (i = 0; i < (int) m_out_videoBuffer->size(); i++) {
				if (m_out_videoBuffer->at(i)->m_uid == puid) {
					delete m_out_videoBuffer->at(i);
					m_out_videoBuffer->at(i) = NULL;
					m_out_videoBuffer->erase(m_out_videoBuffer->begin() + i);
					break;
				}
			}
			if (m_decs->size() == 0)
				return;
			for (i = 0; i < (int) m_decs->size(); i++) {
				if (m_decs->at(i)->m_uid == puid) {
					delete m_decs->at(i);
					m_decs->at(i) = NULL;
					m_decs->erase(m_decs->begin() + i);
					return;
				}
			}
	}
}

void YangVideoDecoderHandles::addVideoStream(uint8_t *ps, int32_t puid,
		int32_t pisAdd) {
	if (pisAdd) {
		YangVideoCodec aet = ps[0] == 0x17 ? Yang_VED_264 : Yang_VED_265;
		YangDecoderFactory df;
		m_decs->push_back(df.createVideoDecoder(aet, &m_context->avinfo.video));
		m_decs->back()->m_uid = puid;
		m_decs->back()->init();
		int32_t wid = 0, hei = 0, fps = 10;

		m_decs->back()->parseRtmpHeader(ps, 128, &wid, &hei, &fps);
		m_out_videoBuffer->push_back(new YangVideoBuffer(m_context->avinfo.video.bitDepth == 8 ? 1 : 2));
		m_out_videoBuffer->back()->m_uid = puid;
		m_out_videoBuffer->back()->init(wid, hei, m_context->avinfo.video.videoDecoderFormat);
		m_out_videoBuffer->back()->m_frames = fps;
		yang_trace("\nwidth==%d,height==%d,fps==%d\n",wid,hei,fps);

		int ind=0;
		if(m_context&&(ind=m_context->streams.getIndex(puid))>-1){
			m_context->streams.m_playBuffers->at(ind)->setInVideoBuffer(m_out_videoBuffer->back());
		}

	} else {

		pthread_mutex_lock(&m_mutex);
		m_removeList.push_back(puid);
		pthread_mutex_unlock(&m_mutex);
	}
}

void YangVideoDecoderHandles::init() {
	isInit = 1;
}

void YangVideoDecoderHandles::setInVideoBuffer(YangVideoDecoderBuffer *pbuf) {
	m_in_videoBuffer = pbuf;
}
void YangVideoDecoderHandles::setOutVideoBuffer(vector<YangVideoBuffer*> *pbuf) {
	m_out_videoBuffer = pbuf;
}
void YangVideoDecoderHandles::onAudioData(YangFrame* pframe){

}
void YangVideoDecoderHandles::onVideoData(YangFrame* pframe){

	YangVideoBuffer *t_vb = getVideoBuffer(pframe->uid);
	if(t_vb) {
		t_vb->putVideo(pframe);
	}
	t_vb=NULL;
}


void YangVideoDecoderHandles::startLoop() {
	m_isConvert = 1;
	uint8_t *srcVideo = new uint8_t[YANG_VIDEO_ENCODE_BUFFER_LEN];
	uint8_t *temp;
	temp = srcVideo + 4;


	int32_t index = 0;
	YangVideoDecoder *t_decoder = NULL;
	int32_t headLen = 0;

	YangYuvType yuvType=YangI420;//YangYv12;
	YangFrame videoFrame;
	memset(&videoFrame,0,sizeof(YangFrame));

	int err=Yang_Ok;

	while (m_isConvert == 1) {
		if (!m_in_videoBuffer) {
			yang_usleep(2000);
			continue;
		}

		if(m_removeList.size()>0) {
					pthread_mutex_lock(&m_mutex);
					removeStream();
					pthread_mutex_unlock(&m_mutex);
		}

		if (m_in_videoBuffer && m_in_videoBuffer->size() == 0) {
			yang_usleep(2000);
			continue;
		}

		videoFrame.payload=temp;

		if (m_in_videoBuffer) m_in_videoBuffer->getEVideo(&videoFrame);

		if (videoFrame.frametype == YANG_Frametype_Spspps) {
			index = getDecoderIndex(videoFrame.uid);
			if (index == -1) {
				addVideoStream(temp, videoFrame.uid, 1);
			}
			continue;
		}
		//compatible flash/flex h264 encode
//		if (temp[0] == 0 && temp[1] == 0 && temp[2] == 0 && temp[3] == 0x02) {
//			nalLen = temp[9];
//			headLen = nalLen + 10 + 4;
//		} else {
//			headLen = 4;
//		}


		t_decoder = getDecoder(videoFrame.uid);

		if (t_decoder && t_decoder->m_state == 0) {
			if (!videoFrame.frametype)
				continue;
			t_decoder->m_state = 1;
		}

        //if (isFFmpeg) {
        //	headLen -= 4;
			*(temp + headLen) = 0x00;
			*(temp + headLen + 1) = 0x00;
			*(temp + headLen + 2) = 0x00;
			*(temp + headLen + 3) = 0x01;
        //}
		if (t_decoder) {
			videoFrame.payload=temp + headLen;
			videoFrame.nb=videoFrame.nb - headLen;
			if((err=t_decoder->decode(&videoFrame,yuvType,this))!=Yang_Ok){
				if(err==-1) continue;
				if(err==2&&m_context&&m_context->sendRtcMessage.sendRtcMessage)
					m_context->sendRtcMessage.sendRtcMessage(m_context->sendRtcMessage.context,videoFrame.uid,YangRTC_Decoder_Error);
                yang_error("decode video fail(%d)..uid===%d",err,videoFrame.uid);
			}

		}
		t_decoder = NULL;
	}
	t_decoder = NULL;
	temp = NULL;
	yang_deleteA(srcVideo);
}

void YangVideoDecoderHandles::stopLoop() {
	m_isConvert = 0;

}
