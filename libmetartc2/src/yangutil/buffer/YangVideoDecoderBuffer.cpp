//#include "stdafx.h"
#include "yangutil/buffer/YangVideoDecoderBuffer.h"

#include <stdlib.h>
#include "memory.h"




YangVideoDecoderBuffer::YangVideoDecoderBuffer(int num) {
	resetIndex();
	m_cache_num = num;
	initFrames(m_cache_num,YANG_VIDEO_ENCODE_BUFFER_LEN);
}

YangVideoDecoderBuffer::~YangVideoDecoderBuffer(void) {

}

void YangVideoDecoderBuffer::getEVideo(YangFrame* pframe) {
	getFrame(pframe);
}

void YangVideoDecoderBuffer::putEVideo(YangFrame* pframe) {
	putFrame(pframe);
}

