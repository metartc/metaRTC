//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/yang_unistd.h>
#include <yangp2p2/YangP2pCapture.h>
#include <yangavutil/video/YangYuvConvert.h>
#include <yangavutil/video/YangYuvUtil.h>




YangP2pCapture::YangP2pCapture() {
	m_context=NULL;
	m_out_audioBuffer = NULL;
	m_out_videoBuffer=NULL;
	m_pre_videoBuffer=NULL;

}

YangP2pCapture::~YangP2pCapture() {
	m_context=NULL;
}


YangVideoBuffer * YangP2pCapture::getOutVideoBuffer(){
	return m_out_videoBuffer;
}

YangVideoBuffer * YangP2pCapture::getPreVideoBuffer(){
	return m_pre_videoBuffer;
}




