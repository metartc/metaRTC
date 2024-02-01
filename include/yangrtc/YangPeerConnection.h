//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGRTC_YANGPEERCONNECTION_H_
#define INCLUDE_YANGRTC_YANGPEERCONNECTION_H_
#include <yangutil/yangavinfotype.h>

typedef struct{
	void* conn;
	YangAVInfo* avinfo;
	YangStreamConfig streamconfig;
}YangPeer;

typedef struct {
	YangPeer peer;
	void (*init)(YangPeer* peer);

	int32_t (*addAudioTrack)(YangPeer* peer,YangAudioCodec codec);
	int32_t (*addVideoTrack)(YangPeer* peer,YangVideoCodec codec);

	int32_t (*addTransceiver)(YangPeer* peer,YangRtcDirection direction);
	YangIceCandidateType (*getIceCandidateType)(YangPeer* peer);

	int32_t (*createOffer)(YangPeer* peer, char **psdp);
	int32_t (*createAnswer)(YangPeer* peer,char* answer);
	int32_t (*createHttpAnswer)(YangPeer* peer,char* answer);
	int32_t (*createDataChannel)(YangPeer* peer);

	int32_t (*setLocalDescription)(YangPeer* peer,char* sdp);
	int32_t (*setRemoteDescription)(YangPeer* peer,char* sdp);
	int32_t (*connectSfuServer)(YangPeer* peer);//srs zlm
	int32_t (*connectWhipWhepServer)(YangPeer* peer,char* url);
	int32_t (*close)(YangPeer* peer);

	yangbool (*isAlive)(YangPeer* peer);
	yangbool (*isConnected)(YangPeer* peer);
	YangRtcConnectionState (*getConnectionState)(YangPeer* peer);

	int32_t (*on_audio)(YangPeer* peer,YangFrame* audioFrame);
	int32_t (*on_video)(YangPeer* peer,YangFrame* videoFrame);
	int32_t (*on_message)(YangPeer* peer,YangFrame* msgFrame);

	int32_t (*sendRtcMessage)(YangPeer* peer, YangRtcMessageType mess);
}YangPeerConnection;


#ifdef __cplusplus
extern "C"{
#endif
void yang_create_peerConnection(YangPeerConnection* peerconn);
void yang_destroy_peerConnection(YangPeerConnection* peerconn);

int32_t yang_p2p_getHttpSdp(YangIpFamilyType familyType,char* httpIp,int32_t httpPort,char* localSdp,char* remoteSdp);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGRTC_YANGPEERCONNECTION_H_ */
