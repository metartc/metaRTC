//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef ___YangAudioCaptureHandle__
#define ___YangAudioCaptureHandle__
//#ifdef _WIN32
#include <yangavutil/audio/YangRtcAec.h>

#include <yangutil/yangavinfotype.h>
#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangavutil/audio/YangAudioUtil.h>
class YangAudioCaptureHandle {
public:
    YangAudioCaptureHandle(YangContext *pcontext);
	virtual ~YangAudioCaptureHandle(void);
    YangRtcAec *m_aec;

public:
	void putBuffer(uint8_t *pBuffer,int32_t plen);
	void putBuffer2(uint8_t *pBuffer,int32_t plen);
	void putEchoBuffer(uint8_t *pBuffer,int32_t plen);
	void putEchoBuffer2(uint8_t *pBuffer,int32_t plen);
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
	int32_t m_audioLen;
	short *pcm;

	YangFrame m_audioFrame;
	YangAudioBuffer *m_audioList;
	YangAudioResample m_resample;
};
//#endif
#endif
