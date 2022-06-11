//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangp2p/YangP2pPublish.h>
#include <yangpush/YangPushCommon.h>
#include <yangp2p/YangP2pFactory.h>

YangP2pPublish::YangP2pPublish(YangContext *pcontext) {
	m_context = pcontext;
	if(m_context->streams.m_playBuffers==NULL) m_context->streams.m_playBuffers=new vector<YangSynBuffer*>();;
	m_context->streams.setSendRequestCallback(this);
	m_videoInfo=&pcontext->avinfo.video;
	m_encoder = NULL;
	m_capture = NULL;
	m_outPreVideoBuffer=NULL;
	m_outVideoBuffer=NULL;
    isStartAudioCapture = 0, isStartVideoCapture =0;
	isStartAudioEncoder = 0, isStartVideoEncoder = 0;
	m_captureType=Yang_VideoSrc_Camera;
}

YangP2pPublish::~YangP2pPublish() {
	stopAll();
	m_context = NULL;

	yang_delete(m_encoder);
	yang_delete(m_capture);
	yang_delete(m_outPreVideoBuffer);
	yang_delete(m_outVideoBuffer);
}
void YangP2pPublish::sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req){
	if(req<Yang_Req_Connected) {
		sendMsgToEncoder(req);
	}
	if(req==Yang_Req_Connected) {


	}
}
void YangP2pPublish::setCaptureType(int pct){
	m_captureType=pct;
}
void YangP2pPublish::setVideoInfo(YangVideoInfo* pvideo){
	m_videoInfo=pvideo;

}
void YangP2pPublish::stopAll(){
	if(m_capture) m_capture->stopAll();
	if(m_encoder) m_encoder->stopAll();
}
void YangP2pPublish::sendMsgToEncoder(YangRequestType req){
	if(m_encoder) m_encoder->sendMsgToEncoder(req);
}

YangP2pCapture* YangP2pPublish::getPushCapture(){
	return m_capture;
}

int32_t YangP2pPublish::startAudioCapture() {
	if (isStartAudioCapture == 1)	return Yang_Ok;

	if (m_capture == NULL)		{
		YangP2pFactory pf;
		m_capture=(YangP2pCapture*)pf.getP2pCapture(m_captureType, m_context) ;
	}
	int32_t err=Yang_Ok;
	if((err=m_capture->initAudio(NULL))!=Yang_Ok) return yang_error_wrap(err,"init audioCapture fail");

	m_capture->startAudioCapture();
	isStartAudioCapture = 1;
	return err;

}
int32_t YangP2pPublish::startVideoCapture() {
	if (isStartVideoCapture == 1)	return Yang_Ok;
	if (m_capture == NULL)		{
		YangP2pFactory pf;
		m_capture=(YangP2pCapture*)pf.getP2pCapture(m_captureType, m_context) ;
	}
	int32_t err=Yang_Ok;
	if((err=m_capture->initVideo())!=Yang_Ok) return yang_error_wrap(err,"init videoCapture fail");
	m_capture->startVideoCapture();
	isStartVideoCapture = 1;
	return err;
}


void YangP2pPublish::setNetBuffer(YangP2pRtc *prr){
	yang_reindex(m_encoder->getOutAudioBuffer());
	yang_reindex(m_encoder->getOutVideoBuffer());
	m_encoder->getOutVideoBuffer()->resetIndex();
	prr->setInAudioList(m_encoder->getOutAudioBuffer());
	prr->setInVideoList(m_encoder->getOutVideoBuffer());
	prr->setInVideoMetaData(m_encoder->getOutVideoMetaData());
}
void YangP2pPublish::initAudioEncoding() {

	if (isStartAudioEncoder == 1)		return;
	if (m_encoder == NULL)
		m_encoder = new YangPushEncoder(m_context);
	m_encoder->initAudioEncoder();
	m_encoder->setInAudioBuffer(m_capture->getOutAudioBuffer());

	isStartAudioEncoder = 1;
}

void YangP2pPublish::change(int32_t st){
	if(m_capture) m_capture->change(st);
}
void YangP2pPublish::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_capture) m_capture->setInAudioBuffer(pbuf);
}
void YangP2pPublish::initVideoEncoding() {
	if (isStartVideoEncoder == 1)	return;
	if (m_encoder == NULL)
		m_encoder = new YangPushEncoder(m_context);
	m_encoder->setVideoInfo(m_videoInfo);

	m_encoder->initVideoEncoder();

	m_encoder->setInVideoBuffer(m_capture->getOutVideoBuffer());

	isStartVideoEncoder = 1;
}
void YangP2pPublish::startAudioEncoding() {
	if (m_encoder)
		m_encoder->startAudioEncoder();
}
void YangP2pPublish::startVideoEncoding() {
	if (m_encoder)
		m_encoder->startVideoEncoder();
}
void YangP2pPublish::deleteVideoEncoding(){
	if(m_encoder) m_encoder->deleteVideoEncoder();
	isStartVideoEncoder=0;

}
void YangP2pPublish::startAudioCaptureState() {
	if (m_capture )
		m_capture->startAudioCaptureState();
}
YangVideoBuffer* YangP2pPublish::getPreVideoBuffer(){
	if (m_capture )  return	m_capture->getPreVideoBuffer();
	return NULL;
}

void YangP2pPublish::startVideoCaptureState() {
	if (m_capture )
		m_capture->startVideoCaptureState();
}

void YangP2pPublish::stopAudioCaptureState() {
	if (m_capture )
		m_capture->stopAudioCaptureState();
}

void YangP2pPublish::stopVideoCaptureState() {
	if (m_capture )
		m_capture->stopVideoCaptureState();
}


YangVideoBuffer* YangP2pPublish::getOutPreVideoBuffer(){
	return m_outPreVideoBuffer;
}

YangVideoBuffer* YangP2pPublish::getOutVideoBuffer(){
	return	m_outVideoBuffer;

}

void YangP2pPublish::startCamera() {
	startVideoCapture();
}


void YangP2pPublish::stopCamera() {
	if (m_capture ) m_capture->stopVideoSource();
}


