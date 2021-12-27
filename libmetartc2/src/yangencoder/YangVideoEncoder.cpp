/*
 * YangH246Encoder.cpp
 *
 *  Created on: 2019年10月16日
 *      Author: yang
 */

#include <yangencoder/YangVideoEncoder.h>
#include <string.h>

YangVideoEncoder::YangVideoEncoder() {
	m_isInit=0;
	memset(&m_videoInfo,0,sizeof(YangVideoInfo));
	m_vbuffer=new uint8_t[YANG_VIDEO_ENCODE_BUFFER_LEN];
}

YangVideoEncoder::~YangVideoEncoder() {
	if(m_vbuffer) delete m_vbuffer;
	m_vbuffer=NULL;
}

void YangVideoEncoder::setVideoPara(YangVideoInfo *pap,YangVideoEncInfo *penc){
	memcpy(&m_videoInfo,pap,sizeof(YangVideoInfo));
	memcpy(&m_enc,penc,sizeof(YangVideoEncInfo));
}

