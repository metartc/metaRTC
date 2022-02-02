#pragma once
#ifndef __YangAudioEncoderrHandle__
#define __YangAudioEncoderrHandle__
#include "stdint.h"

#include <vector>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#include "yangencoder/YangAudioEncoder.h"
using namespace std;
class YangAudioEncoderHandle:public YangThread,public YangEncoderCallback
{
public:
	YangAudioEncoderHandle(YangAudioInfo *pcontext);
	~YangAudioEncoderHandle(void);

private:
	int32_t m_isInit;

public:
	void init();
	void stop();
	void setInAudioBuffer(YangAudioBuffer *pbuf);
	void setOutAudioBuffer(YangAudioEncoderBuffer *pbuf);

	void onVideoData(YangFrame* pframe);
	void onAudioData(YangFrame* pframe);
	int32_t m_isStart;
	int32_t m_uid;



protected:
	void run();
	void stopLoop();
	void startLoop();

void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:

	YangAudioEncoder *m_enc;
	int32_t m_isConvert;
	YangAudioBuffer *m_in_audioBuffer;
	YangAudioEncoderBuffer *m_out_audioBuffer;
	YangAudioInfo *m_context;
	void Encoder(int32_t isIframe,uint8_t*src,int32_t  p_buflen,uint8_t *dest,int32_t *p_destLen);

};
#endif
