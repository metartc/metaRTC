//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGWEBRTC_YANGPEERCONNECTION_H_
#define INCLUDE_YANGWEBRTC_YANGPEERCONNECTION_H_

#include <yangwebrtc/YangAVContext.h>
typedef struct{
	void* session;
	YangAVContext* avcontext;
	YangStreamConfig* streamconfig;
	void* user;
}YangPeer;
typedef struct {
	YangPeer peer;
	void (*init)(YangPeer* peer);
	int32_t (*initParam)(char* url,YangStreamConfig* stream,YangAVContext* context,YangStreamOptType opt);
	void  (*parseHeader)(YangVideoCodec codec,uint8_t *buf, uint8_t *src, int32_t *hLen);
	int32_t (*connectServer)(YangPeer* peer);
	int32_t (*disconnectServer)(YangPeer* peer);
	int32_t (*publishAudio)(YangPeer* peer,YangFrame* audioFrame);
	int32_t (*publishVideo)(YangPeer* peer,YangFrame* videoFrame);
	int32_t (*getState)(YangPeer* peer);
	int32_t (*recvvideo_notify)(YangPeer* peer, YangRtcMessageType mess);
}YangPeerConnection;
void yang_init_peerConnection(YangPeerConnection* peerconn);
void yang_destroy_peerConnection(YangPeerConnection* peerconn);




#endif /* INCLUDE_YANGWEBRTC_YANGPEERCONNECTION_H_ */
