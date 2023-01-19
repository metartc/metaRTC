

//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YangH265RecvTrack_H_
#define SRC_YANGWEBRTC_YangH265RecvTrack_H_


#include <yangrtc/YangPlayTrackH.h>
void yang_create_playTrackH265(YangRtcContext* context,YangPlayTrackH265* track);
void yang_destroy_playTrackH265(YangPlayTrackH265* track);
int32_t yang_playtrackH265_on_rtp(YangRtcContext* context,YangPlayTrackH265* track,YangRtpPacket *src);

#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
