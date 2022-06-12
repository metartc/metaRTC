//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGBANDWIDTH_H_
#define SRC_YANGRTC_YANGBANDWIDTH_H_
#include <yangrtc/YangRtcStats.h>
#include <yangutil/sys/YangTwcc.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtp/YangRtcp.h>
#include <yangutil/yangavtype.h>
#define kDefaultLowLossThreshold 0.02f
#define kDefaultHighLossThreshold 0.1f
typedef struct{
	int32_t lastCheckSn;
	int32_t preCheckSn;
	int32_t lossPacketCount;
	YangRequestType request;
	uint64_t lastPacketCount;
	double lostRate;

}YangBandwidth;
void yang_insert_twcc_local(YangTwcc* twcc,uint16_t sn);
int32_t yang_twcc_decode(YangTwcc* twcc,YangRtcpCommon* comm);
int32_t yang_bandwidth_estimate(YangTwcc* twcc);
int32_t yang_bandwidth_checkByTwcc(YangTwcc* twcc,YangBandwidth* bw);
int32_t yang_bandwidth_checkByRR(YangRemoteRecvStats* stats,YangBandwidth* bw);
#endif /* SRC_YANGRTC_YANGBANDWIDTH_H_ */
