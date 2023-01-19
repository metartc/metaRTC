//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTCPAPP_H_
#define SRC_YANGRTP_YANGRTCPAPP_H_

#include <yangrtp/YangRtcpCommon.h>
#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangutil/buffer/YangBuffer.h>

void yang_create_rtcpApp(YangRtcpCommon* comm);
void yang_destroy_rtcpApp(YangRtcpCommon* comm);
int32_t yang_encode_rtcpApp(YangRtcpCommon* comm,YangBuffer* buffer);
int32_t yang_decode_rtcpApp(YangRtcpCommon* comm,YangBuffer* buffer);
int32_t yang_rtcpapp_isApp(uint8_t *data, int32_t nb_data);
uint64_t yang_rtcpapp_nb_bytes(YangRtcpCommon* comm);
int32_t yang_rtcpapp_set_payload(YangRtcpCommon* comm,uint8_t* payload, int32_t len);

#endif /* SRC_YANGRTP_YANGRTCPAPP_H_ */
