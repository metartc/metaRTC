#include <yangrtp/YangCRtpBuffer.h>
#include <yangutil/yangtype.h>
//#include <memory>

void yang_init_rtpBuffer(YangRtpBuffer* rtpbuf,int pbuflen,int unitsize){
	if(rtpbuf==NULL) return;
	rtpbuf->bufLen=pbuflen;//role==Yang_Stream_Play?1500:20;
	rtpbuf->index=0;
	rtpbuf->buffer=(char*)malloc(unitsize*rtpbuf->bufLen);//new char[unitsize*m_bufLen];
}
void yang_destroy_rtpBuffer(YangRtpBuffer* rtpbuf){
	if(rtpbuf==NULL) return;
	yang_free(rtpbuf->buffer);
}
char* yang_get_rtpBuffer(YangRtpBuffer* rtpbuf){
	rtpbuf->index++;
	if(rtpbuf->index==rtpbuf->bufLen) rtpbuf->index=0;
	return rtpbuf->buffer+1500*rtpbuf->index;
}



