//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangdecoder/YangH264DecoderSoft2.h>

#if !Yang_Enable_H264Decoder_So
YangH264DecoderSoft2::YangH264DecoderSoft2() {
	m_buffer=NULL;
	m_bufLen=0;
	m_decoder=NULL;
}

YangH264DecoderSoft2::~YangH264DecoderSoft2() {
	if(m_buffer) delete[] m_buffer;
	m_buffer=NULL;
	yang_delete(m_decoder);
}

void YangH264DecoderSoft2::getH264RtmpHeader(uint8_t *buf, uint8_t *src,int32_t *hLen) {

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
void YangH264DecoderSoft2::parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps){
	init();
	uint8_t headers[128];
	memset(headers, 0, 128);
	int32_t headerLen = 0;
	getH264RtmpHeader(p, headers, &headerLen);
	m_decoder->init(headers,headerLen);
	*pwid=m_decoder->m_context->m_width;
	*phei=m_decoder->m_context->m_height;
	*pfps=m_decoder->m_context->m_fps;
	if(!m_buffer) m_buffer=new uint8_t[m_decoder->m_context->m_width*m_decoder->m_context->m_height*3/2];

}
void YangH264DecoderSoft2::init(){
	if(m_decoder==NULL) m_decoder=new YangDecoderH264();;
}
int32_t YangH264DecoderSoft2::decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback){
	int32_t ret=0;

	if(m_decoder)  ret=m_decoder->decode(1,videoFrame->payload,videoFrame->nb,yuvtype,m_buffer,&m_bufLen);
	videoFrame->payload=m_buffer;
	videoFrame->nb=m_bufLen;
	if(pcallback&&m_bufLen) pcallback->onVideoData(videoFrame);
	return ret;
}

#endif
