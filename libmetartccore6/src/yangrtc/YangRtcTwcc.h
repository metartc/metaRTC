//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCTWCC_H_
#define SRC_YANGRTC_YANGRTCTWCC_H_

#include <yangutil/yangtype.h>
#include <yangrtp/YangRtcp.h>

typedef struct {
    uint16_t sn;
    uint16_t packetSize;
    uint32_t isUse;
    uint64_t localTs;
    uint64_t remoteTs;
} YangTwccPacket;


typedef struct {
	uint16_t lastReportedSn;
	uint16_t sn;
	uint64_t packetCount;
	uint64_t lastLocalTs;
    YangTwccPacket twccPackets[YANG_RTC_RECV_BUFFER_COUNT];
} YangTwccSession;

typedef struct{
	YangTwccSession session;
	void (*insertLocal)(YangTwccSession* twcc,uint16_t sn);
	void (*insertRemote)(YangTwccSession* twcc,uint16_t sn,int64_t ts);
	int32_t (*decode)(YangTwccSession* twcc,YangRtcpCommon* comm);
}YangRtcTwcc;

void yang_create_rtctwcc(YangRtcTwcc* twcc);
void yang_destroy_rtctwcc(YangRtcTwcc* twcc);



#endif /* SRC_YANGRTC_YANGRTCTWCC_H_ */
