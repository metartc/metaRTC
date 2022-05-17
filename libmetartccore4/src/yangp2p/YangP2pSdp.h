
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRTP_YangP2pSdp_H_
#define YANGRTP_YangP2pSdp_H_
#include <yangrtc/YangRtcSessionH.h>
#include <stdint.h>
typedef struct{
	int32_t retcode;
		char* sdp;
		char* id;
}P2pSdpResponseType;

void yang_destroy_p2presponse(P2pSdpResponseType* srs);
int32_t yang_sdp_queryP2p(YangRtcSession* session,P2pSdpResponseType* p2p,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp);
int32_t yang_p2p_doHandleSignal(YangRtcSession* session,P2pSdpResponseType* srs,int32_t localport, YangStreamOptType role);


#endif
