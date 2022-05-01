//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangEncoderGpu.h"
#include <yangutil/sys/YangLog.h>
YangEncoderGpu::YangEncoderGpu() {

    m_encoder=NULL;
    m_sendKeyframe=0;

}

YangEncoderGpu::~YangEncoderGpu() {
	// TODO Auto-generated destructor stub
}

void YangEncoderGpu::sendKeyFrame(){
	m_sendKeyframe=1;
}

void YangEncoderGpu::setVideoMetaData(YangVideoMeta *pvmd) {

}

/*void YangEncoderGpu::initX264Param(YangVideoInfo *pvp,		YangVideoEncInfo *penc) {

}*/
bool YangEncoderGpu::isSupported()
{
    return false;
}
int32_t YangEncoderGpu::init(YangVideoInfo *pvp, YangVideoEncInfo *penc) {
	if (m_isInit == 1)		return Yang_Ok;
    if(m_encoder==NULL) m_encoder=new YangH264EncoderGpu(pvp,penc);
    if(m_encoder) m_encoder->init();

	m_isInit = 1;

	return Yang_Ok;

}

int32_t YangEncoderGpu::encode(YangFrame* pframe, YangEncoderCallback* pcallback) {
    if(m_encoder&&m_encoder->encode(pframe)>0){
         //  yang_trace("\n%d:",pframe->nb);
         //    for(int j=0;j<50;j++) yang_trace("%02x,",pframe->payload[j]);
        if(pcallback) pcallback->onVideoData(pframe);
        return Yang_Ok;
    }
	return 1;
}
