//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCSTATS_H_
#define SRC_YANGRTC_YANGRTCSTATS_H_
#include <yangrtp/YangRtpPacket.h>
#include <yangutil/yangtype.h>
typedef struct {
	uint32_t audioRtpPacketCount;
	uint32_t videoRtpPacketCount;

    uint64_t rtt;
    uint64_t totalRtt;
    double fractionLost;
    uint64_t reportsReceived;
    uint64_t rttMeasurements;
    uint64_t lostVideoPacketCount;
    uint64_t preLostVideoPacketCount;

}YangRemoteRecvStats;

typedef struct{
	uint32_t frameCount;
	uint32_t pliCount;
	uint32_t nackCount;
	uint32_t reSenndNackPacketCount;
	uint64_t videoRtpPacketCount;
	uint64_t videoRtpBytes;
	uint64_t audioRtpPacketCount;
	uint64_t audioRtpBytes;
	uint64_t preVideoRtpPacketCount;
	uint64_t preAudioRtpPacketCount;

	uint64_t lastVideoSendTime;
	uint64_t lastAudioSendTime;
	uint64_t audioStartTime;
	uint64_t videoStartTime;
}YangSendStats;

typedef struct{
	YangSendStats sendStats;
	YangRemoteRecvStats recvStats;
	void (*on_videoRtp)(YangSendStats* stats,YangRtpPacket* pkt,YangBuffer* buf);
	void (*on_audioRtp)(YangSendStats* stats,YangRtpPacket* pkt,YangBuffer* buf);
}YangRtcStats;

void yang_create_rtcstats(YangRtcStats* stats);
void yang_destroy_rtcstats(YangRtcStats* stats);

#endif /* SRC_YANGRTC_YANGRTCSTATS_H_ */
