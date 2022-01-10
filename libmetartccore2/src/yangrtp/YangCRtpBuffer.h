/*
 * YangCRtpBuffer.h
 *
 *  Created on: 2021年12月29日
 *      Author: yang
 */

#ifndef SRC_YANGRTP_YANGCRTPBUFFER_H_
#define SRC_YANGRTP_YANGCRTPBUFFER_H_

#include <stdint.h>
typedef struct YangRtpBuffer{
	char* buffer;
	int32_t bufLen;
	int32_t index;
}YangRtpBuffer;
void yang_init_rtpBuffer(YangRtpBuffer* rtpbuf,int pbuflen,int unitsize);
void yang_destroy_rtpBuffer(YangRtpBuffer* rtpbuf);
char* yang_get_rtpBuffer(YangRtpBuffer* rtpbuf);




#endif /* SRC_YANGRTP_YANGCRTPBUFFER_H_ */
