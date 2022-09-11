//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangVideoCaptureHandle.h"
#include <yangutil/sys/YangTime.h>
#include "time.h"
#include "stdlib.h"
#include "stdio.h"


YangVideoCaptureHandle::YangVideoCaptureHandle(YangVideoInfo *pcontext) {
	memset(&m_videoFrame,0,sizeof(YangFrame));
	curstamp = 0;
	basesatmp = 0;
	m_isCapture = 0;
	m_isLivingCaptrue=0;
	m_isFilm=0;
	vtick=0;

	m_start_time = 0;
	m_out_videoBuffer = NULL;
	m_pre_videoBuffer=NULL;
	m_living_out_videoBuffer=NULL;
	m_film_out_videoBuffer=NULL;
	m_width=pcontext->width;
	m_height=pcontext->height;
	m_buf=NULL;
	m_bufLen=m_width*m_height*3/2;

	m_encoderVideoFormat=pcontext->videoEncoderFormat;
	m_captureVideoFormat=pcontext->videoCaptureFormat;
	m_isHardwareEncoder=pcontext->videoEncHwType==Yang_Hw_Soft?false:true;

	m_rotate=pcontext->rotate;

}
YangVideoCaptureHandle::~YangVideoCaptureHandle(void) {
	m_out_videoBuffer=NULL;
	m_pre_videoBuffer=NULL;
	m_living_out_videoBuffer=NULL;
	m_film_out_videoBuffer=NULL;
	if(m_buf) delete[] m_buf;
	m_buf=NULL;

}
void YangVideoCaptureHandle::setCaptureFormat(int32_t pformat){
	if(pformat==YangYuy2&&!m_buf) m_buf=new uint8_t[m_bufLen];
}
void  YangVideoCaptureHandle::setVideoBuffer(YangVideoBuffer *pbuf){
	 m_out_videoBuffer=pbuf;
}
void  YangVideoCaptureHandle::setPreVideoBuffer(YangVideoBuffer *pbuf){
	 m_pre_videoBuffer=pbuf;
}
void  YangVideoCaptureHandle::setLivingVideoBuffer(YangVideoBuffer *pbuf){
	m_living_out_videoBuffer=pbuf;
}
void  YangVideoCaptureHandle::setFilmVideoBuffer(YangVideoBuffer *pbuf){
	m_film_out_videoBuffer=pbuf;
}
void YangVideoCaptureHandle::initstamp() {
	basesatmp = curstamp;	//-m_adjust_time*10000;
}
void YangVideoCaptureHandle::putBufferAndroid(int64_t startTime, uint8_t *pBuffer, int32_t BufferLen){
	curstamp=yang_get_system_time();
	if(basesatmp==0) {
		basesatmp=curstamp;
		if(m_rotate&&m_buf==NULL) m_buf=new uint8_t[m_bufLen];
	}
	vtick = curstamp - basesatmp;
	if(m_rotate){
		//if(m_isRotate)
			m_yuv.rotateI420(pBuffer,m_buf,m_width,m_height,(RotationMode)m_rotate);
		//else
			//m_yuv.i420tonv12(pBuffer, m_buf, m_width, m_height);
		m_videoFrame.payload=m_buf;
	}else{
			m_videoFrame.payload=pBuffer;
	}

	m_videoFrame.nb=m_bufLen;
	m_videoFrame.pts=vtick;
	if (m_isCapture&&m_out_videoBuffer) m_out_videoBuffer->putVideo(&m_videoFrame);
}
void YangVideoCaptureHandle::putBuffer(int64_t pstamtime,	uint8_t *pBuffer, int32_t BufferLen) {

	curstamp = pstamtime;
	if(basesatmp==0) basesatmp=pstamtime;
	vtick = pstamtime - basesatmp;

	m_videoFrame.pts=vtick;
	uint8_t* tmp=NULL;
	if(m_buf){
		if(m_captureVideoFormat==YangYuy2){
			if(m_encoderVideoFormat==YangI420) m_yuv.yuy2toI420(pBuffer,m_buf,m_width,m_height);
			if(m_encoderVideoFormat==YangNv12) m_yuv.yuy2tonv12(pBuffer,m_buf,m_width,m_height);
			if(m_encoderVideoFormat==YangArgb) m_yuv.yuy2toargb(pBuffer,m_buf,m_width,m_height);
			tmp=m_buf;
		}else if(m_captureVideoFormat==YangI420){
			if(m_encoderVideoFormat==YangI420) tmp=m_buf;
			if(m_encoderVideoFormat==YangNv12) m_yuv.i420tonv12(pBuffer,m_buf,m_width,m_height);
			//if(m_encoderVideoFormat==YangArgb) m_yuv.i420torgba(pBuffer,m_buf,m_width,m_height);
		}else if(m_captureVideoFormat==YangArgb){
			tmp=pBuffer;
		}
		m_videoFrame.payload=tmp;
		m_videoFrame.nb=m_bufLen;
		putBuffers();

	}else{
		m_videoFrame.payload=pBuffer;
		m_videoFrame.nb=BufferLen;
		putBuffers();

	}
	//return;
}

void YangVideoCaptureHandle::putBuffers() {

	if(m_pre_videoBuffer) m_pre_videoBuffer->putVideo(&m_videoFrame);
	if(m_isFilm&&m_film_out_videoBuffer) m_film_out_videoBuffer->putVideo(&m_videoFrame);
	if (m_isCapture&&m_out_videoBuffer) {
		m_out_videoBuffer->putVideo(&m_videoFrame);

	}
	if (m_isLivingCaptrue&&m_living_out_videoBuffer){
		m_living_out_videoBuffer->putVideo(&m_videoFrame);
	}


}
