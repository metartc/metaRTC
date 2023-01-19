//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangrtp/YangRtpBuffer.h>
#include <yangrtp/YangRtpConstant.h>

void yang_create_rtpBuffer(YangRtpBuffer* rtpbuf,int pbuflen,int unitsize){
	if(rtpbuf==NULL) return;
	rtpbuf->bufLen=pbuflen;
	rtpbuf->index=0;
	rtpbuf->buffer=(char*)yang_calloc(unitsize*rtpbuf->bufLen,1);
}
void yang_destroy_rtpBuffer(YangRtpBuffer* rtpbuf){
	if(rtpbuf==NULL) return;
	yang_free(rtpbuf->buffer);
}
char* yang_get_rtpBuffer(YangRtpBuffer* rtpbuf){
	if(rtpbuf==NULL) return NULL;
	rtpbuf->index++;
	if(rtpbuf->index>=rtpbuf->bufLen) rtpbuf->index=0;
	return rtpbuf->buffer+kRtpPacketSize*rtpbuf->index;
}



