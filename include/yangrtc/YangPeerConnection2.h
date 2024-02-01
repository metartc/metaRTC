//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGRTC_YANGPEERCONNECTION2_H_
#define INCLUDE_YANGRTC_YANGPEERCONNECTION2_H_
#include <yangrtc/YangPeerConnection.h>
class YangPeerConnection2 {
public:
	YangPeerConnection2(YangAVInfo* avinfo,YangStreamConfig* streamConfig);
	virtual ~YangPeerConnection2();
	YangStreamConfig* streamConfig;
public:
	void init();

    int32_t addAudioTrack(YangAudioCodec codec);
    int32_t addVideoTrack(YangVideoCodec codec);

    int32_t addTransceiver(YangRtcDirection direction);

	int32_t createOffer( char **psdp);
	int32_t createAnswer(char* answer);
	int32_t createHttpAnswer(char* answer);
	int32_t createDataChannel();
	int32_t setRemoteDescription(char* sdp);
	int32_t connectSfuServer();//srs zlm
	int32_t connectWhipServer(char* url);

	int32_t close();

	int32_t isAlive();
	int32_t isConnected();

	int32_t on_audio(YangFrame* audioFrame);
	int32_t on_video(YangFrame* videoFrame);
	int32_t on_message(YangFrame* msgFrame);

	int32_t sendRtcMessage( YangRtcMessageType mess);

private:
	YangPeerConnection m_conn;
};

#endif /* INCLUDE_YANGRTC_YANGPEERCONNECTION2_H_ */
