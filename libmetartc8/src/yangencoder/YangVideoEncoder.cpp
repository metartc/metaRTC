//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangencoder/YangVideoEncoder.h>


YangVideoEncoder::YangVideoEncoder() {
	m_isInit=0;
	yang_memset(&m_videoInfo,0,sizeof(YangVideoInfo));
	m_vbuffer=new uint8_t[YANG_VIDEO_ENCODE_BUFFER_LEN];
}

YangVideoEncoder::~YangVideoEncoder() {
	if(m_vbuffer) delete m_vbuffer;
	m_vbuffer=NULL;
}

void YangVideoEncoder::setVideoPara(YangVideoInfo *videoInfo,YangVideoEncInfo *encInfo){
	yang_memcpy(&m_videoInfo,videoInfo,sizeof(YangVideoInfo));
	yang_memcpy(&m_enc,encInfo,sizeof(YangVideoEncInfo));
}

