//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRECVTRACKAUDIO_H_
#define SRC_YANGRTC_YANGRECVTRACKAUDIO_H_
#include <yangrtc/YangRtcStream.h>
#if Yang_Enable_RTC_Audio
void yang_create_playTrackAudio(YangRtcContext* context,YangPlayTrackAudio* track);
void yang_destroy_playTrackAudio(YangPlayTrackAudio* track);
#endif
#endif /* SRC_YANGRTC_YANGRECVTRACKAUDIO_H_ */
