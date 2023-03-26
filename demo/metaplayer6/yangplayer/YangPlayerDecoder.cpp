//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangplayer/YangPlayerDecoder.h>
#include <yangdecoder/YangDecoderFactory.h>

YangPlayerDecoder::YangPlayerDecoder(YangContext* pcontext) {
	m_context=pcontext;
	m_out_audioBuffer=NULL;
	m_out_videoBuffer=NULL;
	m_audioDec=NULL;
	m_videoDec=NULL;

}

YangPlayerDecoder::~YangPlayerDecoder() {
	if(m_audioDec&&m_audioDec->m_isStart){
		m_audioDec->stop();
		while(m_audioDec->m_isStart){
								yang_usleep(1000);
							}
	}
	if(m_videoDec&&m_videoDec->m_isStart){
		m_videoDec->stop();
		while(m_videoDec->m_isStart){
								yang_usleep(1000);
							}
	}
	//yang_usleep(50000);
	yang_delete(m_audioDec);
	yang_delete(m_videoDec);
	//int32_t i=0;
	if(m_out_videoBuffer){
		delete m_out_videoBuffer;m_out_videoBuffer=NULL;
	}
	if(m_out_audioBuffer){
		delete m_out_audioBuffer;m_out_audioBuffer=NULL;
	}

}
void YangPlayerDecoder::stopAll(){
	if(m_audioDec)	m_audioDec->stop();
	if(m_videoDec)	m_videoDec->stop();
}


void YangPlayerDecoder::initAudioDecoder(){
	if(m_out_audioBuffer==NULL) {
		m_out_audioBuffer=new YangAudioPlayBuffer();

	}
	if(m_audioDec==NULL) {
	//	YangDecoderFactory df;
		//YangAudioParam audio={0};

		m_audioDec=new YangAudioDecoderHandle(m_context);
		m_audioDec->init();
		m_audioDec->setOutAudioBuffer(m_out_audioBuffer);
	}
}

void YangPlayerDecoder::initVideoDecoder(){
	//YangConfig *p_config = m_context;
	if(m_out_videoBuffer==NULL) {
		m_out_videoBuffer=new YangVideoBuffer(1);
		//m_out_videoBuffer->m_syn=&m_syn;
	}
	//m_context->videoBuffers=m_out_videoBuffer;
//	YangDecoderFactory df;
	if(m_videoDec==NULL) {

		m_videoDec=new YangVideoDecoderHandle(m_context);
		m_videoDec->init();
		m_videoDec->setOutVideoBuffer(m_out_videoBuffer);
	}

}

void YangPlayerDecoder::startAudioDecoder(){
	if(m_audioDec&&!m_audioDec->m_isStart)		if(m_audioDec) m_audioDec->start();
}

void YangPlayerDecoder::startVideoDecoder(){
		if(m_videoDec&&!m_videoDec->m_isStart) m_videoDec->start();
}

void YangPlayerDecoder::setInVideoBuffer(YangVideoDecoderBuffer *pvel){
	if(m_videoDec!=NULL) m_videoDec->setInVideoBuffer(pvel);
}
void YangPlayerDecoder::setInAudioBuffer(YangAudioEncoderBuffer *pael){
	if(m_audioDec!=NULL) m_audioDec->setInAudioBuffer(pael);
}
YangVideoBuffer* YangPlayerDecoder::getOutVideoBuffer(){
	return m_out_videoBuffer;
}
YangAudioPlayBuffer*  YangPlayerDecoder::getOutAudioBuffer(){
	return  m_out_audioBuffer;
}

