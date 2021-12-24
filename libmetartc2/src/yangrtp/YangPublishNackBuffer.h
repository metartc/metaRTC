#ifndef SRC_YANGRTP_YANGPUBLISHNACKBUFFER_H_
#define SRC_YANGRTP_YANGPUBLISHNACKBUFFER_H_
#include <stdint.h>
//#include <yangutil/buffer/YangBuffer.h>
struct YangPublishBuffer{
	uint16_t seq;
	int nb;
	char* payload;

};
class YangPublishNackBuffer {
public:
	YangPublishNackBuffer(int32_t capacity);
	virtual ~YangPublishNackBuffer();

	 void set(uint16_t at, char* p,int plen);
	 void remove(uint16_t at);
	 YangPublishBuffer* at(uint16_t seq);
private:
	   uint16_t m_capacity;
	    // Ring bufer.
	   YangPublishBuffer** m_queue;
};

#endif /* SRC_YANGRTP_YANGPUBLISHNACKBUFFER_H_ */
