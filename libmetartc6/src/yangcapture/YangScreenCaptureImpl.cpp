//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangScreenCaptureImpl.h"
#include <yangavutil/video/YangYuvConvert.h>
#include <yangutil/yangavinfotype.h>
#include "yangutil/sys/YangTime.h"
#include "yangutil/sys/YangLog.h"
#include <stdio.h>
#include <stdlib.h>

#include <yangutil/yang_unistd.h>
#include <fcntl.h>
#include <malloc.h>
#ifndef _WIN32
#ifndef __ANDROID__
#include <yangcapture/linux/YangX11Screen.h>
#endif
#else
#include "win/YangDXGIManager.h"
#endif
int32_t YangScreenCaptureImpl::init() {

	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(
				m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	if (m_pre_videoBuffer == NULL)
		m_pre_videoBuffer = new YangVideoBuffer(
				m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	return Yang_Ok;
}
YangScreenCaptureImpl::YangScreenCaptureImpl(YangContext *pcontext) {
	m_context = pcontext;
	m_width = 0;
	m_height = 0;
	m_rgb = NULL;
	m_bufLen = 0;
	m_fd = 0;
	m_out_videoBuffer = nullptr;
	m_pre_videoBuffer = nullptr;
	isCapture = 0;
	m_loop = 0;
	m_state = 0;
    m_interval=40;
    m_isDrawmouse=false;
}

YangScreenCaptureImpl::~YangScreenCaptureImpl() {
    if(m_isStart){
        stop();
        yang_stop_thread(this);
    }
	m_context = NULL;
	yang_delete(m_pre_videoBuffer);
	yang_delete(m_out_videoBuffer);
}

void YangScreenCaptureImpl::setVideoCaptureStart() {
	m_state = 1;
}
void YangScreenCaptureImpl::setVideoCaptureStop() {
	m_state = 0;
}
int32_t YangScreenCaptureImpl::getVideoCaptureState(){
	return m_state;
}
YangVideoBuffer* YangScreenCaptureImpl::getPreVideoBuffer() {
	return m_pre_videoBuffer;
}
YangVideoBuffer* YangScreenCaptureImpl::getOutVideoBuffer() {
	return m_out_videoBuffer;
}
void YangScreenCaptureImpl::putBuffer(YangFrame *videoFrame) {
	m_out_videoBuffer->putVideo(videoFrame);
}

void YangScreenCaptureImpl::setDrawmouse(bool isDraw) {
	m_isDrawmouse=isDraw;

}

bool YangScreenCaptureImpl::getisDrawmouse() {
	return m_isDrawmouse;
}

void YangScreenCaptureImpl::stopLoop() {
	m_loop = 0;
}
void YangScreenCaptureImpl::startLoop() {
#ifdef _MSC_VER
	CoInitialize(NULL);
	YangDXGIManager g_DXGIManager;
	g_DXGIManager.SetCaptureSource(CSDesktop);

	RECT rcDim;
	g_DXGIManager.GetOutputRect(rcDim);

	int dwWidth = rcDim.right - rcDim.left;
	int dwHeight = rcDim.bottom - rcDim.top;
	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(
                m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	if (m_pre_videoBuffer == NULL)
		m_pre_videoBuffer = new YangVideoBuffer(
                m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
    m_out_videoBuffer->init(dwWidth, dwHeight, m_context->avinfo.video.videoEncoderFormat);
    m_pre_videoBuffer->init(dwWidth, dwHeight, m_context->avinfo.video.videoEncoderFormat);
	yang_trace("dwWidth=%d dwHeight=%d\n", dwWidth, dwHeight);

	DWORD dwBufSize = dwWidth * dwHeight * 4;
	int yuvLen=dwWidth * dwHeight * 3 / 2;
	YangYuvConvert yuv;
    uint8_t *pBuf = new uint8_t[dwBufSize];
    uint8_t *dst = new uint8_t[yuvLen];
	YangFrame frame;
	memset(&frame,0,sizeof(YangFrame));
	int64_t startTime = 0;
	frame.payload = dst;
	frame.nb = dwWidth * dwHeight * 3 / 2;
    if(m_context->avinfo.video.videoEncoderFormat==YangArgb) {
        frame.nb = dwWidth * dwHeight * 4;
        frame.payload=pBuf;
    }
	HRESULT hr = 0;
	m_loop = 1;
	while (m_loop) {
		yang_usleep(m_interval*1000);
		g_DXGIManager.m_isDrawmouse=m_isDrawmouse;
		hr = g_DXGIManager.GetOutputBits(pBuf, rcDim);
		if (FAILED(hr)) {
            if(hr==DXGI_ERROR_WAIT_TIMEOUT){
                //yang_error("GetOutputBits failed because DXGI_ERROR_WAIT_TIMEOUT\n");
            }else
				yang_error("GetOutputBits failed with hr=0x%08x\n", hr);

		} else {
			if (startTime == 0)
				startTime = yang_get_system_time();

            if(m_context->avinfo.video.videoEncoderFormat==YangI420) yuv.bgratoI420(pBuf, dst, dwWidth, dwHeight);
			//yuv.rgbatoI420(pBuf, dst, dwWidth, dwHeight);

            frame.pts = yang_get_system_time() - startTime;
			m_pre_videoBuffer->putVideo(&frame);
			if (m_state){
				m_out_videoBuffer->putVideo(&frame);
			}

		}

	}
	yang_deleteA(pBuf);
	yang_deleteA(dst);
    CoUninitialize();
#else
#ifndef __ANDROID__
	YangX11Screen x11;

	int dwWidth = x11.m_width;
	int dwHeight = x11.m_height;
	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(
				m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	if (m_pre_videoBuffer == NULL)
		m_pre_videoBuffer = new YangVideoBuffer(
				m_context->avinfo.video.bitDepth == 8 ? 1 : 2);
	m_out_videoBuffer->init(dwWidth, dwHeight, m_context->avinfo.video.videoEncoderFormat);
	m_pre_videoBuffer->init(dwWidth, dwHeight, m_context->avinfo.video.videoEncoderFormat);
	yang_trace("dwWidth=%d dwHeight=%d\n", dwWidth, dwHeight);


	int yuvLen=dwWidth * dwHeight * 3 / 2;
	YangYuvConvert yuv;
	uint8_t *pBuf = NULL;
	uint8_t *dst = new uint8_t[yuvLen];
	YangFrame frame;
	memset(&frame,0,sizeof(YangFrame));
	int64_t startTime = 0;
	frame.payload = dst;
	frame.nb = dwWidth * dwHeight * 3 / 2;
	if(m_context->avinfo.video.videoEncoderFormat==YangArgb) {
		frame.nb = dwWidth * dwHeight * 4;
		frame.payload=pBuf;
	}

	m_loop = 1;
	while (m_loop) {
		yang_usleep(m_interval*1000);
		x11.m_isDrawmouse=m_isDrawmouse;
		pBuf=x11.captureFrame();

		if (pBuf){
			if (startTime == 0)
				startTime = yang_get_system_time();

			if(m_context->avinfo.video.videoEncoderFormat==YangI420) yuv.bgratoI420(pBuf, dst, dwWidth, dwHeight);
			//yuv.rgbatoI420(pBuf, dst, dwWidth, dwHeight);

			frame.pts = yang_get_system_time() - startTime;
			m_pre_videoBuffer->putVideo(&frame);
			if (m_state){
				m_out_videoBuffer->putVideo(&frame);
			}

		}

	}
	yang_deleteA(dst);
#endif
#endif
}

void YangScreenCaptureImpl::setInterval(int32_t pinterval) {
	m_interval=pinterval;
}
