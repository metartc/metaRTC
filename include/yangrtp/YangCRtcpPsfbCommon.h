/*
 * YangCRtcpPsfbCommon.h
 *
 *  Created on: 2022年1月2日
 *      Author: yang
 */

#ifndef SRC_YANGRTP_YANGCRTCPPSFBCOMMON_H_
#define SRC_YANGRTP_YANGCRTCPPSFBCOMMON_H_

#include <yangrtp/YangRtcpCommon.h>

void yang_init_rtcpPsfb(YangRtcpCommon* comm);
int32_t yang_decode_rtcpPsfb(YangRtcpCommon* comm,YangBuffer *buffer);
int32_t yang_encode_rtcpPsfb(YangRtcpCommon* comm,YangBuffer *buffer);
uint64_t yang_rtcpPsfb_nb_bytes(YangRtcpCommon* comm,YangBuffer *buffer);



#endif /* SRC_YANGRTP_YANGCRTCPPSFBCOMMON_H_ */
