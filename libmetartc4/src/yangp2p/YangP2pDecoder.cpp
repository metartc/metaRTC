//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "pthread.h"
#include <iostream>
#include <yangutil/yang_unistd.h>
#include <yangp2p/YangP2pDecoder.h>
#include "yangdecoder/YangDecoderFactory.h"

YangP2pDecoder::YangP2pDecoder(YangContext* pcontext) {
	m_context=pcontext;
	m_out_audioBuffer=NULL;
	m_out_videoBuffer=NULL;
	m_audioDec=NULL;
	m_videoDec=NULL;
}

YangP2pDecoder::~YangP2pDecoder() {
	if(m_audioDec) m_audioDec->stop();
	if(m_videoDec) m_videoDec->stop();
	yang_stop_thread(m_audioDec);
	yang_stop_thread(m_videoDec);

	yang_delete(m_audioDec);
	yang_delete(m_videoDec);
	size_t i=0;
	if(m_out_videoBuffer){
        for(i=0;i<m_out_videoBuffer->size();i++){
			delete m_out_videoBuffer->at(i);
		}
		m_out_videoBuffer->clear();
		yang_delete(m_out_videoBuffer);
	}
	if(m_out_audioBuffer){
        for(i=0;i<m_out_audioBuffer->size();i++){
			delete m_out_audioBuffer->at(i);
		}
		m_out_audioBuffer->clear();
		yang_delete(m_out_audioBuffer);
	}
}
void YangP2pDecoder::stopAll(){
	if(m_audioDec)	m_audioDec->stop();
	if(m_videoDec)	m_videoDec->stop();
}


void YangP2pDecoder::initAudioDecoder(){
	if(m_out_audioBuffer==NULL) m_out_audioBuffer=new vector<YangAudioPlayBuffer*>();
	if(m_audioDec==NULL) {
		m_audioDec=new YangAudioDecoderHandles(m_context);
		m_audioDec->init();
		m_audioDec->setOutAudioBuffer(m_out_audioBuffer);
	}
}

void YangP2pDecoder::initVideoDecoder(){
	if(m_out_videoBuffer==NULL) m_out_videoBuffer=new vector<YangVideoBuffer*>();

	if(m_videoDec==NULL) {
		m_videoDec=new YangVideoDecoderHandles(m_context);
		m_videoDec->init();
		m_videoDec->setOutVideoBuffer(m_out_videoBuffer);
	}

}

void YangP2pDecoder::startAudioDecoder(){
	if(m_audioDec&&!m_audioDec->m_isStart)		if(m_audioDec) m_audioDec->start();
}

void YangP2pDecoder::startVideoDecoder(){
		if(m_videoDec&&!m_videoDec->m_isStart) m_videoDec->start();
}

void YangP2pDecoder::setInVideoBuffer(YangVideoDecoderBuffer *pvel){
	if(m_videoDec!=NULL) m_videoDec->setInVideoBuffer(pvel);
}
void YangP2pDecoder::setInAudioBuffer(YangAudioEncoderBuffer *pael){
	if(m_audioDec!=NULL) m_audioDec->setInAudioBuffer(pael);
}
vector<YangVideoBuffer*>* YangP2pDecoder::getOutVideoBuffer(){
	return m_out_videoBuffer;
}
vector<YangAudioPlayBuffer*>*  YangP2pDecoder::getOutAudioBuffer(){
	return  m_out_audioBuffer;
}

