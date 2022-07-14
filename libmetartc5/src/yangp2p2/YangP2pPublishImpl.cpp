//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangp2p/YangP2pFactoryImpl.h>
#include <yangp2p2/YangP2pPublishImpl.h>
#include <yangpush/YangPushCommon.h>

YangP2pPublishImpl::YangP2pPublishImpl(YangContext *pcontext) {
	m_context = pcontext;
	m_hasAudio=true;
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
	setCaptureType(Yang_VideoSrc_Camera);
}

YangP2pPublishImpl::~YangP2pPublishImpl() {
	stopAll();
	m_context = NULL;

	yang_delete(m_encoder);
	yang_delete(m_capture);
	yang_delete(m_outPreVideoBuffer);
	yang_delete(m_outVideoBuffer);
}


void YangP2pPublishImpl::initCodec(bool phasAudio){
	m_hasAudio=phasAudio;
	if(m_hasAudio) {
        m_hasAudio=bool(startAudioCapture()==Yang_Ok);
	}
	if (m_hasAudio) {

		initAudioEncoding();
	}

	initVideoEncoding();
}
void YangP2pPublishImpl::startCocdec(){
	if (m_hasAudio) 	startAudioEncoding();
	startVideoEncoding();
}

void YangP2pPublishImpl::startCaptureState(){
	if (m_hasAudio)				startAudioCaptureState();
	startVideoCaptureState();
}


void YangP2pPublishImpl::sendRequest(int32_t puid,uint32_t ssrc,YangRequestType req){
	if(req<Yang_Req_Connected) {
		sendMsgToEncoder(req);
	}
	if(req==Yang_Req_Connected) {


	}
}
void YangP2pPublishImpl::setCaptureType(int pct){
	m_captureType=pct;
}
void YangP2pPublishImpl::setVideoInfo(YangVideoInfo* pvideo){
	setCaptureType(Yang_VideoSrc_Camera);
	m_videoInfo=pvideo;

}
void YangP2pPublishImpl::stopAll(){
	if(m_capture) m_capture->stopAll();
	if(m_encoder) m_encoder->stopAll();
}
void YangP2pPublishImpl::sendMsgToEncoder(YangRequestType req){
	if(m_encoder) m_encoder->sendMsgToEncoder(req);
}

void* YangP2pPublishImpl::getPushCapture(){
	return m_capture;
}

int32_t YangP2pPublishImpl::startAudioCapture() {
	if (isStartAudioCapture == 1)	return Yang_Ok;

	if (m_capture == NULL)		{
        YangP2pFactoryImpl pf;
		m_capture=(YangP2pCapture*)pf.getP2pCapture(m_captureType, m_context) ;
	}
	int32_t err=Yang_Ok;
	if((err=m_capture->initAudio(NULL))!=Yang_Ok) return yang_error_wrap(err,"init audioCapture fail");

	m_capture->startAudioCapture();
	isStartAudioCapture = 1;
	return err;

}
int32_t YangP2pPublishImpl::startVideoCapture() {
	if (isStartVideoCapture == 1)	return Yang_Ok;
	if (m_capture == NULL)		{
        YangP2pFactoryImpl pf;
		m_capture=(YangP2pCapture*)pf.getP2pCapture(m_captureType, m_context) ;
	}
	int32_t err=Yang_Ok;
	if((err=m_capture->initVideo())!=Yang_Ok) return yang_error_wrap(err,"init videoCapture fail");
	m_capture->startVideoCapture();
	isStartVideoCapture = 1;
	return err;
}

void YangP2pPublishImpl::resetList(){
	yang_reindex(m_encoder->getOutAudioBuffer());
		yang_reindex(m_encoder->getOutVideoBuffer());
		m_encoder->getOutVideoBuffer()->resetIndex();
}
void YangP2pPublishImpl::setNetBuffer(YangP2pRtcBuffer *prr){
	yang_reindex(m_encoder->getOutAudioBuffer());
	yang_reindex(m_encoder->getOutVideoBuffer());
	m_encoder->getOutVideoBuffer()->resetIndex();
	prr->setInAudioList(m_encoder->getOutAudioBuffer());
	prr->setInVideoList(m_encoder->getOutVideoBuffer());
	prr->setInVideoMetaData(m_encoder->getOutVideoMetaData());
}
void YangP2pPublishImpl::initAudioEncoding() {

	if (isStartAudioEncoder == 1)		return;
	if (m_encoder == NULL)
		m_encoder = new YangPushEncoder(m_context);
	m_encoder->initAudioEncoder();
	m_encoder->setInAudioBuffer(m_capture->getOutAudioBuffer());

	isStartAudioEncoder = 1;
}

void YangP2pPublishImpl::change(int32_t st){
	if(m_capture) m_capture->change(st);
}
void YangP2pPublishImpl::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pbuf){
	if(m_capture) m_capture->setInAudioBuffer(pbuf);
}
void YangP2pPublishImpl::initVideoEncoding() {
	if (isStartVideoEncoder == 1)	return;
	if (m_encoder == NULL)
		m_encoder = new YangPushEncoder(m_context);
	m_encoder->setVideoInfo(m_videoInfo);

	m_encoder->initVideoEncoder();

	m_encoder->setInVideoBuffer(m_capture->getOutVideoBuffer());

	isStartVideoEncoder = 1;
}
void YangP2pPublishImpl::startAudioEncoding() {
	if (m_encoder)
		m_encoder->startAudioEncoder();
}
void YangP2pPublishImpl::startVideoEncoding() {
	if (m_encoder)
		m_encoder->startVideoEncoder();
}
void YangP2pPublishImpl::deleteVideoEncoding(){
	if(m_encoder) m_encoder->deleteVideoEncoder();
	isStartVideoEncoder=0;

}
void YangP2pPublishImpl::startAudioCaptureState() {
	if (m_capture )
		m_capture->startAudioCaptureState();
}
YangVideoBuffer* YangP2pPublishImpl::getPreVideoBuffer(){
	if (m_capture )  return	m_capture->getPreVideoBuffer();
	return NULL;
}

void YangP2pPublishImpl::startVideoCaptureState() {
	if (m_capture )
		m_capture->startVideoCaptureState();
}

void YangP2pPublishImpl::stopAudioCaptureState() {
	if (m_capture )
		m_capture->stopAudioCaptureState();
}

void YangP2pPublishImpl::stopVideoCaptureState() {
	if (m_capture )
		m_capture->stopVideoCaptureState();
}


YangVideoBuffer* YangP2pPublishImpl::getOutPreVideoBuffer(){
	return m_outPreVideoBuffer;
}

//YangVideoBuffer* YangP2pPublishImpl::getOutVideoBuffer(){
	//return	m_outVideoBuffer;

//}
YangAudioEncoderBuffer * YangP2pPublishImpl::getOutAudioBuffer(){
	if(m_encoder) return m_encoder->getOutAudioBuffer();
	return NULL;
}
YangVideoEncoderBuffer * YangP2pPublishImpl::getOutVideoBuffer(){
	if(m_encoder) return m_encoder->getOutVideoBuffer();
	return NULL;
}
YangVideoMeta * YangP2pPublishImpl::getOutVideoMetaData(){
	if(m_encoder) m_encoder->getOutVideoMetaData();
	return NULL;
}
void YangP2pPublishImpl::startCamera() {
	startVideoCapture();
}


void YangP2pPublishImpl::stopCamera() {
	if (m_capture ) m_capture->stopVideoSource();
}


