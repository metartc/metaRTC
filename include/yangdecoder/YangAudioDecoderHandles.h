
#ifndef __YangAudioDecoderHandles__
#define __YangAudioDecoderHandles__

#include "stdint.h"
#include <vector>
#include <map>
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangAudioPlayBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"

#include "yangdecoder/YangAudioDecoder.h"
using namespace std;
class YangAudioDecoderHandles:public YangThread,public YangDecoderCallback
{
public:
	YangAudioDecoderHandles(YangContext *pcontext);
	~YangAudioDecoderHandles(void);

private:
	int32_t m_isInit;

public:
	YangAudioDecoder *getDecoder(int32_t puid);
	YangAudioPlayBuffer* getAudioBuffer(int32_t puid);
	int32_t getDecoderIndex(int32_t puid);
	void init();
	void stop();
	void setInAudioBuffer(YangAudioEncoderBuffer *pbuf);
	void setOutAudioBuffer(vector<YangAudioPlayBuffer*>* pbuf);
	void removeAudioStream(int32_t puid);
	void removeAllStream();
	int32_t m_isStart;
	void onAudioData(YangFrame* pframe);
	void onVideoData(YangFrame* pframe);
	void setRemoteParam(int32_t puid,YangAudioParam* para);

protected:
	void run();
	void stopLoop();
	void startLoop();

void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:
	std::map<int,YangAudioParam*> m_paramMap;
	vector<YangAudioPlayBuffer*> *m_out_audioBuffer;
	vector<YangAudioDecoder*> *m_decs;
	int32_t m_isConvert;
	YangAudioEncoderBuffer *m_in_audioBuffer;
	YangContext *m_context;

	int32_t m_frameSize,m_channel;
	void decode(int32_t isIframe,uint8_t*src,int32_t  p_buflen,uint8_t *dest,int32_t *p_destLen);

};
#endif
