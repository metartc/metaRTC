//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTC_YANGRTCSTATS_H_
#define SRC_YANGRTC_YANGRTCSTATS_H_
#include <yangutil/yangtype.h>
typedef struct {

    uint64_t rtt;
    uint64_t totalRtt;
    double fractionLost;
    uint64_t reportsReceived;
    uint64_t rttMeasurements;
    uint64_t lostVideoPacketCount;
    uint64_t sendVideoPacketCount;
    uint64_t preLostVideoPacketCount;
    uint64_t preSendVideoPacketCount;

} YangRemoteRecvStats;

#endif /* SRC_YANGRTC_YANGRTCSTATS_H_ */
