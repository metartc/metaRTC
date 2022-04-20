//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "malloc.h"
#include <yangutil/yang_unistd.h>
#include <yangdecoder/YangAudioDecoderHandle.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include "stdlib.h"

#include "yangdecoder/YangDecoderFactory.h"

YangAudioDecoderHandle::YangAudioDecoderHandle(YangContext *pcontext) {
	m_context=pcontext;
	m_context->streams.setDecoderMediaConfigCallback(this);
	m_isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_audioBuffer = NULL;
	m_decs = NULL;
	m_out_audioBuffer = NULL;
	m_buf=NULL;
	m_size=0;
	m_is44100=false;
	m_param = new YangAudioParam();
	m_param->encode = (YangAudioCodec) pcontext->avinfo.audio.audioDecoderType;
	m_param->sample = pcontext->avinfo.audio.sample;
	m_param->channel = pcontext->avinfo.audio.channel;

	m_is44100=(m_param->sample==44100);
	memset(&m_audioFrame,0,sizeof(YangFrame));
}


YangAudioDecoderHandle::~YangAudioDecoderHandle(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_context = NULL;
	m_in_audioBuffer = NULL;
	m_out_audioBuffer = NULL;

	if (m_decs) {

		delete m_decs;
		m_decs = NULL;

	}
	yang_deleteA(m_buf);

}

void YangAudioDecoderHandle::stop() {
	stopLoop();
}

void YangAudioDecoderHandle::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

YangAudioPlayBuffer* YangAudioDecoderHandle::getOutAudioBuffer() {

	return m_out_audioBuffer;
}

void YangAudioDecoderHandle::init() {
	m_isInit = 1;
}
void YangAudioDecoderHandle::setRemoteParam(YangAudioParam *para) {
	m_param->encode = para->encode;
	m_param->sample = para->sample;
	m_param->channel = para->channel;
	m_is44100=(m_param->sample==44100);

}

void YangAudioDecoderHandle::setMediaConfig(int32_t puid, YangAudioParam *para,
		YangVideoParam *video) {
	m_param->encode = para->encode;
	m_param->sample = para->sample;
	m_param->channel = para->channel;
	m_is44100=(m_param->sample==44100);

}
void YangAudioDecoderHandle::setInAudioBuffer(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangAudioDecoderHandle::setOutAudioBuffer(YangAudioPlayBuffer *pbuf) {
	m_out_audioBuffer = pbuf;
	if(m_context&&m_context->streams.m_playBuffer)  m_context->streams.m_playBuffer->setInAudioBuffer(pbuf);
}

void YangAudioDecoderHandle::onAudioData(YangFrame *pframe) {

	if (m_out_audioBuffer){
		if(m_is44100){
			if(m_buf==NULL) {
				m_buf=new uint8_t[1024*10];
				m_size=0;

			}
			memcpy(m_buf,pframe->payload,pframe->nb);
			m_size+=pframe->nb;
			int indexs=0;
			int len=882<<2;
			while(m_size>=len){
				pframe->payload=m_buf+indexs;
				pframe->nb=len;
				m_out_audioBuffer->putAudio(pframe);
				indexs+=len;
				m_size-=len;
			}
			if(indexs>0&&m_size>0) memmove(m_buf,m_buf+indexs,m_size);
		}else{

			m_out_audioBuffer->putAudio(pframe);
		}
	}

}
void YangAudioDecoderHandle::onVideoData(YangFrame *pframe) {

}
void YangAudioDecoderHandle::startLoop() {
	m_isConvert = 1;
	uint8_t srcAudioSource[600];
	int32_t audioCacheSize = m_context->avinfo.audio.audioPlayCacheNum;
	YangDecoderFactory ydf;
	YangFrame audioFrame;
	memset(&audioFrame,0,sizeof(YangFrame));
	while (m_isConvert == 1) {
		if (!m_in_audioBuffer) {
			yang_usleep(1000);
			continue;
		}
		if (m_in_audioBuffer->size() == 0) {
			yang_usleep(200);
			continue;
		}
		audioFrame.payload = srcAudioSource;
		m_in_audioBuffer->getPlayAudio(&audioFrame);

		if (!m_decs) {
			m_decs = ydf.createAudioDecoder(m_param);

			m_decs->m_uid = audioFrame.uid;
			m_decs->init();
		}

		if (m_decs){
			if(m_decs->decode(&audioFrame, this)){
				yang_error("decode audio fail..");
			}
		}

		if (m_in_audioBuffer && m_in_audioBuffer->size() > audioCacheSize)
			m_in_audioBuffer->resetIndex();

	}			//end

}

void YangAudioDecoderHandle::stopLoop() {
	m_isConvert = 0;

}
