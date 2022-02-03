#ifndef SRC_YANGSTREAM_SRC_YANGSTREAMRTC_H_
#define SRC_YANGSTREAM_SRC_YANGSTREAMRTC_H_
#include <yangstream/YangStreamHandle.h>
#include <yangwebrtc/YangRtcHandle.h>
class YangStreamRtc :public YangStreamHandle{
public:
	YangStreamRtc(int32_t puid,YangContext* pcontext);
	virtual ~YangStreamRtc();
	int32_t disConnectMediaServer();
	int32_t connectMediaServer();
	int32_t receiveData(int32_t *plen);
	int32_t getConnectState();
	int32_t isconnected();

	//void setRemoteMediaParamCallback(YangMediaConfigCallback* remoteCb);
	//void setSendRequestCallback(YangSendRequestCallback *cb);
public:

	int32_t reconnect();
	int32_t publishVideoData(YangStreamCapture* videoFrame);
	int32_t publishAudioData(YangStreamCapture* audioFrame);
private:
	YangRtcHandle *m_rtc;

};

#endif /* SRC_YANGSTREAM_SRC_YANGSTREAMRTC_H_ */
