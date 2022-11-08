//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPLAYTRACK_H_
#define SRC_YANGRTC_YANGPLAYTRACK_H_
#include <yangrtc/YangPlayTrackH.h>

void yang_create_playTrack(YangRtcContext* context,YangPlayTrack* track,int is_audio);
void yang_destroy_playTrack(YangPlayTrack *track);
int32_t yang_playtrack_do_check_send_nacks(YangRtcContext *context,
		YangPlayTrack *track, uint32_t *timeout_nacks);
int32_t yang_playtrack_on_nack(YangRtcContext* context,YangPlayTrack* track,YangRtpPacket *pkt);
int32_t yang_playtrack_send_rtcp_rr(YangRtcContext* context,yangbool isAudio,YangPlayTrack* track);
int32_t yang_playtrack_send_rtcp_xr_rrtr(YangRtcContext* context,YangPlayTrack* track);
int32_t yang_playtrack_has_ssrc(YangRtcContext* context,YangPlayTrack* track,uint32_t ssrc);
void yang_playtrack_update_rtt(YangPlayTrack* track,int32_t rtt);

void yang_playtrack_update_send_report_time(YangPlayTrack* recv, YangNtp* ntp,	uint32_t rtp_time);
void yang_playtrack_receiveVideo(YangRtcContext *context,YangFrame *videoFrame) ;


#endif /* SRC_YANGRTC_YANGPLAYTRACK_H_ */
