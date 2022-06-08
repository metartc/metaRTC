//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPSR_H_
#define SRC_YANGRTP_YANGRTCPSR_H_

#include <yangrtp/YangRtcpCommon.h>

void yang_create_rtcpSR(YangRtcpCommon* comm);
void yang_destroy_rtcpSR(YangRtcpCommon* comm);
int32_t yang_decode_rtcpSR(YangRtcpCommon* comm,YangBuffer *buffer);
int32_t yang_encode_rtcpSR(YangRtcpCommon* comm,YangBuffer *buffer);
uint64_t yang_get_rtcpSR_nb_bytes(YangRtcpCommon* comm);
#endif /* SRC_YANGRTP_YANGRTCPSR_H_ */
