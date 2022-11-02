

//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK1_H_
#define SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK1_H_

#include <yangrtc/YangRtcStream.h>

#if Yang_Enable_RTC_Video
void yang_create_playvideoTrack(YangRtcContext* context,YangPlayTrackVideo* track);
void yang_destroy_playvideoTrack(YangPlayTrackVideo* track);
#endif

#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
