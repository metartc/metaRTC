//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPLAYTRACK_H_
#define SRC_YANGRTC_YANGPLAYTRACK_H_
#include <yangrtc/YangPullTrackH.h>

void yang_create_pullTrack(YangRtcContext* context,YangPullTrack* track,int is_audio);
void yang_destroy_pullTrack(YangPullTrack *track);
int32_t yang_pulltrack_do_check_send_nacks(YangRtcContext *context,
		YangPullTrack *track, uint32_t *timeout_nacks);
int32_t yang_pulltrack_on_nack(YangRtcContext* context,YangPullTrack* track,YangRtpPacket *pkt);
int32_t yang_pulltrack_send_rtcp_rr(YangRtcContext* context,yangbool isAudio,YangPullTrack* track);
int32_t yang_pulltrack_send_rtcp_xr_rrtr(YangRtcContext* context,YangPullTrack* track);
int32_t yang_pulltrack_has_ssrc(YangRtcContext* context,YangPullTrack* track,uint32_t ssrc);
void yang_pulltrack_update_rtt(YangPullTrack* track,int32_t rtt);

void yang_pulltrack_update_send_report_time(YangPullTrack* recv, YangNtp* ntp,	uint32_t rtp_time);
void yang_pulltrack_receiveVideo(YangRtcContext *context,YangFrame *videoFrame) ;


#endif /* SRC_YANGRTC_YANGPLAYTRACK_H_ */
