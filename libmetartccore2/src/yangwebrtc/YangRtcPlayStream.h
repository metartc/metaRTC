//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGWEBRTC_YANGRTCPLAYSTREAM_H_
#define YANGWEBRTC_YANGRTCPLAYSTREAM_H_

#include <yangwebrtc/YangRtcContext.h>

#define Yang_Using_RtpExtension 0
#ifdef __cplusplus
extern "C"{
#endif

void yang_init_rtcplay(YangRtcContext *context, YangRtcPlayStream* play,	YangRtpBuffer *rtpBuffer);
void yang_destroy_rtcplay(YangRtcPlayStream* play);

void yang_rtcplay_update_rtt(YangRtcContext *context, YangRtcPlayStream* play,uint32_t ssrc, int32_t rtt);
int32_t yang_rtcplay_on_rtcp(YangRtcContext *context, YangRtcPlayStream* play,YangRtcpCommon *rtcp);
int32_t yang_rtcplay_on_rtp(YangRtcContext *context, YangRtcPlayStream* play,char *data, int32_t nb_data) ;
int32_t yang_rtcplay_on_rtcp_sr(YangRtcContext *context, YangRtcPlayStream* play,YangRtcpCommon *rtcp) ;
YangRecvTrack* yang_rtcplay_get_audio_track(YangRtcContext *context, YangRtcPlayStream* play,uint32_t ssrc);
YangVideoRecvTrack* yang_rtcplay_get_video_track(YangRtcContext *context, YangRtcPlayStream* play,uint32_t ssrc);
int32_t yang_rtcplay_on_rtcp_xr(YangRtcContext *context, YangRtcPlayStream* play,YangRtcpCommon *rtcp);
void yang_rtcplay_update_send_report_time(YangRtcContext *context, YangRtcPlayStream* play,uint32_t ssrc,
		 YangNtp *ntp, uint32_t rtp_time);
int32_t yang_rtcplay_send_rtcp_xr_rrtr(YangRtcContext *context, YangRtcPlayStream* play);
int32_t yang_rtcplay_send_rtcp_rr(YangRtcContext *context, YangRtcPlayStream* play);
int32_t yang_rtcplay_send_periodic_twcc(YangRtcContext *context, YangRtcPlayStream* play);
#ifdef __cplusplus
}
#endif
#endif /* YANGWEBRTC_YANGRTCPLAYSTREAM_H_ */
