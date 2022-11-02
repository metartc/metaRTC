//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPUSHH264_H_
#define SRC_YANGRTC_YANGPUSHH264_H_
#include <yangrtc/YangPushH.h>

typedef struct{
YangPushH264Rtp *push;
int32_t (*on_video)(void *session,YangPushH264Rtp* rtp,YangFrame* videoFrame);
int32_t (*on_spspps)(void *session,YangPushH264Rtp* rtp,YangFrame* videoFrame);
}YangPushH264;


void yang_create_pushH264(YangPushH264* push,YangRtpBuffer* videoBuffer);
void yang_destroy_pushH264(YangPushH264* push);

#endif /* SRC_YANGRTC_YANGPUSHH264_H_ */
