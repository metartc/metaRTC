//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangdecoder/YangDecoderOpenh264.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangSpspps.h>
#include<limits.h>

//#ifdef _MSC_VER
//#pragma comment (lib,"openh264.lib")
//#endif
YangDecoderOpenh264::YangDecoderOpenh264() {
	m_svcDecoder=NULL;
	m_buffer=NULL;
	m_bufLen=0;
	m_width=m_height=0;
	m_uPos=m_vPos=0;
}

YangDecoderOpenh264::~YangDecoderOpenh264() {
	if(m_svcDecoder){
		m_svcDecoder->Uninitialize();
		WelsDestroyDecoder(m_svcDecoder);
		m_svcDecoder=NULL;
	}
	yang_deleteA( m_buffer);
}

void YangDecoderOpenh264::getH264RtmpHeader(uint8_t *buf, uint8_t *src,int32_t *hLen) {
	int32_t spsLen = *(buf + 12) + 1;
	uint8_t *spsStart = buf + 13;
	memcpy(src,spsStart,spsLen);
	*hLen = spsLen;
}
void YangDecoderOpenh264::parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps){
	init();
	uint8_t headers[128];
	memset(headers, 0, 128);
	int32_t headerLen = 0;
	getH264RtmpHeader(p, headers, &headerLen);
	yang_h264_decode_spspps(headers,headerLen,pwid,phei,pfps);

	m_width=*pwid;
	m_height=*phei;
	m_bufLen=m_width*m_height*3/2;
	m_uPos=m_width*m_height;
	m_vPos=m_width*m_height+m_width*m_height/4;

	if(!m_buffer) m_buffer=new uint8_t[m_bufLen];

}
void YangDecoderOpenh264::init(){

	if (WelsCreateDecoder(&m_svcDecoder) != 0) {
		yang_error( "openh264 decoder:error create decoder\n");
		return;
	}

	SDecodingParam sDecParam = {0};
	sDecParam.uiTargetDqLayer = UCHAR_MAX;
	sDecParam.eEcActiveIdc = ERROR_CON_FRAME_COPY;//ERROR_CON_SLICE_COPY;
	sDecParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;//VIDEO_BITSTREAM_AVC;
	sDecParam.bParseOnly=false;

	m_svcDecoder->Initialize(&sDecParam);

}
int32_t YangDecoderOpenh264::decode(YangFrame* videoFrame,YangYuvType yuvtype,YangDecoderCallback* pcallback){
	if(videoFrame==NULL||videoFrame->payload==NULL) return 1;
	int32_t ret=0;
	memset(&m_dstBufInfo, 0, sizeof(SBufferInfo));
	uint8_t *pData[3] = {NULL};

	int result = m_svcDecoder->DecodeFrameNoDelay(videoFrame->payload, videoFrame->nb, pData, &m_dstBufInfo);
	if(result == 0 && m_dstBufInfo.iBufferStatus == 1){
		writeData(videoFrame,pData,pcallback);
	}else{
		yang_error("openh264 decode error:%d",m_dstBufInfo.iBufferStatus);
		ret=ERROR_CODEC_Decode;
	}
	return ret;
}


void YangDecoderOpenh264::writeData(YangFrame* videoFrame,uint8_t** pData,YangDecoderCallback* pcallback){
	videoFrame->payload=m_buffer;
	videoFrame->nb=m_bufLen;

	int i=0;
	int uvStride=m_dstBufInfo.UsrData.sSystemBuffer.iStride[1];
	int yStride=m_dstBufInfo.UsrData.sSystemBuffer.iStride[0];
	for (i = 0; i < m_height; i++) {
		memcpy(m_buffer + i * m_width, pData[0] + i * yStride, m_width);
	}
	for (i = 0; i < m_height / 2; i++) {
		memcpy(m_buffer + m_uPos+i * m_width / 2,pData[1] + i * uvStride, m_width / 2);
	}
	for (i = 0; i < m_height / 2; i++) {
		memcpy(m_buffer + (m_vPos+ i * m_width / 2), pData[2] + i * uvStride, m_width / 2);
	}
	if(pcallback&&m_bufLen) pcallback->onVideoData(videoFrame);
}


