//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef INCLUDE_YANGRTC_YANGMETACONNECTION_H_
#define INCLUDE_YANGRTC_YANGMETACONNECTION_H_

#include <yangutil/yangavinfo.h>

typedef struct {
	int32_t  (*addAudioTrack)(YangPeer* peer,YangAudioCodec codec);
	int32_t  (*addVideoTrack)(YangPeer* peer,YangVideoCodec codec);
	int32_t  (*addTransceiver)(YangPeer* peer,YangMediaTrack media,YangRtcDirection direction);

	int32_t  (*createOffer)(YangPeer* peer, char **psdp);
	int32_t  (*createAnswer)(YangPeer* peer,char* answer);

	int32_t  (*createDataChannel)(YangPeer* peer);

	int32_t  (*generateCertificate)(YangPeer* peer);
	int32_t  (*setCertificateFile)(YangPeer* peer,char* pkeyfile,char* certfile);

	int32_t  (*setLocalDescription)(YangPeer* peer,char* sdp);
	int32_t  (*setRemoteDescription)(YangPeer* peer,char* sdp);

	int32_t  (*close)(YangPeer* peer);

	yangbool (*isAlive)(YangPeer* peer);
	yangbool (*isConnected)(YangPeer* peer);

	int32_t  (*on_audio)(YangPeer* peer,YangFrame* audioFrame);
	int32_t  (*on_video)(YangPeer* peer,YangFrame* videoFrame);
	int32_t  (*on_message)(YangPeer* peer,YangFrame* msgFrame);

	int32_t  (*addIceCandidate)(YangPeer* peer,char* candidateStr);
	int32_t  (*sendRtcMessage)(YangPeer* peer, YangRtcMessageType mess);
	int32_t  (*sendRequestPli)(YangPeer* peer);

	YangRtcConnectionState (*getConnectionState)(YangPeer* peer);
	YangIceCandidateType (*getIceCandidateType)(YangPeer* peer);

}YangMetaConnection;


#ifdef __cplusplus
extern "C"{
#endif

void yang_create_peer(YangPeer* peer);
void yang_create_metaConnection(YangMetaConnection* peerconn);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_YANGRTC_YANGMETACONNECTION_H_ */
