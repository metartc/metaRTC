//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTP_YANGRTCPBYE_H_
#define SRC_YANGRTP_YANGRTCPBYE_H_

#include <yangutil/yangtype.h>
#include <yangrtp/YangRtcpCommon.h>

void yang_create_rtcpBye(YangRtcpCommon* comm);
void yang_destroy_rtcpBye(YangRtcpCommon* comm);
int32_t yang_encode_rtcpBye(YangRtcpCommon* comm,YangBuffer* buffer);
int32_t yang_decode_rtcpBye(YangRtcpCommon* comm,YangBuffer* buffer);

#endif /* SRC_YANGRTP_YANGRTCPBYE_H_ */
