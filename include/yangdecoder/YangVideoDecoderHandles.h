#ifndef __YangVideoDecoderHandles__
#define __YangVideoDecoderHandles__
#include "stdint.h"

#include <vector>

#include "yangutil/buffer/YangVideoDecoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#include "YangVideoDecoder.h"
using namespace std;
class YangVideoDecoderHandles:public YangThread,public YangDecoderCallback
{
public:
	YangVideoDecoderHandles(YangContext *pcontext);
	~YangVideoDecoderHandles(void);

private:
	int32_t isInit;

public:
	int32_t m_isStart;
	YangVideoDecoder *getDecoder(int32_t puid);
	YangVideoBuffer* getVideoBuffer(int32_t puid);
	int32_t getDecoderIndex(int32_t puid);
	void parseVideoHeader(uint8_t *p,int32_t pind);
	void addVideoStream(uint8_t *ps,int32_t pind,int32_t pisAdd);
	void removeAllStream();
	//int32_t fx;
	void init();
	void stop();
	void setInVideoBuffer(YangVideoDecoderBuffer *pbuf);
	void setOutVideoBuffer(vector<YangVideoBuffer*>* pbuf);

	void onAudioData(YangFrame* pframe);
	void onVideoData(YangFrame* pframe);




protected:
	void run();
	void stopLoop();
	void startLoop();


void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:
	YangVideoDecoderBuffer *m_in_videoBuffer;
	vector<YangVideoBuffer*> *m_out_videoBuffer;
	vector<YangVideoDecoder*> *m_decs;
	vector<int> m_removeList;
	int32_t m_isConvert;
	//int32_t m_roomState;
	YangContext *m_context;

	void removeStream();
	void decode(int32_t isIframe,uint8_t*src,int32_t  p_buflen,uint8_t *dest,int32_t *p_destLen);


};
#endif
