//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "YangDecoderLibde265.h"
#include <stdio.h>
#include <memory.h>

YangDecoderLibde265::YangDecoderLibde265() {

	m_buffer=NULL;
	m_bufLen=0;

}

YangDecoderLibde265::~YangDecoderLibde265() {


	if(m_buffer) delete[] m_buffer;
	m_buffer=NULL;


}

void YangDecoderLibde265::getH264RtmpHeader(uint8_t *buf, uint8_t *src,int32_t *hLen) {

	int32_t spsLen = *(buf + 12) + 1;
	uint8_t *spsStart = buf + 13;

	int32_t ppsLen = *(spsStart + spsLen + 1) + 1;
	uint8_t *ppsStart = buf + 13 + spsLen + 2;
	*(src + 3) = 0x01;
	memcpy(src + 4, spsStart, spsLen);
	*(src + 4 + spsLen + 3) = 0x01;
	memcpy(src + 4 + spsLen + 4, ppsStart, ppsLen);
	*hLen = 8 + spsLen + ppsLen;

}
void YangDecoderLibde265::parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps){
	init();
	uint8_t headers[128];
	memset(headers, 0, 128);
	int32_t headerLen = 0;
	getH264RtmpHeader(p, headers, &headerLen);


}
void YangDecoderLibde265::init(){

}
int32_t YangDecoderLibde265::decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback){
	int32_t ret=0;


	videoFrame->payload=m_buffer;
	videoFrame->nb=m_bufLen;
	if(pcallback&&m_bufLen) pcallback->onVideoData(videoFrame);
	return ret;
}


