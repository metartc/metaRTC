//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangVideoEncoderHandle__
#define __YangVideoEncoderHandle__
#include "YangVideoEncoder.h"
#include "stdint.h"

#include <vector>

#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangIni.h"
#include "yangutil/sys/YangThread.h"

using namespace std;
class YangVideoEncoderHandle:public YangThread,public YangEncoderCallback
{
public:
	YangVideoEncoderHandle(YangContext* pcontext,YangVideoInfo* pvideoInfo);
	~YangVideoEncoderHandle(void);

private:
	int32_t m_isInit;

public:
	int32_t m_isStart;
	int32_t m_uid;
	void init();
	void stop();
	void setOutVideoBuffer(YangVideoEncoderBuffer * pvl);
	void setInVideoBuffer(YangVideoBuffer *pvl);


	void setVideoMetaData(YangVideoMeta *pvmd);
	void sendMsgToEncoder(YangRequestType request);
	void onVideoData(YangFrame* pframe);
	void onAudioData(YangFrame* pframe);
protected:
	void run();
	void stopLoop();
	void startLoop();
	void saveFile(char *fileName, uint8_t *pBuffer, long BufferLen);

private:
	YangContext* m_context;
	YangVideoBuffer *m_in_videoBuffer;
	YangVideoEncoderBuffer *m_out_videoBuffer;
	YangVideoMeta *m_vmd;
	int32_t m_isConvert;
	YangVideoInfo *m_videoInfo;
	YangVideoEncInfo *m_encInfo;
	int32_t m_sendMsgToEncoder;
	YangRtcEncoderMessage m_request;

};
#endif
