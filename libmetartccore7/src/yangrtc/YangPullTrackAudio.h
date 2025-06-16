//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRECVTRACKAUDIO_H_
#define SRC_YANGRTC_YANGRECVTRACKAUDIO_H_
#include <yangrtc/YangRtcStream.h>
#if Yang_Enable_RTC_Audio
void yang_create_pullTrackAudio(YangRtcContext* context,YangPullTrackAudio* track);
void yang_destroy_pullTrackAudio(YangPullTrackAudio* track);
#endif
#endif /* SRC_YANGRTC_YANGRECVTRACKAUDIO_H_ */
