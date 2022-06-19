//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGPLAYTRACKH_H_
#define SRC_YANGRTC_YANGPLAYTRACKH_H_

#include <stdint.h>
#include <yangrtc/YangRtcContext.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangrtc/YangRtcStream.h>
//

#define yang_cache_index(x) 	 x&YANG_RTC_RECV_BUFFER_COUNT2

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

} YangPlayTrackH264;

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
} YangPlayTrackH265;





#endif /* SRC_YANGRTC_YANGPLAYTRACKH_H_ */
