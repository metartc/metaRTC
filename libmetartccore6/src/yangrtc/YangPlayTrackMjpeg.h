

//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YangMjpegRecvTrack_H_
#define SRC_YANGWEBRTC_YangMjpegRecvTrack_H_


#include <yangrtc/YangPlayTrackH.h>
#if Yang_Enable_MJpeg_Encoding
void yang_create_playTrackMjpeg(YangRtcContext* context,YangPlayTrackMjpeg* track);
void yang_destroy_playTrackMjpeg(YangPlayTrackMjpeg* track);
int32_t yang_playtrackMjpeg_on_rtp(YangRtcContext* context,YangPlayTrackMjpeg* track,YangRtpPacket *src);
#endif
#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
