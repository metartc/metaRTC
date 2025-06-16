//
// Copyright (c) 2019-2025 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGRTCSTREAM_H_
#define SRC_YANGRTC_YANGRTCSTREAM_H_

#include <yangutil/sys/YangTime.h>
#include <yangrtp/YangPublishNackBuffer.h>
#include <yangrtp/YangReceiveNackBuffer.h>

#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRecvNack.h>

#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtc/YangRtcContextH.h>
#include <yangutil/buffer2/YangSortBuffer.h>

typedef struct {

	yangbool enableAudioBuffer;
	yangbool isAudio;

	uint16_t last_seq;

	int32_t uid;
	uint32_t ssrc;

	int64_t last_sender_report_rtp_time;
	uint64_t last_sender_report_sys_time;

	YangReceiveNackBuffer *rtp_queue;
	YangRtpRecvNack *nack_receiver;

	YangFrame audioFrame;
	YangNtp last_sender_report_ntp;
	YangSortBuffer* sortBuffer;
} YangPullTrack;

#if Yang_Enable_RTC_Audio
typedef struct{
	YangPullTrack session;
	int32_t (*on_rtp)(YangRtcContext *context,YangPullTrack* session, YangRtpPacket *ppkt);
	int32_t (*on_nack)(YangRtcContext *context, YangPullTrack *track,YangRtpPacket *pkt);
	int32_t (*check_nacks)(YangRtcContext *context,YangPullTrack *audiotrack);

	int32_t (*has_ssrc)(YangRtcContext *context, YangPullTrack *track,uint32_t ssrc);
	void (*update_rtt)(YangPullTrack *track, int32_t rtt);
	void (*update_send_report_time)(YangPullTrack *track, YangNtp *ntp,	uint32_t rtp_time);
	int32_t (*send_rtcp_rr)(YangRtcContext *context,YangPullTrack *track);
	int32_t (*send_rtcp_xr_rrtr)(YangRtcContext *context,YangPullTrack *track) ;
}YangPullTrackAudio;
#endif

#if Yang_Enable_RTC_Video
typedef struct  {
	YangPullTrack track;
	void *h264Track;
	void *h265Track;
} YangPullTrackSession;

typedef struct{
	YangPullTrackSession session;
	int32_t (*on_rtp)(YangRtcContext* context,YangPullTrackSession* track,YangRtpPacket *pkt);
	int32_t (*on_nack)(YangRtcContext *context, YangPullTrack *track,YangRtpPacket *pkt);
	int32_t (*check_nacks)(YangRtcContext *context,YangPullTrackSession *videotrack);
	void (*setRequestKeyframeState)(YangPullTrackSession* track,int32_t state);
	int32_t (*has_ssrc)(YangRtcContext *context, YangPullTrack *track,uint32_t ssrc);
	void (*update_rtt)(YangPullTrack *track, int32_t rtt);
	void (*update_send_report_time)(YangPullTrack *track, YangNtp *ntp,	uint32_t rtp_time);
	int32_t (*send_rtcp_rr)(YangRtcContext *context,YangPullTrack *track);
	int32_t (*send_rtcp_xr_rrtr)(YangRtcContext *context,YangPullTrack *track) ;
}YangPullTrackVideo;
#endif
typedef struct  {
	uint32_t audioSsrc;
	uint32_t videoSsrc;
	int32_t mw_msgs;
#if Yang_Enable_RTC_Audio
	YangPublishNackBuffer *audio_queue;
#endif
#if Yang_Enable_RTC_Video
	YangPublishNackBuffer *video_queue;
#endif

} YangRtcPushStream;


typedef struct  {
	uint8_t request_keyframe;
	uint8_t twccFbCount;
	uint16_t twccEnabled;
	int32_t twccId;

	int64_t last_time_send_twcc;
	YangRecvTWCC* twcc;

	YangRtpBuffer *rtpBuffer;
	YangRtpPacket rtp;
	YangBuffer buf;
#if Yang_Enable_RTC_Audio
	YangPullTrackAudio *audioTrack;
#endif
#if Yang_Enable_RTC_Video
	YangPullTrackVideo *videoTrack;
#endif

} YangRtcPullStream;




#endif /* SRC_YANGRTC_YANGRTCSTREAM_H_ */
