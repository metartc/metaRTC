//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTPPACKET_H_
#define SRC_YANGRTP_YANGRTPPACKET_H_

#include <yangrtp/YangRtpBuffer.h>
#include <yangrtp/YangRtpHeader.h>

#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangBuffer.h>

typedef struct YangRtpPacket{
	    int32_t actual_buffer_size;
	    int32_t nalu_type;
	    int32_t frame_type;
	    YangRtpPacketPayloadType payload_type;
	    int32_t nb;
	    int32_t cached_payload_size;  // The cached payload size for packet.
	    char* payload;
	    YangRtpHeader header;
}YangRtpPacket;

#ifdef __cplusplus
extern "C"{
#endif

void yang_create_rtpPacket(YangRtpPacket* pkt);
void yang_destroy_rtpPacket(YangRtpPacket* pkt);
void yang_reset_rtpPacket(YangRtpPacket* pkt);
int32_t yang_decode_rtpPacket(YangRtpPacket* pkt,YangBuffer *buf);
int32_t yang_encode_rtppayload(YangRtpPacket* pkt,YangBuffer *buf);
char* yang_wrap_rtpPacket(YangRtpPacket* pkt,YangRtpBuffer* rtpb,char* data,int32_t nb);
void yang_rtpPacket_set_padding(YangRtpPacket* pkt,int32_t size);
void yang_rtpPacket_add_padding(YangRtpPacket* pkt,int32_t size) ;
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGRTP_YANGRTPPACKET_H_ */
