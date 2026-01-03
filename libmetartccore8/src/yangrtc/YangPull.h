//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPLAY_H_
#define SRC_YANGRTC_YANGPLAY_H_
#include <yangrtc/YangRtcStream.h>

typedef struct{
	YangRtcPullStream* pullStream;
	void (*update_rtt)(YangRtcContext *context, YangRtcPullStream* pullStream,uint32_t ssrc, int32_t rtt);
	int32_t (*on_rtcp)(YangRtcContext *context, YangRtcPullStream* pullStream,YangRtcpCommon *rtcp);
	int32_t (*on_rtp)(YangRtcContext *context, YangRtcPullStream* pullStream,char *data, int32_t nb_data) ;
	int32_t (*on_rtcp_sr)(YangRtcContext *context, YangRtcPullStream* playStream,YangRtcpCommon *rtcp) ;
#if Yang_Enable_RTC_Audio
	YangPullTrackAudio* (*get_audio_track)(YangRtcContext *context, YangRtcPullStream* pullStream,uint32_t ssrc);
#endif
#if Yang_Enable_RTC_Video
	YangPullTrackVideo* (*get_video_track)(YangRtcContext *context, YangRtcPullStream* pullStream,uint32_t ssrc);
#endif
	int32_t (*on_rtcp_xr)(YangRtcContext *context, YangRtcPullStream* pullStream,YangRtcpCommon *rtcp);
	void (*update_send_report_time)(YangRtcContext *context, YangRtcPullStream* pullStream,uint32_t ssrc,
			 YangNtp *ntp, uint32_t rtp_time);
	int32_t (*send_rtcp_xr_rrtr)(YangRtcContext *context, YangRtcPullStream* pullStream);
	int32_t (*send_rtcp_rr)(YangRtcContext *context, YangRtcPullStream* pullStream);
	int32_t (*send_periodic_twcc)(YangRtcContext *context, YangRtcPullStream* pullStream);
}YangRtcPull;


#endif /* SRC_YANGRTC_YANGPLAY_H_ */
