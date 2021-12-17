/*
 * YangH264DecoderSoft.cpp
 *
 *  Created on: 2020年9月28日
 *      Author: yang
 */

#include "YangH264DecoderSoft.h"
#include <stdio.h>
#include <memory.h>
YangH264DecoderSoft::YangH264DecoderSoft() {
	m_lib.loadObject("libyangh264dec");
	context=NULL;
	m_buffer=NULL;
	m_bufLen=0;
	yang_getYangH264DecContext=(struct YangH264DecContext *(*)())m_lib.loadFunction("getYangH264DecContext");
	yang_init_H264DecCont=(void (*)(struct YangH264DecContext *cont,uint8_t *headers, int32_t headerLen))m_lib.loadFunction("init_H264DecCont");
	yang_decode=(int32_t (*)(struct YangH264DecContext *cont,int32_t isIframe,uint8_t *pData, int32_t nSize,enum YangYuvType yuvType,
			uint8_t *dest, int32_t *pnFrameReturned))m_lib.loadFunction("decode");
	yang_decode_close=(void (*)(struct YangH264DecContext *cont))m_lib.loadFunction("decode_close");
}

YangH264DecoderSoft::~YangH264DecoderSoft() {

	//if(m_dec) delete m_dec;
	if(m_buffer) delete[] m_buffer;
	m_buffer=NULL;
	if(context){
		yang_decode_close(context);
		context=NULL;
	}
	yang_getYangH264DecContext=NULL;
	yang_init_H264DecCont=NULL;
	yang_decode=NULL;
	yang_decode_close=NULL;
	m_lib.unloadObject();

}

void YangH264DecoderSoft::getH264RtmpHeader(uint8_t *buf, uint8_t *src,int32_t *hLen) {

	int32_t spsLen = *(buf + 12) + 1;
	uint8_t *spsStart = buf + 13;

	int32_t ppsLen = *(spsStart + spsLen + 1) + 1;
	uint8_t *ppsStart = buf + 13 + spsLen + 2;
	*(src + 3) = 0x01;
	memcpy(src + 4, spsStart, spsLen);
	*(src + 4 + spsLen + 3) = 0x01;
	memcpy(src + 4 + spsLen + 4, ppsStart, ppsLen);
	*hLen = 8 + spsLen + ppsLen;
	/**printf("\n************************************sps*******len=%d\n",spsLen);
	for(int32_t i=0;i<spsLen;i++) printf("%02x,",spsStart[i]);
	printf("\n************************************pps*******len=%d\n",ppsLen);
	for(int32_t i=0;i<ppsLen;i++) printf("%02x,",ppsStart[i]);
	printf("\n");
	**/
}
void YangH264DecoderSoft::parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps){
	init();
	uint8_t headers[128];
	memset(headers, 0, 128);
	int32_t headerLen = 0;
	getH264RtmpHeader(p, headers, &headerLen);
	yang_init_H264DecCont(context,headers,headerLen);
	*pwid=context->m_width;
	*phei=context->m_height;
	*pfps=context->m_fps;
	if(!m_buffer) m_buffer=new uint8_t[context->m_width*context->m_height*3/2];
	//printf("\ndecode wid=%d,hei=%d,fps=%d....................\n",*pwid,*phei,*pfps);
}
	void YangH264DecoderSoft::init(){
		if(context==NULL) context=yang_getYangH264DecContext();
	}
	int32_t YangH264DecoderSoft::decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback){
		int32_t ret=0;

		if(context)  ret=yang_decode(context,1,videoFrame->payload,videoFrame->nb,yuvtype,m_buffer,&m_bufLen);
		videoFrame->payload=m_buffer;
		videoFrame->nb=m_bufLen;
		if(pcallback&&m_bufLen) pcallback->onVideoData(videoFrame);
		return ret;
	}

	//int32_t YangH264DecoderSoft::decode(int32_t isIframe, uint8_t *pData, int32_t nSize, uint8_t *dest, int32_t *pnFrameReturned){
		//if(context)  return yang_decode(context,isIframe,pData,nSize,dest,pnFrameReturned);
		//printf("\n*****decodesize===%d....",*pnFrameReturned);
		//return 0;
	//}
