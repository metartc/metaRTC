#ifndef SRC_YANGRTP_YANGRTPBUFFER_H_
#define SRC_YANGRTP_YANGRTPBUFFER_H_
#include <stdint.h>
class YangRtpBuffer{
public:
	YangRtpBuffer(int pbuflen,int unitsize=1500);
	~YangRtpBuffer();
public:
	char* getBuffer();
private:
	char* m_buffer;
	int32_t m_bufLen;
	int32_t m_index;

};

#endif /* SRC_YANGRTP_YANGRTPBUFFER_H_ */
