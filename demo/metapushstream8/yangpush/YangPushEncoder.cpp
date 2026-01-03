//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangpush/YangPushEncoder.h>
#include <yangencoder/YangEncoderFactory.h>

YangPushEncoder::YangPushEncoder(YangContext *pcontext) {
	m_ae=NULL;
	m_ve=NULL;
	m_out_videoBuffer = NULL;
	m_out_auidoBuffer = NULL;

	m_context = pcontext;
	m_videoInfo=&pcontext->avinfo.video;
}

YangPushEncoder::~YangPushEncoder() {
	stopAll();
	yang_stop_thread(m_ae);
	yang_stop_thread(m_ve);

	yang_delete(m_ae);
	yang_delete(m_ve);

	yang_delete(m_out_videoBuffer);	//=NULL;
	yang_delete(m_out_auidoBuffer);	//=NULL;

	m_context = NULL;
}
void YangPushEncoder::stopAll() {
	yang_stop(m_ae);
	yang_stop(m_ve);

}

void YangPushEncoder::deleteVideoEncoder(){
	yang_stop(m_ve);
	yang_stop_thread(m_ve);
	yang_delete(m_ve);
}

void YangPushEncoder::setVideoInfo(YangVideoInfo* pvideo){
	if(pvideo) m_videoInfo=pvideo;
}
void YangPushEncoder::initAudioEncoder() {
	if (m_out_auidoBuffer == NULL)
		m_out_auidoBuffer = new YangAudioEncoderBuffer(m_context->avinfo.audio.audioCacheNum);
	if (m_ae == NULL) {
		//	YangEncoderFactory yf;
		m_ae = new YangAudioEncoderHandle(&m_context->avinfo.audio);
		m_ae->setOutAudioBuffer(m_out_auidoBuffer);
		m_ae->init();
	}

}
void YangPushEncoder::initVideoEncoder() {
	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoEncoderBuffer(m_context->avinfo.video.evideoCacheNum);

	if (m_ve == NULL) {
		//	YangEncoderFactory yf;
		m_ve = new YangVideoEncoderHandle(m_context,m_videoInfo);
		m_ve->setOutVideoBuffer(m_out_videoBuffer);
		m_ve->init();
	}
}
void YangPushEncoder::sendMsgToEncoder(YangRequestType req){
	if(m_ve) m_ve->sendMsgToEncoder(req);
}
void YangPushEncoder::startAudioEncoder() {
	if (m_ae && !m_ae->m_isStart) {
		m_ae->start();
		yang_usleep(1000);
	}
}
void YangPushEncoder::startVideoEncoder() {
	if (m_ve && !m_ve->m_isStart) {
		m_ve->start();
		yang_usleep(2000);
	}
}
void YangPushEncoder::setInAudioBuffer(YangAudioBuffer *pbuf) {
	if (m_ae != NULL)
		m_ae->setInAudioBuffer(pbuf);
}
void YangPushEncoder::setInVideoBuffer(YangVideoBuffer *pbuf) {
	if (m_ve != NULL)
		m_ve->setInVideoBuffer(pbuf);
}
YangAudioEncoderBuffer* YangPushEncoder::getOutAudioBuffer() {
	return m_out_auidoBuffer;
}
YangVideoEncoderBuffer* YangPushEncoder::getOutVideoBuffer() {
	return m_out_videoBuffer;
}

