//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGRTP_YANGRTPBUFFER_H_
#define SRC_YANGRTP_YANGRTPBUFFER_H_

#include <yangutil/yangtype.h>

typedef struct YangRtpBuffer{
	char* buffer;
	int32_t bufLen;
	int32_t index;
}YangRtpBuffer;

#ifdef __cplusplus
extern "C"{
#endif
void yang_create_rtpBuffer(YangRtpBuffer* rtpbuf,int pbuflen,int unitsize);
void yang_destroy_rtpBuffer(YangRtpBuffer* rtpbuf);
char* yang_get_rtpBuffer(YangRtpBuffer* rtpbuf);

#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGRTP_YANGRTPBUFFER_H_ */
