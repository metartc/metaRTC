#ifndef ___YangAudioBuffer__
#define ___YangAudioBuffer__
#include <string.h>

#include <yangutil/buffer/YangMediaBuffer.h>
class YangAudioBuffer:public YangMediaBuffer
{
public:
	YangAudioBuffer(int32_t pcacheNum);
	~YangAudioBuffer(void);
	void putAudio(YangFrame* pframe);
	int32_t getAudio(YangFrame* pframe);
	uint8_t *getAudioRef(YangFrame* pframe);
	void reset();

private:

	int32_t m_bufLen;

};
#endif
