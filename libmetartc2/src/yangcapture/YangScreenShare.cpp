/*
 * YangScreenShareLinux.cpp
 *
 *  Created on: 2020年8月30日
 *      Author: yang
 */

#include "YangScreenShare.h"
#include "yangutil/yang_unistd.h"
#include "yangavutil/video/YangYuvConvert.h"

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
	//m_capture=new YangScreenShareImpl();
	//m_vhandle=new YangVideoCaptureHandle();
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
    /**
	//m_vhandle->m_start_time = 0;
	int32_t width=m_capture->m_width;
	int32_t height=m_capture->m_height;
	uint8_t buf[width*height*4];
	uint8_t yuv[width*height*3/2];
	int32_t yuvLen=width*height*3/2;
	YangYuvConvert con;
	int64_t timestamp=0;
	YangFrame videoFrame;
memset(&m_audioFrame,0,sizeof(YangFrame));
	while (m_isloop) {
		m_capture->captureFrame(buf);
		con.rgb24toI420(buf,yuv,width,height);
		videoFrame.payload=yuv;
		videoFrame.nb=yuvLen;
		videoFrame.timestamp=timestamp;
		m_out_videoBuffer->putVideo(&videoFrame);
		yang_usleep(3000);
    }**/
}

