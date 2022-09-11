//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangrecord/YangRecordHandle.h>
#include <yangutil/yang_unistd.h>

YangRecordHandle::YangRecordHandle(YangContext* pcontext) {
	m_context=pcontext;
	m_context->avinfo.video.videoCacheNum*=2;
	m_context->avinfo.audio.audioCacheNum*=2;
	m_cap=NULL;
	m_rec=NULL;

}

YangRecordHandle::~YangRecordHandle() {
	m_cap=NULL;

	yang_delete(m_rec);
	m_context=NULL;
}


void YangRecordHandle::init(YangPushCapture* pcap){
	m_cap=pcap;
	if(!m_rec){
		m_rec=new YangMp4FileApp(m_context);
		m_rec->init();
		m_rec->setInAudioBuffer(m_cap->getOutAudioBuffer());
		m_rec->setInVideoBuffer(m_cap->getOutVideoBuffer());
		//m_rec->setFileTimeLen(1);
	}
}
/**
void YangRecordHandle::pauseRecord(){
	if(m_cap)	m_cap->startPauseCaptureState();
	if(m_rec) m_rec->pauseRecord();
}
void YangRecordHandle::resumeRecord(){
	if(m_rec) m_rec->resumeRecord();
	if(m_cap) m_cap->stopPauseCaptureState();

}

**/
void YangRecordHandle::recordFile(char* filename){
	m_rec->startRecordMp4(filename,1,1);
	yang_sleep(1);
	//m_cap->startVideoCaptureState();
	//m_cap->startAudioCaptureState();
}
void YangRecordHandle::stopRecord(){
	m_cap->stopAudioCaptureState();
	m_cap->stopVideoCaptureState();
//	m_cap->stopAll();
	m_rec->m_enc->m_ae->stop();
	m_rec->m_enc->m_ve->stop();

	yang_sleep(1);

	m_rec->stopRecordMp4();
}
