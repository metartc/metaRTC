//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangVideoEncoderBuffer2__
#define __YangVideoEncoderBuffer2__

#include <yangutil/buffer2/YangMediaBuffer2.h>
#include <string.h>
typedef struct{
	YangMediaBuffer2 mediaBuffer;
	void (*getEVideo)(YangMediaBuffer2* buf,YangFrame* pframe);
	uint8_t * (*getEVideoRef)(YangMediaBuffer2* buf,YangFrame* frame);
	void (*putEVideo)(YangMediaBuffer2* buf,YangFrame* pframe);
	int32_t (*size)(YangMediaBuffer2* buf);
	void (*resetIndex)(YangMediaBuffer2* buf);
}YangVideoEncoderBuffer2;

void yang_create_videoEncoderBuffer2(YangVideoEncoderBuffer2* buf,int pcachenum);
void yang_destroy_videoEncoderBuffer2(YangVideoEncoderBuffer2* buf);

#endif
