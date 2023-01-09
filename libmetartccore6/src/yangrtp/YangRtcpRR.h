//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPRR_H_
#define SRC_YANGRTP_YANGRTCPRR_H_

#include <yangrtp/YangRtcpCommon.h>
#include <yangutil/buffer/YangBuffer.h>

void yang_create_rtcpRR(YangRtcpCommon* comm);
void yang_destroy_rtcpRR(YangRtcpCommon* comm);
int32_t yang_decode_rtcpRR(YangRtcpCommon* comm,YangBuffer *buffer);
int32_t yang_encode_rtcpRR(YangRtcpCommon* comm,YangBuffer *buffer);
uint64_t yang_rtcpRR_nb_bytes(YangRtcpCommon* comm);
void yang_rtcpRR_set_sender_ntp(YangRtcpCommon* comm,uint64_t ntp);
void yang_set_sender_ntp(YangRtcpCommon* comm,uint64_t ntp);
#endif /* SRC_YANGRTP_YANGRTCPRR_H_ */
