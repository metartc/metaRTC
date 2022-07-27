//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/buffer2/YangVideoEncoderBuffer2.h>

#include <stdlib.h>
#include "string.h"




void yang_videoEncoderBuffer2_getEVideo(YangMediaBuffer2* buf,YangFrame* pframe){

	buf->getFrame(&buf->session,pframe);
}
uint8_t * yang_videoEncoderBuffer2_getEVideoRef(YangMediaBuffer2* buf,YangFrame* frame){
	return buf->getFrameRef(&buf->session,frame);

}
void yang_videoEncoderBuffer2_putEVideo(YangMediaBuffer2* buf,YangFrame* pframe){
	buf->putFrame(&buf->session,pframe);
}
int32_t yang_videoEncoderBuffer2_size(YangMediaBuffer2* buf){
	return buf->session.size;
}
void yang_videoEncoderBuffer2_resetIndex(YangMediaBuffer2* buf){
	 buf->resetIndex(&buf->session);
}

void yang_create_videoEncoderBuffer2(YangVideoEncoderBuffer2* buf,int pcachenum){
	YangMediaBuffer2* mb=&buf->mediaBuffer;
	yang_create_mediaBuffer2(mb);
	mb->resetIndex(&mb->session);
	mb->session.cache_num=pcachenum;
	mb->initFrames(&mb->session,mb->session.cache_num,YANG_VIDEO_ENCODE_BUFFER_LEN);
	buf->getEVideo=yang_videoEncoderBuffer2_getEVideo;
	buf->getEVideoRef=yang_videoEncoderBuffer2_getEVideoRef;
	buf->putEVideo=yang_videoEncoderBuffer2_putEVideo;
	buf->size=yang_videoEncoderBuffer2_size;
	buf->resetIndex=yang_videoEncoderBuffer2_resetIndex;

}
void yang_destroy_videoEncoderBuffer2(YangVideoEncoderBuffer2* buf){
	YangMediaBuffer2* mb=&buf->mediaBuffer;
	yang_destroy_mediaBuffer2(mb);
}

