#ifndef YangStreamType_YangStreamType_H_
#define YangStreamType_YangStreamType_H_

#include <yangutil/yangavtype.h>











#ifdef __cplusplus
#include <string>

class YangRtcMessageNotify{
public:
	YangRtcMessageNotify(){};
	virtual ~YangRtcMessageNotify(){};
	virtual int notify(int puid,YangRtcMessageType mess)=0;
};

class YangMediaConfigCallback {
public:
	YangMediaConfigCallback() {};
	virtual ~YangMediaConfigCallback() {};
	virtual void setMediaConfig(int32_t puid, YangAudioParam *audio,
			YangVideoParam *video)=0;
};

class YangSendRequestCallback {
public:
	YangSendRequestCallback() {};
	virtual ~YangSendRequestCallback() {};
	virtual void sendRequest(int32_t puid, uint32_t ssrc,
			YangRequestType req)=0;
};
class YangReceiveCallback {
public:
	YangReceiveCallback() {	};
	virtual ~YangReceiveCallback() {};
	virtual void receiveAudio(YangFrame *audioFrame)=0;
	virtual void receiveVideo(YangFrame *videoFrame)=0;
};
#endif

#endif
