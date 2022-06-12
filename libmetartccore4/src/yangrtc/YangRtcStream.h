//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGRTCSTREAM_H_
#define SRC_YANGRTC_YANGRTCSTREAM_H_
#include <yangutil/sys/YangTime.h>
#include <stdint.h>
#include <yangrtp/YangPublishNackBuffer.h>
#include <yangrtp/YangReceiveNackBuffer.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRecvNack.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtcpTWCC.h>



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
} YangRecvTrack;
typedef struct  {
	bool in_use;
	bool end;
	uint16_t sn;
	uint32_t ts;
	int32_t nalu_type;
	int32_t nb;
	Fua2H264DataCache fua2;
	char *payload;
} YangH264PacketCache;
typedef struct  {
	uint16_t header_sn;
	uint16_t lost_sn;
	int32_t hasReceiveStap;
	int32_t uid;
	int32_t hasRequestKeyframe;
	int64_t key_frame_ts;

	YangRtpSTAPData stapData;
	YangBuffer buf;
	YangFrame videoFrame;
	char *video_buffer;
	YangH264PacketCache cache_video_pkts[YANG_RTC_RECV_BUFFER_COUNT];

} YangH264RecvTrack;

typedef struct  {
	bool in_use;
	bool end;
	uint16_t sn;
	uint32_t ts;
	int32_t nalu_type;
	int32_t nb;
	Fua2H265DataCache fua2;
	char *payload;
} YangH265PacketCache;

typedef struct  {
	uint16_t header_sn;
	uint16_t lost_sn;
	int32_t hasReceiveStap;
	int64_t key_frame_ts;

	int32_t uid;
	//uint32_t ssrc;
	uint32_t hasRequestKeyframe;

	char *video_buffer;
	YangRtpSTAPData stapData;
	YangBuffer buf;
	YangFrame videoFrame;

	YangH265PacketCache cache_video_pkts[YANG_RTC_RECV_BUFFER_COUNT];
} YangH265RecvTrack;

typedef struct  {
	YangRecvTrack recv;
	YangH264RecvTrack *h264Track;
	YangH265RecvTrack *h265Track;
} YangVideoRecvTrack;
typedef struct  {
	uint32_t audioSsrc;
	uint32_t videoSsrc;
	int32_t mw_msgs;
	int16_t realtime;

	YangPublishNackBuffer *audio_queue;
	YangPublishNackBuffer *video_queue;
} YangRtcPublishStream;


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
	YangRecvTrack *audioTrack;
	YangVideoRecvTrack *videoTrack;

} YangRtcPlayStream;

#endif /* SRC_YANGRTC_YANGRTCSTREAM_H_ */
