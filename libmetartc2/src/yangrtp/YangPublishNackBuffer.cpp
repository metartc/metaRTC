#include <yangrtp/YangPublishNackBuffer.h>
#include <yangutil/sys/YangLog.h>
#include <memory.h>
YangPublishNackBuffer::YangPublishNackBuffer(int32_t capacity) {
	  m_capacity = (uint16_t)capacity;
	    m_queue = new YangPublishBuffer*[m_capacity];
	    memset(m_queue, 0, sizeof(YangPublishBuffer*) * capacity);

}

YangPublishNackBuffer::~YangPublishNackBuffer() {
    for (int32_t i = 0; i < m_capacity; ++i) {
    	YangPublishBuffer* pkt = m_queue[i];
        yang_delete(pkt);
    }
    yang_deleteA(m_queue);
}
YangPublishBuffer* YangPublishNackBuffer::at(uint16_t seq) {
    return m_queue[seq % m_capacity];
}

void YangPublishNackBuffer::set(uint16_t at, char* data,int plen)
{
	YangPublishBuffer* p = m_queue[at % m_capacity];
    yang_delete(p);
    if(data){
		YangPublishBuffer *p1=new YangPublishBuffer();
		p1->payload=data;
		p1->nb=plen;
		p1->seq=at;
		m_queue[at % m_capacity] = p1;
    }else{
    	m_queue[at % m_capacity]=NULL;
    }
}

void YangPublishNackBuffer::remove(uint16_t at)
{
    set(at, NULL,0);
}
