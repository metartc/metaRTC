#ifndef YangStreamType_YangStreamType_H_
#define YangStreamType_YangStreamType_H_

#include <yangutil/yangavtype.h>
#include <string>

enum YangStreamOptType {
	Yang_Stream_Play, Yang_Stream_Publish, Yang_Stream_Both
};

enum YangRequestType {
	Yang_Req_Sendkeyframe, Yang_Req_Connected, Yang_Req_Disconnected
};

enum YangRtcMessageType{
	YangRTC_Decoder_Input
};

struct SrsSdpResponseType {
	std::string retcode;
	std::string serverIp;
	std::string sdp;
	std::string sessionid;
};

struct YangStreamConfig {
	std::string serverIp;
	std::string localIp;
	std::string app;
	std::string stream;
	std::string url;
	int32_t localPort;
	int32_t serverPort;
	int32_t uid;
	YangStreamOptType streamOptType;
};

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
