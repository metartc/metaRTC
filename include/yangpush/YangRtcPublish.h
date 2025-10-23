
//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef SRC_YANGMEETING_INCLUDE_YangRtcPublish_H_
#define SRC_YANGMEETING_INCLUDE_YangRtcPublish_H_
#include <yangrtc/YangPeerConnection7.h>
#include <string>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangThread2.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <vector>
using namespace std;
class YangRtcPublish: public YangThread,public YangCallbackRtc {
public:
	YangRtcPublish(YangContext *pcontext);
	virtual ~YangRtcPublish();
    int32_t init(char* url,yangbool isWhip);

    int32_t connectMediaServer();
    int32_t disConnectMediaServer();

	void setInVideoMetaData(YangVideoMeta *pvmd);
	void setInAudioList(YangAudioEncoderBuffer *pbuf);
	void setInVideoList(YangVideoEncoderBuffer *pbuf);

    void setMediaConfig(int32_t  uid,YangAudioParam* audio,YangVideoParam* video);
    void sendRequest(int32_t  uid,uint32_t  ssrc,YangRequestType req);

	int32_t stopPublishAudioData();
	int32_t stopPublishVideoData();

	int32_t publishMsg(YangFrame* msgFrame);

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
        std::vector<YangPeerConnection7*> m_pushs;
private:
	int32_t m_transType;
	int32_t notifyState;
};

#endif /* SRC_YANGMEETING_INCLUDE_YangRtcPublish_H_ */
