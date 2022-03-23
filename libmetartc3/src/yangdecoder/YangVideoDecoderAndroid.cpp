//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangdecoder/YangVideoDecoderAndroid.h>
#ifdef __ANDROID__
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangMeta.h>

#include "stdlib.h"
#ifdef _WIN32
#include "YangH264Header.h"
#endif



YangVideoDecoderAndroid::YangVideoDecoderAndroid(YangVideoInfo *pcontext,YangVideoCodec pencdectype) {
	m_encDecType= pencdectype;
	m_context=pcontext;
	usingHw = 0;	//pcontext->usingHwDec==2?1:0;
	if(pcontext->videoDecHwType>0) usingHw=1;

	m_buffer=NULL;
	m_bufLen=0;

	g_hwType=(YangVideoHwType)pcontext->videoDecHwType;
	m_bitDepth=pcontext->bitDepth;

	m_width = 0;
	m_height = 0;

	yLen = 0;
	uLen = 0;
	allLen = 0;
	buffer = NULL;
	ret = 0;



}
YangVideoDecoderAndroid::~YangVideoDecoderAndroid() {
	m_context=NULL;
	if(m_buffer) delete[] m_buffer;
	m_buffer=NULL;
	decode_close();


}
YangVideoHwType YangVideoDecoderAndroid::g_hwType=YangV_Hw_Intel;
void  YangVideoDecoderAndroid::parseHeaderH265(uint8_t *p,int32_t pLen,int32_t *pwid,int32_t *phei,int32_t *pfps){


}
void  YangVideoDecoderAndroid::parseHeaderH264(uint8_t *p,int32_t pLen,int32_t *pwid,int32_t *phei,int32_t *pfps){

}
void YangVideoDecoderAndroid::parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid,
		int32_t *phei, int32_t *pfps) {

	uint8_t headers[128];
	memset(headers, 0, 128);
	int32_t headerLen = 0;
	if(m_encDecType==Yang_VED_264){
        yang_getH264RtmpHeader(p, headers, &headerLen);
#ifdef _WIN32
        YangH264Header h264header;
        h264header.parseRtmpHeader(p);
        *pwid = h264header.width;
        *phei  = h264header.height;
        *pfps = h264header.fps;
#else
		parseHeaderH264(headers,headerLen,pwid,phei,pfps);
#endif
	}else if(m_encDecType==Yang_VED_265) {
		yang_getH265RtmpHeader(p, headers, &headerLen);
		parseHeaderH265(headers,headerLen,pwid,phei,pfps);

	}
	m_width = *pwid;
	m_height= *phei ;

	if(!m_buffer) m_buffer=new uint8_t[m_width*m_height*3/2];

	int32_t bitLen=(m_bitDepth==8?1:2);

	yLen = m_width * m_height*bitLen;
	uLen = yLen / 4;
	allLen = yLen * 3 / 2;




	m_isInit = 1;
}

void YangVideoDecoderAndroid::init() {
	//AVCodecID codec_id = AV_CODEC_ID_H264;
	// av_register_all();
	// avcodec_register_all();
	// avcodec_register(AV_CODEC_ID_H264);



}

int32_t YangVideoDecoderAndroid::decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback){
	if (usingHw)
			return decode_2(videoFrame,yuvtype,pcallback);
		else
			return decode_1(videoFrame,yuvtype,pcallback);

}
int32_t YangVideoDecoderAndroid::decode_1(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback) {

        m_bufLen= allLen;
        videoFrame->payload=m_buffer;
        videoFrame->nb=m_bufLen;
        if(pcallback) pcallback->onVideoData(videoFrame);

    return Yang_Ok;
}
int32_t YangVideoDecoderAndroid::decode_2(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback) {

        m_bufLen = allLen;
        videoFrame->payload=m_buffer;
        videoFrame->nb=m_bufLen;
        if(pcallback) pcallback->onVideoData(videoFrame);

    return Yang_Ok;
}
void YangVideoDecoderAndroid::decode_close() {

	buffer = NULL;

}

#endif
