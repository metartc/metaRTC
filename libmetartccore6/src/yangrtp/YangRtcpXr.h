//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPXR_H_
#define SRC_YANGRTP_YANGRTCPXR_H_

#include <yangrtp/YangRtcpCommon.h>

void yang_create_rtcpXr(YangRtcpCommon* comm);
int32_t yang_decode_rtcpXr(YangRtcpCommon* comm,YangBuffer *buffer);
int32_t yang_encode_rtcpXr(YangRtcpCommon* comm,YangBuffer *buffer);

uint64_t yang_get_rtcpXr_nb_bytes(YangRtcpCommon* comm);


#endif /* SRC_YANGRTP_YANGRTCPXR_H_ */
