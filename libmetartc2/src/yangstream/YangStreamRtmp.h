
#ifndef YANGSTREAM_SRC_YANGSTREAMRTMP_H_
#define YANGSTREAM_SRC_YANGSTREAMRTMP_H_
#include "stdint.h"
#include <yangrtmp/YangRtmpBase.h>
#include <yangstream/YangStreamHandle.h>
class YangStreamRtmp :public YangStreamHandle{
public:
	YangStreamRtmp(int32_t puid,YangContext* pcontext);
	virtual ~YangStreamRtmp();
	int32_t disConnectMediaServer();
		int32_t connectMediaServer();
		int32_t receiveData(int32_t *plen);
		int32_t getConnectState();
		int32_t isconnected();

		//void setRemoteMediaParamCallback(YangMediaConfigCallback* remoteCb);
		//void setSendRequestCallback(YangSendRequestCallback *cb);
	public:
		int32_t sendPmt();
		int32_t reconnect();
		int32_t publishVideoData(YangStreamCapture* videoFrame);
		int32_t publishAudioData(YangStreamCapture* audioFrame);
	private:
		YangRtmpBase *rtmp;
		int32_t rtmpRet;
		char* m_buf;
		int32_t m_bufLen;
		RTMPPacket pac;
		RTMPPacket pac_a;

		YangFrame m_audioFrame;
		YangFrame m_videoFrame;
};

#endif /* YANGSTREAM_SRC_YANGSTREAMRTMP_H_ */
