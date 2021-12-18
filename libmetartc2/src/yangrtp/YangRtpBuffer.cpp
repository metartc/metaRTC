#include <yangrtp/YangRtpBuffer.h>
#include <yangutil/yangtype.h>
YangRtpBuffer::YangRtpBuffer(int pbuflen,int unitsize){
	m_bufLen=pbuflen;//role==Yang_Stream_Play?1500:20;
	m_index=0;
	m_buffer=new char[unitsize*m_bufLen];

}
YangRtpBuffer::~YangRtpBuffer(){
	yang_deleteA(m_buffer);
}
char* YangRtpBuffer::getBuffer(){
	m_index++;
	if(m_index==m_bufLen) m_index=0;
	return m_buffer+1500*m_index;
}

