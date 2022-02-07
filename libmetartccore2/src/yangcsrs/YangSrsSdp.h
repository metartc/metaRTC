//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRTP_YangSrsSdp_H_
#define YANGRTP_YangSrsSdp_H_
#include <yangwebrtc/YangRtcSessionH.h>
#include <stdint.h>
typedef struct{
	int32_t retcode;
	char* serverIp;
	char* sdp;
	char* sessionid;
}SrsSdpResponseType;

void yang_destroy_srsresponse(SrsSdpResponseType* srs);
int32_t yang_sdp_querySrs(YangRtcSession* session,SrsSdpResponseType* srs,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp);



#endif
