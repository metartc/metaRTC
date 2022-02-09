//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRTP_YangRtcHandleImpl_H_
#define YANGRTP_YangRtcHandleImpl_H_

#include <string>
#include <string.h>

#include <yangwebrtc/YangRtcHandle.h>

extern "C"{
#include <yangwebrtc/YangPeerConnection.h>
}

using namespace std;

class YangSrsRtcHandleImpl:public YangRtcHandle,public YangRtcMessageNotify
{
public:
	YangSrsRtcHandleImpl(YangContext* pcontext);
	~YangSrsRtcHandleImpl();
public:
	void init(YangStreamConfig* pconf);

	int32_t connectRtcServer();
	int32_t disconnectServer();

	int32_t publishVideo(YangStreamCapture* videoFrame);
	int32_t publishAudio(YangStreamCapture* audioFrame);
	int32_t getState();
	int notify(int puid,YangRtcMessageType mess);

	YangPeerConnection m_peerconn;
	YangContext* m_conf;
	YangStreamConfig* m_stream;
private:
	int32_t m_isInit;
	YangFrame m_audioFrame,m_videoFrame;


};
#endif
