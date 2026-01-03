//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGRTC_YANGPEERCONNECTION_H_
#define INCLUDE_YANGRTC_YANGPEERCONNECTION_H_
#include <yangutil/yangavinfo.h>


typedef struct {
	YangPeer peer;

	int32_t (*addAudioTrack)(YangPeer* peer,YangAudioCodec codec);
	int32_t (*addVideoTrack)(YangPeer* peer,YangVideoCodec codec);
	int32_t (*addTransceiver)(YangPeer* peer,YangMediaTrack media,YangRtcDirection direction);

	int32_t (*createOffer)(YangPeer* peer, char **psdp);
	int32_t (*createAnswer)(YangPeer* peer,char* answer);
	int32_t (*createDataChannel)(YangPeer* peer);

	int32_t (*setLocalDescription)(YangPeer* peer,char* sdp);
	int32_t (*setRemoteDescription)(YangPeer* peer,char* sdp);

	int32_t (*close)(YangPeer* peer);

	yangbool (*isAlive)(YangPeer* peer);
	yangbool (*isConnected)(YangPeer* peer);

	int32_t  (*on_audio)(YangPeer* peer,YangPushData *audioData);
	int32_t  (*on_video)(YangPeer* peer,YangPushData *videoData);
	int32_t (*on_message)(YangPeer* peer,YangFrame* msgFrame);
	//not implement
	int32_t (*addIceCandidate)(YangPeer* peer,char* candidateStr);
	int32_t (*sendRtcMessage)(YangPeer* peer, YangRtcMessageType mess);
	int32_t  (*sendRequestPli)(YangPeer* peer);

	YangIceCandidateType (*getIceCandidateType)(YangPeer* peer);
	YangRtcConnectionState (*getConnectionState)(YangPeer* peer);

}YangPeerConnection;

typedef struct{
	void* session;
	int32_t (*reset)(void* session);
	int32_t (*initAudio)(void* session,YangAudioCodec acodec,int32_t sample,int32_t channel);
	int32_t (*initVideo)(void* session,YangVideoCodec vcodec,int32_t pktCount);
	int32_t (*resetAudioTime)(void* session);
	int32_t (*resetVideoTime)(void* session);
	YangPushData* (*getAudioData)(void* session,YangFrame* audioFrame);
	YangPushData* (*getVideoData)(void* session,YangFrame* videoFrame);
}YangPacer;

#ifdef __cplusplus
extern "C"{
#endif

void yang_create_peerConnection(YangPeerConnection* peerconn);
void yang_destroy_peerConnection(YangPeerConnection* peerconn);

int32_t yang_create_pacer(YangPacer* pacer);
void yang_destroy_pacer(YangPacer* pacer);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGRTC_YANGPEERCONNECTION_H_ */
