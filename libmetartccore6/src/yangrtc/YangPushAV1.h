//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPUSHAV1_H_
#define SRC_YANGRTC_YANGPUSHAV1_H_
#include <yangrtc/YangPushH.h>

typedef struct{
YangPushAV1Rtp *push;
int32_t (*on_video)(void *session,YangPushAV1Rtp* rtp,YangFrame* videoFrame);
int32_t (*on_spspps)(void *session,YangPushAV1Rtp* rtp,YangFrame* videoFrame);
}YangPushAV1;


void yang_create_pushAV1(YangPushAV1* push,YangRtpBuffer* videoBuffer);
void yang_destroy_pushAV1(YangPushAV1* push);

#endif /* SRC_YANGRTC_YANGPUSHH264_H_ */
