
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGMEETING_INCLUDE_YangRtcPublish_H_
#define SRC_YANGMEETING_INCLUDE_YangRtcPublish_H_
#include <yangrtc/YangPeerConnection2.h>
#include <string>
#include <yangutil/sys/YangThread2.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <vector>
using namespace std;
class YangRtcPublish: public YangThread {
public:
	YangRtcPublish(YangContext *pcontext);
	virtual ~YangRtcPublish();
        int32_t init(char* url);
	int32_t init(int32_t nettype, char* server, int32_t pport,char* app,char* stream);
	int32_t connectServer(int32_t puid);
	int32_t connectMediaServer();
	int32_t disConnectMediaServer();
	int32_t reconnectMediaServer();
	void setInVideoMetaData(YangVideoMeta *pvmd);
	void setInAudioList(YangAudioEncoderBuffer *pbuf);
	void setInVideoList(YangVideoEncoderBuffer *pbuf);

	int32_t stopPublishAudioData();
	int32_t stopPublishVideoData();

	int32_t publishMsg(YangFrame* msgFrame);
	int32_t receiveMsg(YangFrame* msgFrame);
	int32_t m_netState;
	int32_t isPublished;
	int32_t m_isStart;
	void stop();
protected:
	void run();
	void handleError(int32_t perrCode);
	void startLoop();
	void startLoop_h265();
	YangContext *m_context;
	YangVideoMeta *m_vmd;
	YangVideoEncoderBuffer *m_in_videoBuffer;
	YangAudioEncoderBuffer *m_in_audioBuffer;

	int32_t m_isConvert;
	int32_t m_isInit;
	int32_t m_audioEncoderType;
	std::vector<YangPeerConnection2*> m_pushs;
private:
	int32_t m_transType;
	int32_t notifyState;
};

#endif /* SRC_YANGMEETING_INCLUDE_YangRtcPublish_H_ */
