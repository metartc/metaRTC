//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGPEERCONNECTION7_H_
#define SRC_YANGPEERCONNECTION7_H_

#include <yangrtc/YangPeerInfo.h>
#include <yangrtc/YangMetaConnection.h>
#include <yangrtc/YangPeerConnection.h>
class YangCallbackReceive {
public:
	YangCallbackReceive(){};
	virtual ~YangCallbackReceive(){};
	virtual void receiveAudio(YangFrame* audioFrame)=0;
	//frametype:YANG_Frametype_I YANG_Frametype_P
	virtual void receiveVideo(YangFrame* videoFrame)=0;
	virtual void receiveMsg(YangFrame* msgFrame)=0;
};

class YangCallbackIce{
public:
	YangCallbackIce(){};
	virtual ~YangCallbackIce(){};
	virtual void onIceStateChange(int32_t  uid,YangIceCandidateState iceState)=0;
	virtual void onConnectionStateChange(int32_t  uid,YangRtcConnectionState connectionState)=0;
	virtual void onIceCandidate(int32_t  uid,char* sdp)=0;
	virtual void onIceGatheringState(int32_t  uid,YangIceGatheringState gatherState)=0;
};

class YangCallbackRtc{
public:
	YangCallbackRtc(){};
	virtual ~YangCallbackRtc(){};
	virtual void setMediaConfig(int32_t  uid,YangAudioParam* audio,YangVideoParam* video)=0;
	virtual void sendRequest(int32_t  uid,uint32_t  ssrc,YangRequestType req)=0;
};

class YangCallbackSslAlert{
public:
	YangCallbackSslAlert() {};
	virtual ~YangCallbackSslAlert() {};
	virtual void sslCloseAlert(int32_t  uid)=0;
};


class  YangPeerConnection8 {
public:
	YangPeerConnection8(YangPeerInfo* peerInfo,YangCallbackReceive* receive,YangCallbackIce* ice,YangCallbackRtc* rtc,YangCallbackSslAlert* sslAlert);
	virtual ~YangPeerConnection8();

	YangPeer m_peer;

	int32_t  addAudioTrack(YangAudioCodec codec);
	int32_t  addVideoTrack(YangVideoCodec codec);
	int32_t  addTransceiver(YangMediaTrack media,YangRtcDirection direction);

	int32_t  createOffer( char **psdp);
	int32_t  createAnswer(char* answer);

	int32_t  createDataChannel();

	int32_t  generateCertificate();

	int32_t  setLocalDescription(char* sdp);
	int32_t  setRemoteDescription(char* sdp);

	int32_t  close();

	yangbool isAlive();
	yangbool isConnected();

	int32_t  on_audio(YangPushData* pushData);
	int32_t  on_video(YangPushData* pushData);
	int32_t  on_message(YangFrame* msgFrame);

	int32_t  addIceCandidate(char* candidateStr);
	int32_t  sendRequestPli();
private:
	YangMetaConnection m_conn;
};

class YangRtcPacer{
public:
	YangRtcPacer();
	~YangRtcPacer();
public:
	int32_t reset();
	int32_t initAudio(YangAudioCodec acodec,int32_t sample,int32_t channel);
	int32_t initVideo(YangVideoCodec vcodec,int32_t pktCount);
	int32_t resetAudioTime();
	int32_t resetVideoTime();
	YangPushData* getAudioData(YangFrame* audioFrame);
	YangPushData* getVideoData(YangFrame* videoFrame);
private:
	YangPacer m_pacer;

};


#endif /* SRC_YANGPEERCONNECTION8_H_ */
