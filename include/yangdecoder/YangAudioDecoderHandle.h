
#ifndef __YangAudioDecoderHandle__
#define __YangAudioDecoderHandle__

#include "stdint.h"
#include <vector>

#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"

#include "yangdecoder/YangAudioDecoder.h"
using namespace std;
class YangAudioDecoderHandle:public YangThread,public YangDecoderCallback,public YangMediaConfigCallback
{
public:
	YangAudioDecoderHandle(YangContext *pcontext);
	~YangAudioDecoderHandle(void);
	int32_t m_isStart;
private:
	int32_t m_isInit;

public:

	YangAudioPlayBuffer* getOutAudioBuffer();
	void setRemoteParam(YangAudioParam* para);
	void init();
	void stop();
	void setInAudioBuffer(YangAudioEncoderBuffer *pbuf);
	void setOutAudioBuffer(YangAudioPlayBuffer* pbuf);
	void removeAudioStream(int32_t puid);
	void removeAllStream();

	void onAudioData(YangFrame* pframe);
	void onVideoData(YangFrame* pframe);
	void setMediaConfig(int32_t puid, YangAudioParam *audio,YangVideoParam *video);

protected:
	void run();
	void stopLoop();
	void startLoop();

void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:
	YangAudioPlayBuffer* m_out_audioBuffer;
	YangAudioDecoder *m_decs;
	int32_t m_isConvert;
	YangAudioEncoderBuffer *m_in_audioBuffer;
	YangContext *m_context;
	YangAudioParam* m_param;

	//int32_t m_channel;
	bool m_is44100;
	uint8_t* m_buf;
	int32_t m_size;
	YangFrame m_audioFrame;
	void decode(int32_t isIframe,uint8_t*src,int32_t  p_buflen,uint8_t *dest,int32_t *p_destLen);

};
#endif
