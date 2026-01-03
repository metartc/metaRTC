//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangdecoder/YangVideoDecoderMac.h>

void yang_decodermac_putFrame(void* session,YangFrame* frame){
	YangVideoDecoderMac* decoder=(YangVideoDecoderMac*)session;
	if(session==NULL||frame==NULL) return;
	if(decoder->m_callback)
		decoder->m_callback->onVideoData(frame);
}

YangVideoDecoderMac::YangVideoDecoderMac() {
	m_decoder=(YangCVideoDecoder*)yang_calloc(sizeof(YangCVideoDecoder),1);
	m_buffer=NULL;
	m_bufLen=0;
	m_callback=NULL;
	yang_create_ios_decoder(m_decoder);
	m_macCallback.session=this;
	m_macCallback.onVideoData=yang_decodermac_putFrame;

}

YangVideoDecoderMac::~YangVideoDecoderMac() {


	if(m_buffer) delete[] m_buffer;
	m_buffer=NULL;

	yang_destroy_ios_decoder(m_decoder);
	yang_free(m_decoder);
}


void YangVideoDecoderMac::parseHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps){
	init();
	if(m_decoder)
		m_decoder->parseHeader(m_decoder->session,p,pLen,pwid,phei,pfps);

}

void YangVideoDecoderMac::init(){
	if(m_decoder)
		m_decoder->init(m_decoder->session,&m_macCallback);
}

int32_t YangVideoDecoderMac::decode(YangFrame* videoFrame,YangColorSpace yuvtype,YangDecoderCallback* pcallback){
	m_callback=pcallback;
    if(videoFrame==NULL||videoFrame->payload==NULL)
    	return ERROR_CODEC_Decode;


	if(m_decoder)
		return m_decoder->decode(m_decoder->session,videoFrame);

	return 1;
}
