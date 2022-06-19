//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrecord//YangMp4FileApp.h>

#include <yangutil/yang_unistd.h>



YangMp4FileApp::YangMp4FileApp(YangContext* pcontext) {
	m_context=pcontext;
	m_audio=&pcontext->avinfo.audio;
	m_video=&pcontext->avinfo.video;
	m_encPara=&pcontext->avinfo.enc;
	m_enc=NULL;
	m_rec=NULL;
	m_isPause=0;
}

YangMp4FileApp::~YangMp4FileApp() {
	m_audio=NULL;
	m_video=NULL;
	m_enc=NULL;
	yang_delete(m_enc);
	yang_delete(m_rec);
}
void YangMp4FileApp::init(){

	if(!m_enc) {
		m_enc=new YangRecEncoder(m_context);
		m_enc->initAudioEncoder();
		m_enc->initVideoEncoder();

	}
	if(!m_rec) {
		m_rec=new YangRecordMp4(m_audio,m_video,m_encPara);
		m_rec->setInAudioBuffer(m_enc->getOutAudioBuffer());
		m_rec->setInVideoBuffer(m_enc->getOutVideoBuffer());
	}
}
void YangMp4FileApp::setFileTimeLen(int32_t ptlen_min){
	if(m_rec) m_rec->setFileTimeLen(ptlen_min);
}
void YangMp4FileApp::setInAudioBuffer(YangAudioBuffer *pbuf){
	if(m_enc) m_enc->setInAudioBuffer(pbuf);
}
	void YangMp4FileApp::setInVideoBuffer(YangVideoBuffer *pbuf){
		if(m_enc) m_enc->setInVideoBuffer(pbuf);
	}
	void YangMp4FileApp::pauseRecord(){
		if(m_rec) m_rec->pauseRec();
		m_isPause=1;
	}
	void YangMp4FileApp::resumeRecord(){
		if(!m_isPause) return;
		if(m_rec) m_rec->resumeRec();
		m_isPause=0;
	}
void YangMp4FileApp::startRecordMp4(char *filename0,int32_t p_module,int32_t p_isMp4)
{

	if(m_rec){
		m_rec->initPara(m_enc->getOutVideoMetaData(),filename0,1);
		m_rec->start();

	}
	if(m_enc) {
		m_enc->startAudioEncoder();
		m_enc->startVideoEncoder();
	}

}
void YangMp4FileApp::stopRecordMp4(){
	m_rec->stop();
	yang_sleep(1);
}
