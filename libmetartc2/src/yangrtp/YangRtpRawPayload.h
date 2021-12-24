#ifndef SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_
#define SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_
#include <yangrtp/YangRtp.h>
struct YangRtpRawData{
	char* payload;
	int32_t nb;
};

int32_t yang_encode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt);
int32_t yang_decode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt);


#endif /* SRC_YANGRTP_YANGRTPRAWPAYLOAD_H_ */
