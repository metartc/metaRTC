//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangRtcStats.h>



void yang_bandwidth_updateEstimate(int64_t now_ms) {

}

int32_t yang_bandwidth_checkByRR(YangBandwidthSession* bw,YangRtcStats* stats){

	int32_t err=Yang_Ok;

	int32_t lostCount=stats->recvStats.lostVideoPacketCount-stats->recvStats.preLostVideoPacketCount;
	int32_t sendCount=stats->sendStats.videoRtpPacketCount-stats->sendStats.preVideoRtpPacketCount;
	if(sendCount>0){
		bw->lostRate=lostCount/sendCount;
	}

	stats->recvStats.preLostVideoPacketCount=stats->recvStats.lostVideoPacketCount;
	stats->sendStats.preVideoRtpPacketCount=stats->sendStats.videoRtpPacketCount;
	return err;
}

int32_t yang_bandwidth_checkByTwcc(YangBandwidthSession* bw,YangTwccSession* twcc){

	int32_t err=Yang_Ok;

	return err;
}

int32_t yang_bandwidth_estimate(YangBandwidthSession* bw,YangTwccSession* twcc){


	return Yang_Ok;
}

void yang_create_bandwidth(YangBandwidth* bw){
	bw->estimate=yang_bandwidth_estimate;
	bw->checkByTwcc=yang_bandwidth_checkByTwcc;
	bw->checkByRR=yang_bandwidth_checkByRR;
}
void yang_destroy_bandwidth(YangBandwidth* bw){

}

