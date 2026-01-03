//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_BUFFER_YANGBUFFERMANAGER_H_
#define INCLUDE_YANGUTIL_BUFFER_YANGBUFFERMANAGER_H_
#include <yangutil/yangtype.h>


#ifdef __cplusplus
extern "C"{
#include <yangutil/yangframebuffer.h>
}
#else
#include <yangutil/yangframebuffer.h>
#endif

#ifdef __cplusplus
class YangBufferManager{
public:
	YangBufferManager();
	YangBufferManager(int32_t num,int32_t bufsize);
	virtual ~YangBufferManager();
	void init(int32_t num,int32_t bufsize);
	uint8_t* getBuffer();
private:
	uint8_t* m_cache;
	int32_t m_curindex;
	int32_t m_unitsize;
	int32_t m_size;
};
#endif


#endif /* INCLUDE_YANGUTIL_BUFFER_YANGBUFFERMANAGER_H_ */
