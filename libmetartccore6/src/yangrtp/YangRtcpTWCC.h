//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPTWCC_H_
#define SRC_YANGRTP_YANGRTCPTWCC_H_

#include <yangrtp/YangRtp.h>
#include <yangrtc/YangRtcTwcc.h>
#include <yangrtp/YangRtcpCommon.h>
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
	uint16_t isUse;
	int64_t ts;
}YangRtpTwcc;




//yang_vector_declare(YangRtpTwcc)

yang_vector_declare(uint16_t)

typedef struct{
    uint8_t fb_pkt_count;
    uint8_t isSendtwcc;
    uint16_t newSnsIndex;
    uint16_t newRecvSn;
    uint16_t lastSn;
    uint16_t baseSn;
	uint16_t nextBaseSn;

	uint32_t ssrc;
    uint32_t mediaSsrc;
	int32_t pktLen;
    int32_t referenceTime;
    YangRtcpHeader header;

    int64_t tss[YANG_RTC_RECV_BUFFER_COUNT];
    YangUint16Sort sns;
    uint16_tVector encodedChucks;
    uint16_tVector pktDeltas;
}YangRecvTWCC;
typedef struct{
    uint8_t deltaSizes[kTwccFbMaxBitElements];
    uint16_t size;
    yangbool all_same;
    yangbool has_large_delta;

}YangRtcpTWCCChunk;
void yang_create_recvtwcc(YangRecvTWCC* twcc);
void yang_destroy_recvtwcc(YangRecvTWCC* twcc);
void yang_create_rtcptwcc(YangRtcpCommon* comm);
void yang_destroy_rtcptwcc(YangRtcpCommon* comm);
int32_t yang_twcc_rtcp_decode(YangRtcpCommon* rtcp,YangBuffer *buffer);
int32_t yang_twcc_recv_decode(YangRecvTWCC* twcc,YangBuffer *buffer);
int32_t yang_twcc_recv_encode(YangRecvTWCC* twcc,YangBuffer *buffer);


#endif /* SRC_YANGRTP_YANGRTCPTWCC_H_ */
