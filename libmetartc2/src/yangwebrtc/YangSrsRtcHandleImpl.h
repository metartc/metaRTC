
#ifndef YANGRTP_YangRtcHandleImpl_H_
#define YANGRTP_YangRtcHandleImpl_H_

#include <string>
#include <string.h>
#include <yangwebrtc/YangRtcSession.h>
#include <yangwebrtc/YangUdpHandle.h>
#include <yangwebrtc/YangRtcHandle.h>
#include <yangwebrtc/YangRtcContext.h>
#include <yangwebrtc/YangSrsSdp.h>
using namespace std;

class YangSrsRtcHandleImpl:public YangRtcHandle
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


private:
	YangSrsSdp* m_http;//url_publish;
	YangUdpHandle* m_udp;
	YangRtcSession *m_session;
	YangSdpHandle m_sdpHandle;


	YangRtcContext m_context;
	int32_t m_isInit;


private:
	int32_t handlePlaySdp();
	int32_t createStunPacket(SrsSdpResponseType* prt,YangBuffer *stream,YangStreamOptType role);
	int32_t doHandleSignal(int32_t localport,YangSrsSdp* url,YangStreamOptType role);
};
#endif
