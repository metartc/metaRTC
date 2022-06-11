//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGTWCC_H_
#define INCLUDE_YANGUTIL_SYS_YANGTWCC_H_
#include <yangutil/yangtype.h>

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
} YangTwcc;

void yang_insert_twcc_local(YangTwcc* twcc,uint16_t sn);


#endif /* INCLUDE_YANGUTIL_SYS_YANGTWCC_H_ */
