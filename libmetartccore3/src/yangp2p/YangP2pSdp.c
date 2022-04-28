//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangp2p/YangP2pSdp.h>
#include <yangrtc/YangRtcSessionH.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttpSocket.h>

#include <yangsdp/YangRtcSdp.h>
#include <yangsdp/YangSdp.h>

#include <stdio.h>
#include <stdlib.h>
#define Yang_SDP_BUFFERLEN 1024*12

void yang_destroy_p2presponse(P2pSdpResponseType* p2p){
	if(p2p==NULL) return;
	yang_free(p2p->sdp);
	yang_free(p2p->serverIp);
	yang_free(p2p->sessionid);
}

int32_t yang_sdp_queryP2p(YangRtcSession* session,P2pSdpResponseType* p2p,char* ip,int32_t port,char* purl, char* psdp)
{

	int32_t err=Yang_Ok;

	char* sdp=(char*)calloc(1,Yang_SDP_BUFFERLEN);
	if(yang_http_post(sdp,ip, port, purl, (uint8_t*)psdp, strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query p2p sdp failure!");
	}
	char* sBuffer=(char*)calloc(1,Yang_SDP_BUFFERLEN);

	yang_cstr_replace(sdp,sBuffer, "{", "");
	memset(sdp,0,Yang_SDP_BUFFERLEN);
	strcpy(sdp,sBuffer);
	memset(sBuffer,0,Yang_SDP_BUFFERLEN);
	yang_cstr_replace(sdp,sBuffer, "}", "");

	memset(sdp,0,Yang_SDP_BUFFERLEN);
	strcpy(sdp,sBuffer);
	memset(sBuffer,0,Yang_SDP_BUFFERLEN);
	yang_cstr_replace(sdp,sBuffer, "\\r\\n", "\n");

	YangStrings strs;
	memset(&strs,0,sizeof(YangStrings));
	yang_cstr_split(sBuffer, (char*)",",&strs);


	char* p=NULL;
	for (int32_t i = 0; i < strs.vsize; i++) {

		if ((p = strstr(strs.str[i], "\"code\":"))) {
			char *buf = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"code\":"), buf, "\"", "");
			p2p->retcode = atoi(buf);
			yang_free(buf);
			if ((err = p2p->retcode) != 0)	break;

			continue;
		}

		if ((p = strstr(strs.str[i], "\"server\":"))) {
			p2p->serverIp = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"server\":"), p2p->serverIp, "\"", "");

			continue;
		}

		if ((p = strstr(strs.str[i], "\"sessionid\":"))) {

			p2p->sessionid = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"sessionid\":"), p2p->sessionid, "\"","");

			continue;
		}

		if ((p = strstr(strs.str[i], "\"sdp\":"))) {
			p2p->sdp = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"sdp\":"), p2p->sdp, "\"", "");

			continue;
		}

	}



	yang_destroy_strings(&strs);
	yang_free(sdp);

	return err;

}
int32_t yang_p2p_doHandleSignal(YangRtcSession* session,P2pSdpResponseType* srs,int32_t localport, YangStreamOptType role) {
	int32_t err = Yang_Ok;
	char *srsSdp = (char*)calloc(1,1024*12);
	char *tsdp=NULL;


	yang_sdp_genLocalSdp(session,localport, &tsdp,role);

	char qt = '"';
	const char* roleStr=role==Yang_Stream_Play?"play":"publish";
	sprintf(srsSdp,
			"{%capi%c:%chttp://%s:%d/rtc/v1/%s/%c,%cstreamurl%c:%cwebrtc://%s:%d/%s/%s%c,%cclientip%c:null,%csdp%c:%c%s%c}",
			qt, qt, qt, session->context.streamConf->remoteIp, session->context.streamConf->remotePort,roleStr ,qt, qt, qt, qt, session->context.streamConf->remoteIp,
			session->context.streamConf->remotePort, session->context.streamConf->app, session->context.streamConf->stream, qt, qt, qt, qt, qt, qt, tsdp, qt);


	char apiurl[256] ;
	memset(apiurl,0,sizeof(apiurl));

	sprintf(apiurl, "rtc/v1/%s/", roleStr);
	err=yang_sdp_queryP2p(session,srs,(char*)session->context.streamConf->remoteIp,session->context.streamConf->remotePort,apiurl, srsSdp);


	yang_free(srsSdp);
	yang_free(tsdp);

	return err;
}

