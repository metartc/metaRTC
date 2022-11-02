//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPLAY_H_
#define SRC_YANGRTC_YANGPLAY_H_
#include <yangrtc/YangRtcStream.h>

typedef struct{
	YangRtcPlayStream* playStream;
	void (*update_rtt)(YangRtcContext *context, YangRtcPlayStream* playStream,uint32_t ssrc, int32_t rtt);
	int32_t (*on_rtcp)(YangRtcContext *context, YangRtcPlayStream* playStream,YangRtcpCommon *rtcp);
	int32_t (*on_rtp)(YangRtcContext *context, YangRtcPlayStream* playStream,char *data, int32_t nb_data) ;
	int32_t (*on_rtcp_sr)(YangRtcContext *context, YangRtcPlayStream* playStream,YangRtcpCommon *rtcp) ;
#if Yang_Enable_RTC_Audio
	YangPlayTrackAudio* (*get_audio_track)(YangRtcContext *context, YangRtcPlayStream* playStream,uint32_t ssrc);
#endif
#if Yang_Enable_RTC_Video
	YangPlayTrackVideo* (*get_video_track)(YangRtcContext *context, YangRtcPlayStream* playStream,uint32_t ssrc);
#endif
	int32_t (*on_rtcp_xr)(YangRtcContext *context, YangRtcPlayStream* playStream,YangRtcpCommon *rtcp);
	void (*update_send_report_time)(YangRtcContext *context, YangRtcPlayStream* playStream,uint32_t ssrc,
			 YangNtp *ntp, uint32_t rtp_time);
	int32_t (*send_rtcp_xr_rrtr)(YangRtcContext *context, YangRtcPlayStream* playStream);
	int32_t (*send_rtcp_rr)(YangRtcContext *context, YangRtcPlayStream* playStream);
	int32_t (*send_periodic_twcc)(YangRtcContext *context, YangRtcPlayStream* playStream);
}YangRtcPlay;


#endif /* SRC_YANGRTC_YANGPLAY_H_ */
