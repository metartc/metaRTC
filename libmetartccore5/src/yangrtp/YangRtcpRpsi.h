//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPRPSI_H_
#define SRC_YANGRTP_YANGRTCPRPSI_H_

#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangutil/buffer/YangBuffer.h>

void yang_create_rtcpRpsi(YangRtcpCommon* comm);
void yang_destroy_rtcpRpsi(YangRtcpCommon* comm);
int32_t yang_decode_rtcpRpsi(YangRtcpCommon* comm,YangBuffer *buffer);
int32_t yang_encode_rtcpRpsi(YangRtcpCommon* comm,YangBuffer *buffer);
uint64_t yang_rtcpRpsi_nb_bytes();

#endif /* SRC_YANGRTP_YANGRTCPRPSI_H_ */
