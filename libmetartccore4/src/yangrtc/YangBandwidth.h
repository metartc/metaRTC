//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGBANDWIDTH_H_
#define SRC_YANGRTC_YANGBANDWIDTH_H_
#include <yangutil/sys/YangTwcc.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtp/YangRtcp.h>

int32_t yang_twcc_decode(YangTwcc* twcc,YangRtcpCommon* comm);
int32_t yang_bandwidth_estimate(YangTwcc* twcc);
#endif /* SRC_YANGRTC_YANGBANDWIDTH_H_ */
