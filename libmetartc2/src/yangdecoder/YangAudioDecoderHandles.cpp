#include "malloc.h"
#include <yangutil/yang_unistd.h>
#include <yangdecoder/YangAudioDecoderHandles.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavinfotype.h>
#include "stdlib.h"

#include "yangdecoder/YangDecoderFactory.h"

YangAudioDecoderHandles::YangAudioDecoderHandles(YangContext *pcontext) {
	m_isInit = 0;
	m_isStart = 0;
	m_isConvert = 1;
	m_in_audioBuffer = NULL;
	m_decs = new vector<YangAudioDecoder*>();
	m_out_audioBuffer = NULL;
	m_context = pcontext;
	m_frameSize=1024;
	m_channel=2;
}


YangAudioDecoderHandles::~YangAudioDecoderHandles(void) {
	if (m_isConvert) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	m_context = NULL;
	m_in_audioBuffer = NULL;

	size_t i = 0;
	if (m_out_audioBuffer && m_out_audioBuffer->size() > 0) {
		for (i = 0; i <  m_out_audioBuffer->size(); i++) 		{yang_delete( m_out_audioBuffer->at(i));	}

		m_out_audioBuffer->clear();

	}
	m_out_audioBuffer = NULL;
	if (m_decs) {
		for (i = 0; i < m_decs->size(); i++) {yang_delete( m_decs->at(i));	}

		m_decs->clear();
		delete m_decs;
		m_decs = NULL;

	}

    for(std::map<int, YangAudioParam*>::iterator it = m_paramMap.begin(); it != m_paramMap.end(); ++it) {
        	yang_delete(it->second);
    }
    m_paramMap.clear();

}
void YangAudioDecoderHandles::stop() {
	stopLoop();
}

void YangAudioDecoderHandles::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}

void YangAudioDecoderHandles::setRemoteParam(int32_t puid,YangAudioParam* para){

	map<int, YangAudioParam*>::iterator iter;
		iter=m_paramMap.find(puid);
		YangAudioParam* t_param=NULL;
		if(iter == m_paramMap.end()){
			t_param=new YangAudioParam();
			m_paramMap[puid]=t_param;
		}else{
			t_param=iter->second;
		}
		t_param->encode=para->encode;
		t_param->sample=para->sample;
		t_param->channel=para->channel;

}

int32_t YangAudioDecoderHandles::getDecoderIndex(int32_t puid) {
	for (int32_t i = 0; i < (int) m_decs->size(); i++) {
		if (m_decs->at(i)->m_uid == puid)
			return i;
	}
	return -1;
}
void YangAudioDecoderHandles::removeAllStream() {
	int32_t i = 0;
	if (m_decs && m_decs->size() > 0) {
		for (i = 0; i < (int) m_decs->size(); i++) {
			delete m_decs->at(i);
			m_decs->at(i) = NULL;
		}
		m_decs->clear();
	}
	if (m_out_audioBuffer && m_out_audioBuffer->size() > 0) {
		for (i = 0; i < (int) m_out_audioBuffer->size(); i++) {
			delete m_out_audioBuffer->at(i);
			m_out_audioBuffer->at(i) = NULL;
		}
		m_out_audioBuffer->clear();
	}

    for (map<int,YangAudioParam*>::iterator it = m_paramMap.begin(); it != m_paramMap.end();it++) {
        yang_delete(it->second);
    }
    m_paramMap.clear();
}
void YangAudioDecoderHandles::removeAudioStream(int32_t puid) {
	int32_t i = 0;
	if (m_decs && m_decs->size() > 0) {
		for (i = 0; i < (int) m_decs->size(); i++) {
			if (m_decs->at(i)->m_uid == puid) {
				delete m_decs->at(i);
				m_decs->at(i) = NULL;
				m_decs->erase(m_decs->begin() + i);
				break;
			}

		}
	}
	if (m_out_audioBuffer && m_out_audioBuffer->size() > 0) {
		for (i = 0; i < (int) m_out_audioBuffer->size(); i++) {
			if (m_out_audioBuffer->at(i)->m_uid == puid) {
				delete m_out_audioBuffer->at(i);
				m_out_audioBuffer->at(i) = NULL;
				m_out_audioBuffer->erase(m_out_audioBuffer->begin() + i);
				return;
			}
		}

	}
	map<int,YangAudioParam*>::iterator it = m_paramMap.find(puid);
	if(it!=m_paramMap.end()){
		yang_delete(it->second);
	}
	m_paramMap.erase(puid);
   // m_paramMap.clear();

}

