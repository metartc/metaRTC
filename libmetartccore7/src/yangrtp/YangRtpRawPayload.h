//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_
#define SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_

#include <yangutil/buffer/YangBuffer.h>

typedef struct YangRtpRawData{
	int32_t nb;
	char* payload;
}YangRtpRawData;
#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_encode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt);
int32_t yang_decode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt);
#ifdef __cplusplus
}
#endif


#endif /* SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_ */
