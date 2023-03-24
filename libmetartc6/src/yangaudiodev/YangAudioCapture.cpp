//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangaudiodev/YangAudioCapture.h>
#include <yangutil/sys/YangLog.h>

YangAudioCapture::YangAudioCapture()
{
	aIndex=0;
	m_isStart=0;
	m_context=NULL;
}

YangAudioCapture::~YangAudioCapture()
{
	m_context=NULL;
}


void YangAudioCapture::run(){
	m_isStart=1;
	startLoop();
	m_isStart=0;
}
void YangAudioCapture::stop(){
	stopLoop();
}


