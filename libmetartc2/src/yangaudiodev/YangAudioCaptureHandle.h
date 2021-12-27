#ifndef ___YangAudioCaptureHandle__
#define ___YangAudioCaptureHandle__
//#ifdef _WIN32
#include <yangavutil/audio/YangAecBase.h>

#include <yangutil/yangavinfotype.h>
#include <yangutil/buffer/YangAudioBuffer.h>

class YangAudioCaptureHandle {
public:
    YangAudioCaptureHandle(YangContext *pcontext);
	virtual ~YangAudioCaptureHandle(void);
	YangAecBase *m_aec;

public:
	void putBuffer(uint8_t *pBuffer,int32_t plen);
	void putBuffer1(uint8_t *pBuffer,int32_t plen);
	void putEchoBuffer(uint8_t *pBuffer,int32_t plen);
	void putEchoPlay(short *pbuf,int32_t plen);
	void startRecordWave(char *filename);
	void stopRecordWave();
	void setOutAudioBuffer(YangAudioBuffer *plist);
	YangAudioBuffer *m_aecPlayBuffer;
	int32_t isBuf;
private:

	int32_t hasPlayData;
	int32_t m_aecBufferFrames;
	int32_t isFirst;
	short *pcm;
	YangFrame m_audioFrame;
	YangAudioBuffer *m_audioList;
};
//#endif
#endif
