/*
 * InvokeList.h
 *
 *  Created on: 2019年9月27日
 *      Author: yang
 */

#ifndef YANGRTMP_IMPL_YANGINVOKELIST_H_
#define YANGRTMP_IMPL_YANGINVOKELIST_H_
#include "YangRtmpBase.h"
#include "stdint.h"


class YangInvokeBuffer {
public:
	YangInvokeBuffer();
	virtual ~YangInvokeBuffer();
	//int32_t start;
	//int32_t first;
    int32_t index;
	int32_t newIndex;
	int32_t size;
	int32_t pac_len;
	void reset();
	void resetIndex();
	char *cache,*tmp,*tmp1;
	int32_t a_len,m_cache_num;//,m_cache_num_cap;
	void putPacket(RTMPPacket *pac);
	void getPacket(RTMPPacket *pac);


};

#endif /* YANGRTMP_IMPL_YANGINVOKELIST_H_ */
