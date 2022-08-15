//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGRTCSTREAM_H_
#define SRC_YANGRTC_YANGRTCSTREAM_H_
#include <yangutil/sys/YangTime.h>
#include <stdint.h>
#include <yangrtp/YangPublishNackBuffer.h>
#include <yangrtp/YangReceiveNackBuffer.h>

#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRecvNack.h>

#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtc/YangRtcContextH.h>


typedef struct {
	int32_t uid;
	uint16_t last_seq;
	int16_t isAudio;
	// Latest sender report ntp and rtp time.

	int64_t last_sender_report_rtp_time;

	// Prev sender report ntp and rtp time.

	int64_t last_sender_report_rtp_time1;
	//YangNtp last_sender_report_ntp;
	uint64_t last_sender_report_sys_time;

	uint32_t ssrc;

	YangReceiveNackBuffer *rtp_queue;
	YangRtpRecvNack *nack_receiver;
	YangFrame audioFrame;
	YangNtp last_sender_report_ntp;
	YangNtp last_sender_report_ntp1;
} YangPlayTrack;

typedef struct{
	YangPlayTrack session;
	int32_t (*on_rtp)(YangRtcContext *context, YangRtpPacket *ppkt);
	int32_t (*on_nack)(YangRtcContext *context, YangPlayTrack *track,YangRtpPacket *pkt);
	int32_t (*check_nacks)(YangRtcContext *context,YangPlayTrack *audiotrack);

	int32_t (*has_ssrc)(YangRtcContext *context, YangPlayTrack *track,uint32_t ssrc);
	void (*update_rtt)(YangPlayTrack *track, int32_t rtt);
	void (*update_send_report_time)(YangPlayTrack *track, YangNtp *ntp,	uint32_t rtp_time);
	int32_t (*send_rtcp_rr)(YangRtcContext *context,YangPlayTrack *track);
	int32_t (*send_rtcp_xr_rrtr)(YangRtcContext *context,YangPlayTrack *track) ;
}YangPlayTrackAudio;

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

typedef struct  {
	uint32_t audioSsrc;
	uint32_t videoSsrc;
	int32_t mw_msgs;
	int16_t realtime;

	YangPublishNackBuffer *audio_queue;
	YangPublishNackBuffer *video_queue;
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
	YangPlayTrackAudio *audioTrack;
	YangPlayTrackVideo *videoTrack;

} YangRtcPlayStream;




#endif /* SRC_YANGRTC_YANGRTCSTREAM_H_ */
