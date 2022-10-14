//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPUSHMjpeg_H_
#define SRC_YANGRTC_YANGPUSHMjpeg_H_
#include <yangrtc/YangPushH.h>
#if Yang_Enable_MJpeg_Encoding
typedef struct{
YangPushMjpegRtp *push;
int32_t (*on_video)(void *session,YangPushMjpegRtp* rtp,YangFrame* videoFrame);
int32_t (*on_spspps)(void *session,YangPushMjpegRtp* rtp,YangFrame* videoFrame);
}YangPushMjpeg;


void yang_create_pushMjpeg(YangPushMjpeg* push,YangRtpBuffer* videoBuffer);
void yang_destroy_pushMjpeg(YangPushMjpeg* push);
#endif
#endif /* SRC_YANGRTC_YANGPUSHMjpeg_H_ */
