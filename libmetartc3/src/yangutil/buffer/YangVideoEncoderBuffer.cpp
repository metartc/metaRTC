//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "yangutil/buffer/YangVideoEncoderBuffer.h"

#include <stdlib.h>
#include "string.h"

YangVideoEncoderBuffer::YangVideoEncoderBuffer(int pcachenum)
{
	resetIndex();
	m_cache_num=pcachenum;
	initFrames(m_cache_num,YANG_VIDEO_ENCODE_BUFFER_LEN);

}

YangVideoEncoderBuffer::~YangVideoEncoderBuffer(void)
{

}


void YangVideoEncoderBuffer::getEVideo(YangFrame* pframe){

	getFrame(pframe);
}
uint8_t * YangVideoEncoderBuffer::getEVideoRef(YangFrame* frame){
	return getFrameRef(frame);

}
void YangVideoEncoderBuffer::putEVideo(YangFrame* pframe){
	putFrame(pframe);
}

