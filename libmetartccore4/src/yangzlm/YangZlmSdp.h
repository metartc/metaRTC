
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRTP_YangZlmSdp_H_
#define YANGRTP_YangZlmSdp_H_
#include <yangrtc/YangRtcSessionH.h>
#include <stdint.h>
typedef struct{
	int32_t retcode;
	char* sdp;
	char* id;
}ZlmSdpResponseType;

void yang_destroy_zlmresponse(ZlmSdpResponseType* zlm);
int32_t yang_zlm_query(YangRtcSession* session,ZlmSdpResponseType* zlm,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp);



#endif
