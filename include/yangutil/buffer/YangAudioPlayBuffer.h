#ifndef ___YangAudioPlayBuffer1__
#define ___YangAudioPlayBuffer1__

#include <yangutil/buffer/YangMediaBuffer.h>


class YangAudioPlayBuffer:public YangMediaBuffer{

public:
	YangAudioPlayBuffer();
	~YangAudioPlayBuffer(void);

	void putAudio(YangFrame* pframe);
	void getAudio(YangFrame* pframe);
	uint8_t *getAudios(YangFrame* pframe);
	int32_t getFrameTimestamp(int64_t *timestamp);
	int64_t getNextTimestamp();
	//int32_t playFrame();
	void reset();
private:
	int32_t m_bufLen;

};
#endif
