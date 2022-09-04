//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangVideoDecoderHandle__
#define __YangVideoDecoderHandle__
#include "stdint.h"

#include <vector>

#include "yangutil/buffer/YangVideoDecoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"
#include "YangVideoDecoder.h"
using namespace std;
class YangVideoDecoderHandle:public YangThread,public YangDecoderCallback
{
public:
	YangVideoDecoderHandle(YangContext *pcontext);
	~YangVideoDecoderHandle(void);

private:
	int32_t isInit;

public:
	int32_t m_isStart;
	YangVideoDecoder *getDecoder(int32_t puid);
	YangVideoBuffer* getOutVideoBuffer();

	void parseVideoHeader(uint8_t *p,int32_t pind);
	void addVideoStream(uint8_t *ps,int32_t pind,int32_t pisAdd);

	void init();
	void stop();
	void setInVideoBuffer(YangVideoDecoderBuffer *pbuf);
	void setOutVideoBuffer(YangVideoBuffer* pbuf);

	void onAudioData(YangFrame* pframe);
	void onVideoData(YangFrame* pframe);




protected:
	void run();
	void stopLoop();
	void startLoop();


void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:
	YangVideoDecoderBuffer *m_in_videoBuffer;
	YangVideoBuffer *m_out_videoBuffer;
	YangVideoDecoder *m_decs;

	int32_t m_isConvert;
	YangContext *m_context;

	void decode(int32_t isIframe,uint8_t*src,int32_t  p_buflen,uint8_t *dest,int32_t *p_destLen);


};
#endif
