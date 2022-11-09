//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGUTIL_BUFFER_YANGBUFFER_H_
#define YANGUTIL_BUFFER_YANGBUFFER_H_
#include <yangutil/sys/YangThread.h>
#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>
#define yang_reindex(p)  if(p!=NULL) p->resetIndex();

class YangMediaBuffer {
public:
	YangMediaBuffer();
	virtual ~YangMediaBuffer();
public:
	int32_t m_mediaType;
	int32_t m_uid;
	void resetIndex();
        uint32_t size();
protected:
	uint32_t m_putIndex;
	uint32_t m_getIndex;
	uint32_t m_cache_num;
	uint32_t m_size;
	uint32_t m_nextIndex;
	int32_t m_ret;
	YangFrame** m_frames;
	YangBufferManager* m_bufferManager;
protected:
	void initFrames(int pnum,int unitsize);

	void putFrame(YangFrame* pframe);
	void getFrame(YangFrame* pframe);
	YangFrame* getCurFrameRef();
	int64_t getNextFrameTimestamp();
	uint8_t* getFrameRef(YangFrame* pframe);
private:
	yang_thread_mutex_t m_lock;
};

#endif /* YANGUTIL_BUFFER_YANGBUFFER_H_ */
