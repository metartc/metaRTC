//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGBANDWIDTH_H_
#define SRC_YANGRTC_YANGBANDWIDTH_H_
#include <yangrtc/YangRtcStats.h>
#include <yangrtc/YangRtcTwcc.h>
#include <yangutil/yangavctype.h>

typedef struct{
	int32_t lastCheckSn;
	int32_t preCheckSn;
	int32_t lossPacketCount;
	YangRequestType request;
	uint64_t lastPacketCount;
	double lostRate;
}YangBandwidthSession;

typedef struct{
	YangBandwidthSession session;
	int32_t (*estimate)(YangBandwidthSession* bw,YangTwccSession* twcc);
	int32_t (*checkByTwcc)(YangBandwidthSession* bw,YangTwccSession* twcc);
	int32_t (*checkByRR)(YangBandwidthSession* bw,YangRtcStats* stats);
}YangBandwidth;

void yang_create_bandwidth(YangBandwidth* bw);
void yang_destroy_bandwidth(YangBandwidth* bw);

#endif /* SRC_YANGRTC_YANGBANDWIDTH_H_ */
