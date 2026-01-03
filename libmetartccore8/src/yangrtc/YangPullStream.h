//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef YANGWEBRTC_YANGRTCPLAYSTREAM_H_
#define YANGWEBRTC_YANGRTCPLAYSTREAM_H_

#include <yangrtc/YangRtcContext.h>
#include <yangrtc/YangPull.h>

void yang_create_rtcpull(YangRtcContext *context, YangRtcPull* play,	YangRtpBuffer *rtpBuffer);
void yang_destroy_rtcpull(YangRtcPull* play);

#endif /* YANGWEBRTC_YANGRTCPLAYSTREAM_H_ */
