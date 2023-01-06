//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPSLI_H_
#define SRC_YANGRTP_YANGRTCPSLI_H_

#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangutil/buffer/YangBuffer.h>

void yang_create_rtcpSli(YangRtcpCommon* comm,uint32_t sender_ssrc);
void yang_destroy_rtcpSli(YangRtcpCommon* comm);
int32_t yang_encode_rtcpSli(YangRtcpCommon* comm,YangBuffer* buffer);
int32_t yang_decode_rtcpSli(YangRtcpCommon* comm,YangBuffer* buffer);


#endif /* SRC_YANGRTP_YANGRTCPSLI_H_ */
