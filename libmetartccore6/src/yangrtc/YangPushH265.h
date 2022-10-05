//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGPUSHH265_H_
#define SRC_YANGRTC_YANGPUSHH265_H_
#include <yangrtc/YangPushH.h>
typedef struct{
YangPushH265Rtp *push;
int32_t (*on_video)(void *session,YangPushH265Rtp* rtp,YangFrame* videoFrame);
int32_t (*on_spspps)(void *session,YangPushH265Rtp* rtp,YangFrame* videoFrame);
}YangPushH265;

void yang_create_pushH265(YangPushH265* push,YangRtpBuffer* videoBuffer);
void yang_destroy_pushH265(YangPushH265* push);

#endif /* SRC_YANGRTC_YANGPUSHH265_H_ */
