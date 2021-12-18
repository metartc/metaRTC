/*
 * YangScreenCapture.cpp
 *
 *  Created on: 2020年8月30日
 *      Author: yang
 */

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
#include <sys/ioctl.h>
#include <linux/fb.h>
int32_t YangScreenCaptureImpl::init(){

	struct fb_var_screeninfo fb_var_info;
	struct fb_fix_screeninfo fb_fix_info;



	// 打开framebuffer设备
	m_fd = open("/dev/fb0", O_RDONLY);
	if(m_fd < 0)
	{
		yang_error("can not open dev\n");
		return ERROR_SYS_Linux_ScreenDeviceOpenFailure;
	}

	// 获取LCD的可变参数
	ioctl(m_fd, FBIOGET_VSCREENINFO, &fb_var_info);
	// 一个像素多少位
	printf("bits_per_pixel: %d\n", fb_var_info.bits_per_pixel);
	// 一个像素多少位
	printf("bits_per_pixel: %d\n", fb_var_info.bits_per_pixel);
	// x分辨率
	printf("xres: %d\n", fb_var_info.xres);
	// y分辨率
	printf("yres: %d\n", fb_var_info.yres);
	// r分量长度(bit)
	printf("red_length: %d\n", fb_var_info.red.length);
	// g分量长度(bit)
	printf("green_length: %d\n", fb_var_info.green.length);
	// b分量长度(bit)
	printf("blue_length: %d\n", fb_var_info.blue.length);
	// t(透明度)分量长度(bit)
	printf("transp_length: %d\n", fb_var_info.transp.length);
	// r分量偏移
	printf("red_offset: %d\n", fb_var_info.red.offset);
	// g分量偏移
	printf("green_offset: %d\n", fb_var_info.green.offset);
	// b分量偏移
	printf("blue_offset: %d\n", fb_var_info.blue.offset);
	// t分量偏移
	printf("transp_offset: %d\n", fb_var_info.transp.offset);

	// 获取LCD的固定参数
	ioctl(m_fd, FBIOGET_FSCREENINFO, &fb_fix_info);
	// 一帧大小
	printf("smem_len: %d\n", fb_fix_info.smem_len);
	// 一行大小
	printf("line_length: %d\n", fb_fix_info.line_length);
	// 一帧大小
	m_bufLen = (fb_var_info.xres * fb_var_info.yres * fb_var_info.bits_per_pixel / 8);
	m_width=fb_var_info.xres;
	m_height=fb_var_info.yres;
	m_rgb = (uint8_t *)malloc(m_bufLen);
	//if(trgb == NULL)		exit(0);
	m_rgb = (uint8_t *)malloc(fb_var_info.xres * fb_var_info.yres * 3);
	//if(m_rgb == NULL)
	//{
	//	goto here;
	//}

	return Yang_Ok;
}
#else
#include "YangDXGIManager.h"

int32_t YangScreenCaptureImpl::init() {

	if (m_out_videoBuffer == NULL)
		m_out_videoBuffer = new YangVideoBuffer(
				m_context->video.bitDepth == 8 ? 1 : 2);
	if (m_pre_videoBuffer == NULL)
		m_pre_videoBuffer = new YangVideoBuffer(
				m_context->video.bitDepth == 8 ? 1 : 2);
	return Yang_Ok;
}

#endif
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
				m_context->video.bitDepth == 8 ? 1 : 2);
	if (m_pre_videoBuffer == NULL)
		m_pre_videoBuffer = new YangVideoBuffer(
				m_context->video.bitDepth == 8 ? 1 : 2);
    m_out_videoBuffer->init(dwWidth, dwHeight, m_context->video.videoEncoderFormat);
    m_pre_videoBuffer->init(dwWidth, dwHeight, m_context->video.videoEncoderFormat);
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
    if(m_context->video.videoEncoderFormat==YangArgb) {
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

            if(m_context->video.videoEncoderFormat==YangI420) yuv.bgratoI420(pBuf, dst, dwWidth, dwHeight);
			//yuv.rgbatoI420(pBuf, dst, dwWidth, dwHeight);

			frame.timestamp = yang_get_system_time() - startTime;
			m_pre_videoBuffer->putVideo(&frame);
			if (m_state){
				//yang_trace("%d,",frame.nb);
				m_out_videoBuffer->putVideo(&frame);
			}

		}

	}
	yang_deleteA(pBuf);
	yang_deleteA(dst);
    CoUninitialize();
#endif
}

void YangScreenCaptureImpl::setInterval(int32_t pinterval) {
	m_interval=pinterval;
}
