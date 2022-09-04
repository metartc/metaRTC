//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/buffer/YangVideoBuffer.h>

#include <stdlib.h>


YangVideoBuffer::~YangVideoBuffer(void)
{

}

YangVideoBuffer::YangVideoBuffer(int32_t pBitDepthLen)
{
	reset();
	m_bitDepthLen=pBitDepthLen;
	m_frames=0;
    m_cache_num = 30;
	m_width=0;
	m_height=0;
	m_length=0;

}


YangVideoBuffer::YangVideoBuffer(int32_t pwid,int32_t phei,YangYuvType ptype,int32_t pBitDepthLen){
	reset();
	m_bitDepthLen=pBitDepthLen;
	init(pwid,phei,ptype);

}
void YangVideoBuffer::init(int32_t pwid,int32_t phei,YangYuvType ptype){
	reset();
	m_width=pwid;
	m_height=phei;
	m_length=m_width*m_height*3*m_bitDepthLen/2;
	if(ptype==YangYuy2)  m_length=m_width*m_height*2*m_bitDepthLen;
	if(ptype==YangRgb)  m_length=m_width*m_height*3*m_bitDepthLen;
	if(ptype==YangArgb||ptype==YangBgra)  m_length=m_width*m_height*4*m_bitDepthLen;
    m_cache_num=30;
	initFrames(m_cache_num,m_length);

	m_frames=0;

}

void YangVideoBuffer::reset() {
	resetIndex();
}


void YangVideoBuffer::putVideo(YangFrame* pframe){
	putFrame(pframe);
}

void YangVideoBuffer::getVideo(YangFrame* pframe){

	getFrame(pframe);
}
uint8_t * YangVideoBuffer::getVideoRef(YangFrame* pframe){
	return getFrameRef(pframe);

}

int64_t YangVideoBuffer::getTimestamp(int64_t *ptimestamp){

	YangFrame* f=getCurFrameRef();
		if(f) *ptimestamp=f->pts;
	return 0;
}

int64_t YangVideoBuffer::getNextTimestamp(){
	return getNextFrameTimestamp();
}
YangFrame* YangVideoBuffer::getCurVideoFrame(){
	return getCurFrameRef();
}
