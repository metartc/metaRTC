//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangcapture/YangScreenShare.h>
#include <yangavutil/video/YangYuvConvert.h>


YangScreenCapture::YangScreenCapture(){
	m_isStart=0;
}
YangScreenCapture::~YangScreenCapture(){

 }

void YangScreenCapture::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangScreenCapture::stop() {
	stopLoop();

}
YangScreenShare::YangScreenShare() {
	m_isloop=0;
	m_out_videoBuffer=NULL;
	m_capture=NULL;
	m_isCapture=0;
	m_interval=0;
}

YangScreenShare::~YangScreenShare() {
	m_capture=NULL;
	m_out_videoBuffer=NULL;
}
void YangScreenShare::setOutVideoBuffer(YangVideoBuffer *pbuf){
	m_out_videoBuffer=pbuf;
}
void YangScreenShare::setScreenHandle(YangScreenCaptureHandleI *handle){
	m_capture=handle;
}
void YangScreenShare::setInterval(int32_t pinterval){
	m_interval=1000*pinterval;
}
void YangScreenShare::setVideoCaptureStart() {
	m_isCapture = 1;
}
void YangScreenShare::setVideoCaptureStop() {
	m_isCapture = 0;
}
int32_t YangScreenShare::getVideoCaptureState() {
	return m_isCapture;
}

void YangScreenShare::initstamp() {
	//m_vhandle->initstamp();
}
void YangScreenShare::stopLoop() {
	m_isloop = 0;
}
int32_t YangScreenShare::init() {
	return m_capture->init();
}
void YangScreenShare::startLoop() {
	m_isloop = 1;

}

