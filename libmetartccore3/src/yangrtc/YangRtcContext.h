//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_structYangRtcContext_H_
#define SRC_YANGWEBRTC_structYangRtcContext_H_

#include <yangrtc/YangRtcContextH.h>
#ifdef __cplusplus
extern "C"{
#endif

void yang_create_rtcContext(YangRtcContext* context);
void yang_destroy_rtcContext(YangRtcContext* context);

int32_t yang_has_ssrc(YangRtcTrack* track,uint32_t ssrc);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGWEBRTC_struct YangRtcContext_H_ */
