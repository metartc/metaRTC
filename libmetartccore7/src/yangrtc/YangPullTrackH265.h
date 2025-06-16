

//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YangH265RecvTrack_H_
#define SRC_YANGWEBRTC_YangH265RecvTrack_H_


#include <yangrtc/YangPullTrackH.h>
void yang_create_pullTrackH265(YangRtcContext* context,YangPullTrackH265* track);
void yang_destroy_pullTrackH265(YangPullTrackH265* track);
int32_t yang_pulltrackH265_on_rtp(YangRtcContext* context,YangPullTrackH265* track,YangRtpPacket *src);

#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
