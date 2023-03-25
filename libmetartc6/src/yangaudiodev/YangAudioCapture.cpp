//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangaudiodev/YangAudioCapture.h>
#include <yangutil/sys/YangLog.h>


YangAudioCapture::YangAudioCapture()
{
	aIndex=0;
	m_isStart=yangfalse;

}

YangAudioCapture::~YangAudioCapture()
{

}


void YangAudioCapture::run(){
	m_isStart=1;
	startLoop();
	m_isStart=0;
}
void YangAudioCapture::stop(){
	stopLoop();
}


