

//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YangH264RecvTrack_H_
#define SRC_YANGWEBRTC_YangH264RecvTrack_H_


#include <yangrtc/YangPullTrackH.h>

void yang_create_pullTrackH264(YangRtcContext* context,YangPullTrackH264* track);
void yang_destroy_pullTrackH264(YangPullTrackH264* track);
int32_t yang_pulltrackH264_on_rtp(YangRtcContext* context,YangPullTrackH264* track,YangRtpPacket *src);

#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
