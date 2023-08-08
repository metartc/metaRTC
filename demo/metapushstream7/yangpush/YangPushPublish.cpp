//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangpush/YangPushPublish.h>
#include <yangpush/YangPushCommon.h>


YangPushPublish::YangPushPublish(YangContext *pcontext) {
	m_context = pcontext;
	m_context->streams->setSendRequestCallback(this);
	m_videoInfo=&pcontext->avinfo.video;
	m_encoder = NULL;
	m_capture = NULL;
	m_outPreVideoBuffer=NULL;
	m_outVideoBuffer=NULL;
	isStartAudioCapture = 0, isStartVideoCapture = 0,isStartScreenCapture=0;
	isStartAudioEncoder = 0, isStartVideoEncoder = 0;
	m_captureType=Yang_VideoSrc_Camera;

}

YangPushPublish::~YangPushPublish() {
	stopAll();
	m_context = NULL;

	yang_delete(m_encoder);
	yang_delete(m_capture);
	yang_delete(m_outPreVideoBuffer);
	yang_delete(m_outVideoBuffer);
}
void YangPushPublish::sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req){
	if(req<Yang_Req_Connected) {
		sendMsgToEncoder(req);
	}
	if(req==Yang_Req_Connected) {


	}
}
void YangPushPublish::setCaptureType(int pct){
	m_captureType=pct;
}
void YangPushPublish::setVideoInfo(YangVideoInfo* pvideo){
	m_videoInfo=pvideo;
	if(m_captureType==Yang_VideoSrc_OutInterface) {
		if(m_outVideoBuffer==NULL) m_outVideoBuffer=new YangVideoBuffer(pvideo->width,pvideo->height,pvideo->videoEncoderFormat,m_context->avinfo.video.bitDepth==8?1:2);
		if(m_outPreVideoBuffer==NULL) m_outPreVideoBuffer=new YangVideoBuffer(pvideo->width,pvideo->height,pvideo->videoEncoderFormat,m_context->avinfo.video.bitDepth==8?1:2);
	}
}
void YangPushPublish::stopAll(){
	if(m_capture) m_capture->stopAll();
	if(m_encoder) m_encoder->stopAll();
}
void YangPushPublish::sendMsgToEncoder(YangRequestType req){
	if(m_encoder) m_encoder->sendMsgToEncoder(req);
}

YangPushCapture* YangPushPublish::getPushCapture(){
	return m_capture;
}

int32_t YangPushPublish::startAudioCapture() {
    int32_t err=Yang_Ok;
	if (isStartAudioCapture == 1)	return Yang_Ok;
	if (m_capture == NULL)		m_capture=new YangPushCapture(m_context);

	if((err=m_capture->initAudio(NULL))!=Yang_Ok) return yang_error_wrap(err,"init audioCapture fail");

	m_capture->startAudioCapture();
	isStartAudioCapture = 1;
	return err;

}
int32_t YangPushPublish::startVideoCapture() {
	if (isStartVideoCapture == 1)	return Yang_Ok;
	if (m_capture == NULL)		m_capture=new YangPushCapture(m_context);
	int32_t err=Yang_Ok;
	if((err=m_capture->initVideo())!=Yang_Ok) return yang_error_wrap(err,"init videoCapture fail");
	m_capture->startVideoCapture();
	isStartVideoCapture = 1;
	return err;
}


void YangPushPublish::setRtcNetBuffer(YangRtcPublish *prr){
	yang_reindex(m_encoder->getOutAudioBuffer());
	yang_reindex(m_encoder->getOutVideoBuffer());
	m_encoder->getOutVideoBuffer()->resetIndex();
	prr->setInAudioList(m_encoder->getOutAudioBuffer());
	prr->setInVideoList(m_encoder->getOutVideoBuffer());
	prr->setInVideoMetaData(m_encoder->getOutVideoMetaData());
}

void YangPushPublish::initAudioEncoding() {

	if (isStartAudioEncoder == 1)		return;
	if (m_encoder == NULL)
		m_encoder = new YangPushEncoder(m_context);
	m_encoder->initAudioEncoder();
	m_encoder->setInAudioBuffer(m_capture->getOutAudioBuffer());

	isStartAudioEncoder = 1;
}

void YangPushPublish::change(int32_t st){
	if(m_capture) m_capture->change(st);
}
void YangPushPublish::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_capture) m_capture->setInAudioBuffer(pbuf);
}
void YangPushPublish::initVideoEncoding() {
	if (isStartVideoEncoder == 1)	return;
	if (m_encoder == NULL)
		m_encoder = new YangPushEncoder(m_context);
	m_encoder->setVideoInfo(m_videoInfo);

	m_encoder->initVideoEncoder();
	if(m_captureType==Yang_VideoSrc_Camera){
		m_encoder->setInVideoBuffer(m_capture->getOutVideoBuffer());
	}else if(m_captureType==Yang_VideoSrc_Screen){
		m_encoder->setInVideoBuffer(m_capture->getScreenOutVideoBuffer());
	}else if(m_captureType==Yang_VideoSrc_OutInterface){
		m_encoder->setInVideoBuffer(m_outVideoBuffer);
	}
	isStartVideoEncoder = 1;
}
void YangPushPublish::startAudioEncoding() {
	if (m_encoder)
		m_encoder->startAudioEncoder();
}
void YangPushPublish::startVideoEncoding() {
	if (m_encoder)
		m_encoder->startVideoEncoder();
}
void YangPushPublish::deleteVideoEncoding(){
	if(m_encoder) m_encoder->deleteVideoEncoder();
	isStartVideoEncoder=0;

}
void YangPushPublish::startAudioCaptureState() {
	if (m_capture )
		m_capture->startAudioCaptureState();
}
YangVideoBuffer* YangPushPublish::getPreVideoBuffer(){
	if (m_capture )  return	m_capture->getPreVideoBuffer();
	return NULL;
}

void YangPushPublish::startVideoCaptureState() {
	if (m_capture )
		m_capture->startVideoCaptureState();
}

void YangPushPublish::stopAudioCaptureState() {
	if (m_capture )
		m_capture->stopAudioCaptureState();
}
void YangPushPublish::stopVideoCaptureState() {
	if (m_capture )
		m_capture->stopVideoCaptureState();
}
void YangPushPublish::stopScreenCaptureState(){
	if (m_capture )
				m_capture->stopScreenCaptureState();
}


YangVideoBuffer* YangPushPublish::getOutPreVideoBuffer(){
	return m_outPreVideoBuffer;
}
	YangVideoBuffer* YangPushPublish::getOutVideoBuffer(){
		 return	m_outVideoBuffer;

	}
void YangPushPublish::startCamera() {
	startVideoCapture();
}


void YangPushPublish::stopCamera() {
	if (m_capture ) m_capture->stopCamera();
}



void YangPushPublish::setScreenInterval(int32_t pinterval) {
	if(m_capture) m_capture->setScreenInterval(pinterval);
}

void YangPushPublish::setDrawmouse(bool isDraw) {
	if(m_capture) m_capture->setDrawmouse(isDraw);
}
