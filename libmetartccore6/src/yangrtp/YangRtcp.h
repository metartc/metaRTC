//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGWEBRTC_YANGRTCRTCP_H_
#define YANGWEBRTC_YANGRTCRTCP_H_

#include <yangutil/yangtype.h>
#include <yangutil/buffer/YangBuffer.h>

#define Yang_RtcpHeader_Length 4
// RTCP Packet Types, @see http://www.networksorcery.com/enp/protocol/rtcp.htm
typedef enum
{
    YangRtcpType_fir = 192,
    YangRtcpType_sr = 200,
    YangRtcpType_rr = 201,
    YangRtcpType_sdes = 202,
    YangRtcpType_bye = 203,
    YangRtcpType_app = 204,
    YangRtcpType_rtpfb = 205,
    YangRtcpType_psfb = 206,
    YangRtcpType_xr = 207,
}YangRtcpType;



// RTCP Header, @see http://tools.ietf.org/html/rfc3550#section-6.1
// @remark The header must be 4 bytes, which align with the max field size 2B.
typedef struct
{
	uint8_t version;
	uint8_t padding;
	uint8_t rc;
	uint8_t type;
	uint16_t length:16;
}YangRtcpHeader;


typedef struct{
	 uint64_t ntp;
	 uint32_t rtp_ts;
	 uint32_t send_rtp_packets;
	 uint32_t send_rtp_bytes;
}YangRtcpSR;

typedef struct
{
    uint32_t ssrc;
    uint8_t fraction_lost;
    uint32_t lost_packets;
    uint32_t highest_sn;
    uint32_t jitter;
    uint32_t lsr;
    uint32_t dlsr;
}YangRtcpRB;


typedef struct{
	 uint32_t mediaSsrc;
	 uint32_t vsize;
	 uint32_t capacity;
	 uint16_t* nacks;

}YangRtcpNack;
typedef struct{
    uint8_t pb;
    uint8_t payload_type;
    uint32_t mediaSsrc;
    int32_t nb_native_rpsi;
    char* native_rpsi;
}YangRtcpRpsi;
typedef struct{
	uint16_t first;
	uint16_t number;
	uint8_t picture;
}YangRtcpSli;
typedef struct{
    uint32_t ssrc;
    uint32_t mediaSsrc;
    int32_t payloadLen;
    int32_t nb_data;
    char* data;
    YangRtcpRB* rb;
    YangRtcpSR* sr;
    YangRtcpNack *nack;
    YangRtcpRpsi* rpsi;
    YangRtcpSli* sli;
    char* appName;
    void* twcc;
    YangRtcpHeader header;
    uint8_t payload[1500];
}YangRtcpCommon;

int32_t yang_decode_rtcpHeader(YangRtcpHeader* header,char* buf);
int32_t yang_encode_rtcpHeader(YangRtcpHeader* header,YangBuffer* buf);
#endif /* YANGWEBRTC_YANGRTCRTCP_H_ */
