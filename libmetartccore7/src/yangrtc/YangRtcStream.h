//
// Copyright (c) 2019-2022 yanggaofeng
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
} YangPlayTrack;

#if Yang_Enable_RTC_Audio
typedef struct{
	YangPlayTrack session;
	int32_t (*on_rtp)(YangRtcContext *context,YangPlayTrack* session, YangRtpPacket *ppkt);
	int32_t (*on_nack)(YangRtcContext *context, YangPlayTrack *track,YangRtpPacket *pkt);
	int32_t (*check_nacks)(YangRtcContext *context,YangPlayTrack *audiotrack);

	int32_t (*has_ssrc)(YangRtcContext *context, YangPlayTrack *track,uint32_t ssrc);
	void (*update_rtt)(YangPlayTrack *track, int32_t rtt);
	void (*update_send_report_time)(YangPlayTrack *track, YangNtp *ntp,	uint32_t rtp_time);
	int32_t (*send_rtcp_rr)(YangRtcContext *context,YangPlayTrack *track);
	int32_t (*send_rtcp_xr_rrtr)(YangRtcContext *context,YangPlayTrack *track) ;
}YangPlayTrackAudio;
#endif

#if Yang_Enable_RTC_Video
typedef struct  {
	YangPlayTrack track;
	void *h264Track;
	void *h265Track;
} YangPlayTrackSession;

typedef struct{
	YangPlayTrackSession session;
	int32_t (*on_rtp)(YangRtcContext* context,YangPlayTrackSession* track,YangRtpPacket *pkt);
	int32_t (*on_nack)(YangRtcContext *context, YangPlayTrack *track,YangRtpPacket *pkt);
	int32_t (*check_nacks)(YangRtcContext *context,YangPlayTrackSession *videotrack);
	void (*setRequestKeyframeState)(YangPlayTrackSession* track,int32_t state);
	int32_t (*has_ssrc)(YangRtcContext *context, YangPlayTrack *track,uint32_t ssrc);
	void (*update_rtt)(YangPlayTrack *track, int32_t rtt);
	void (*update_send_report_time)(YangPlayTrack *track, YangNtp *ntp,	uint32_t rtp_time);
	int32_t (*send_rtcp_rr)(YangRtcContext *context,YangPlayTrack *track);
	int32_t (*send_rtcp_xr_rrtr)(YangRtcContext *context,YangPlayTrack *track) ;
}YangPlayTrackVideo;
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
	YangPlayTrackAudio *audioTrack;
#endif
#if Yang_Enable_RTC_Video
	YangPlayTrackVideo *videoTrack;
#endif

} YangRtcPlayStream;




#endif /* SRC_YANGRTC_YANGRTCSTREAM_H_ */