YangAudioDecoder* YangAudioDecoderHandles::getDecoder(int32_t puid) {
	for (int32_t i = 0; i < (int) m_decs->size(); i++) {
		if (m_decs->at(i)->m_uid == puid)
			return m_decs->at(i);
	}
	return NULL;
}

YangAudioPlayBuffer* YangAudioDecoderHandles::getAudioBuffer(int32_t puid) {
	for (int32_t i = 0; i < (int) m_out_audioBuffer->size(); i++) {
		if (m_out_audioBuffer->at(i)->m_uid == puid)
			return m_out_audioBuffer->at(i);
	}
	return NULL;
}

void YangAudioDecoderHandles::init() {
	m_isInit = 1;
}

void YangAudioDecoderHandles::setInAudioBuffer(YangAudioEncoderBuffer *pbuf) {
	m_in_audioBuffer = pbuf;
}
void YangAudioDecoderHandles::setOutAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf) {
	m_out_audioBuffer = pbuf;
}
void YangAudioDecoderHandles::onAudioData(YangFrame* pframe){
	YangAudioPlayBuffer *t_vb = getAudioBuffer(pframe->uid);
	if (!t_vb) {
		m_out_audioBuffer->push_back(new YangAudioPlayBuffer());
		m_out_audioBuffer->back()->m_uid = pframe->uid;
		t_vb = m_out_audioBuffer->back();
		int ind=0;
		if(m_context&&(ind=m_context->streams.getIndex(pframe->uid))>-1){
			m_context->streams.m_playBuffers->at(ind)->setInAudioBuffer(t_vb);
			m_context->streams.m_playBuffers->at(ind)->setAudioClock(m_context->streams.getAudioClock());
		}
	}
	if (t_vb)		{
		t_vb->putAudio(pframe);
	}
	t_vb=NULL;
}
void YangAudioDecoderHandles::onVideoData(YangFrame* pframe){

}

void YangAudioDecoderHandles::startLoop() {
	m_isConvert = 1;
	uint8_t srcAudioSource[600];
	int32_t audioCacheSize = m_context->audio.audioPlayCacheNum;
	YangAudioDecoder *t_decoder = NULL;
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
		audioFrame.payload=srcAudioSource;
		m_in_audioBuffer->getPlayAudio(&audioFrame);
		t_decoder = getDecoder(audioFrame.uid);

		if (!t_decoder) {
			map<int, YangAudioParam*>::iterator iter;
			iter=m_paramMap.find(audioFrame.uid);
			if(iter != m_paramMap.end()){
				m_decs->push_back(ydf.createAudioDecoder(iter->second));
			}else{
				YangAudioParam* param=new YangAudioParam();
				m_paramMap[audioFrame.uid]=param;
				param->encode=(YangAudioEncDecType)m_context->audio.audioDecoderType;
				param->sample=m_context->audio.sample;
				param->channel=m_context->audio.channel;
				m_decs->push_back(ydf.createAudioDecoder(param));
			}

			t_decoder = m_decs->back();
			t_decoder->m_uid = audioFrame.uid;
			t_decoder->init();
		}

		if (t_decoder){
			if(t_decoder->decode(&audioFrame,this)){
				yang_error("decode audio fail..uid==%d",audioFrame.uid);
			}
		}

		if (m_in_audioBuffer && m_in_audioBuffer->size() > audioCacheSize)
			m_in_audioBuffer->resetIndex();
		t_decoder = NULL;
	}			//end

}

void YangAudioDecoderHandles::stopLoop() {
	m_isConvert = 0;

}
