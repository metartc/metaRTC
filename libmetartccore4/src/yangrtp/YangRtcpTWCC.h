//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPTWCC_H_
#define SRC_YANGRTP_YANGRTCPTWCC_H_
#include <yangrtp/YangRtcpCommon.h>
#include <yangrtp/YangRtp.h>
#include <yangutil/sys/YangVector.h>
#define kTwccFbPktHeaderSize (4 + 8 + 8)
#define kTwccFbChunkBytes (2)
#define kTwccFbPktFormat (15)
#define kTwccFbPayloadType (205)
#define kTwccFbMaxPktStatusCount (0xffff)
#define kTwccFbDeltaUnit (250)	 // multiple of 250us
#define kTwccFbTimeMultiplier (kTwccFbDeltaUnit * (1 << 8)) // multiplicand multiplier/* 250us -> 64ms  (1 << 8) */
#define kTwccFbReferenceTimeDivisor ((1ll<<24) * kTwccFbTimeMultiplier) // dividend divisor

#define kTwccFbMaxRunLength 		0x1fff
#define kTwccFbOneBitElements 		14
#define kTwccFbTwoBitElements 		7
#define kTwccFbLargeRecvDeltaBytes	2
#define kTwccFbMaxBitElements 		kTwccFbOneBitElements


// The Message format of TWCC, @see https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
//       0                   1                   2                   3
//       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |V=2|P|  FMT=15 |    PT=205     |           length              |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                     SSRC of packet sender                     |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                      SSRC of media source                     |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |      base sequence number     |      packet status count      |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                 reference time                | fb pkt. count |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |          packet chunk         |         packet chunk          |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      .                                                               .
//      .                                                               .
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |         packet chunk          |  recv delta   |  recv delta   |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      .                                                               .
//      .                                                               .
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |           recv delta          |  recv delta   | zero padding  |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct{
	uint16_t sn;
	int64_t ts;
}YangRtpTwcc;

yang_vector_declare(YangRtpTwcc)

yang_vector_declare(uint16_t)

typedef struct{
    uint8_t fb_pkt_count;
    uint16_t base_sn;
	uint16_t next_base_sn;

	uint32_t ssrc;
    uint32_t media_ssrc;
	int32_t pkt_len;
    int32_t reference_time;
    YangRtcpHeader header;
    YangRtpTwccVector twccs;
    uint16_tVector encoded_chucks;
    uint16_tVector pkt_deltas;
}YangRtcpTWCC;
typedef struct{
    uint8_t delta_sizes[kTwccFbMaxBitElements];
    uint16_t size;
    bool all_same;
    bool has_large_delta;

}YangRtcpTWCCChunk;
void yang_create_twcc(YangRtcpTWCC* twcc);
void yang_destroy_twcc(YangRtcpTWCC* twcc);
void yang_create_rtcptwcc(YangRtcpCommon* comm);
void yang_destroy_rtcptwcc(YangRtcpCommon* comm);
int32_t yang_twcc_rtcp_decode(YangRtcpCommon* twcc,YangBuffer *buffer);
int32_t yang_twcc_decode(YangRtcpTWCC* twcc,YangBuffer *buffer);
int32_t yang_twcc_encode(YangRtcpTWCC* twcc,YangBuffer *buffer);


#endif /* SRC_YANGRTP_YANGRTCPTWCC_H_ */
