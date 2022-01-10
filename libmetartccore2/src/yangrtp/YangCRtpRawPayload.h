/*
 * YangCRtpRawPayload.h
 *
 *  Created on: 2021年12月29日
 *      Author: yang
 */

#ifndef SRC_YANGRTP_YANGCRTPRAWPAYLOAD_H_
#define SRC_YANGRTP_YANGCRTPRAWPAYLOAD_H_

#include <yangutil/buffer/YangBuffer.h>
#include <stdint.h>
typedef struct YangRtpRawData{
	int32_t nb;
	char* payload;
}YangRtpRawData;

int32_t yang_encode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt);
int32_t yang_decode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt);



#endif /* SRC_YANGRTP_YANGCRTPRAWPAYLOAD_H_ */
