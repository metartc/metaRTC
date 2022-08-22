

//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YangH264RecvTrack_H_
#define SRC_YANGWEBRTC_YangH264RecvTrack_H_


#include <yangrtc/YangPlayTrackH.h>

void yang_create_playTrackH264(YangRtcContext* context,YangPlayTrackH264* track);
void yang_destroy_playTrackH264(YangPlayTrackH264* track);
int32_t yang_playtrackH264_on_rtp(YangRtcContext* context,YangPlayTrackH264* track,YangRtpPacket *src);

#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
