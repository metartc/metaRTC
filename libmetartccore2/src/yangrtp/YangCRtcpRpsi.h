/*
 * YangCRtcpRpsi.h
 *
 *  Created on: 2022年1月2日
 *      Author: yang
 */

#ifndef SRC_YANGRTP_YANGCRTCPRPSI_H_
#define SRC_YANGRTP_YANGCRTCPRPSI_H_

#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangutil/buffer/YangBuffer.h>

void yang_init_rtcpRpsi(YangRtcpCommon* comm);
void yang_destroy_rtcpRpsi(YangRtcpCommon* comm);
int32_t yang_decode_rtcpRpsi(YangRtcpCommon* comm,YangBuffer *buffer);
int32_t yang_encode_rtcpRpsi(YangRtcpCommon* comm,YangBuffer *buffer);
uint64_t yang_rtcpRpsi_nb_bytes();




#endif /* SRC_YANGRTP_YANGCRTCPRPSI_H_ */
