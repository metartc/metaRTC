//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/buffer/YangAudioEncoderBuffer.h>

YangAudioEncoderBuffer::YangAudioEncoderBuffer(int32_t paudioCacheNum)
{
	resetIndex();
	m_cache_num=paudioCacheNum;
	initFrames(m_cache_num,1024);

}

YangAudioEncoderBuffer::~YangAudioEncoderBuffer()
{

}


void YangAudioEncoderBuffer::reset(){
	resetIndex();
}

void YangAudioEncoderBuffer::putAudio(YangFrame* audioFrame){
	putPlayAudio(audioFrame);
}

void YangAudioEncoderBuffer::getAudio(YangFrame* audioFrame){
	getPlayAudio(audioFrame);
}
uint8_t* YangAudioEncoderBuffer::getAudioRef(YangFrame* frame){
	return getFrameRef(frame);
}
void YangAudioEncoderBuffer::putPlayAudio(YangFrame* audioFrame){
	putFrame(audioFrame);
}

void YangAudioEncoderBuffer::getPlayAudio(YangFrame* audioFrame){
	getFrame(audioFrame);
}


