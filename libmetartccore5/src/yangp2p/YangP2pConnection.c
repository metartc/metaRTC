//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangice/YangRtcStun.h>
#include <yangp2p/YangP2pConnection.h>

#include <yangrtc/YangRtcConnection.h>
#include <yangrtc/YangRtcUdp.h>

#include <yangsdp/YangSdp.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttpSocket.h>
typedef struct{
	int32_t retcode;
		char* sdp;
		char* id;
}P2pSdpResponseType;


#define Yang_SDP_BUFFERLEN 1024*12

void yang_destroy_p2presponse(P2pSdpResponseType* p2p){
	if(p2p==NULL) return;
	yang_free(p2p->sdp);
	yang_free(p2p->id);
}

int32_t yang_sdp_queryP2p(P2pSdpResponseType* p2p,char* ip,int32_t port,char* purl, char* psdp)
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

		if ((p = strstr(strs.str[i], "\"code\""))) {
			char *buf = (char*) calloc(1, strlen(p) + 1);
            yang_cstr_replace(p + strlen("\"code\":"), buf, "\"", "");

			p2p->retcode = abs(atoi(buf));
			yang_free(buf);

			if ((err = p2p->retcode) != 0)	break;

			continue;
		}



		if ((p = strstr(strs.str[i], "\"id\""))) {
			p2p->id = (char*) calloc(1, strlen(p) + 1);
			yang_cstr_replace(p + strlen("\"id\" : "), p2p->id, "\"","");
			continue;
		}

		if ((p = strstr(strs.str[i], "\"sdp\""))) {
			char* sdptmp=(char*) calloc(1, strlen(p) + 1);
			p2p->sdp = (char*) calloc(1, strlen(p) + 1);

			yang_cstr_replace(p + strlen("\"sdp\":"), sdptmp, "\"", "");
			yang_cstr_replace(sdptmp, p2p->sdp, "\r", "");

			continue;
		}

	}

	yang_destroy_strings(&strs);
	yang_free(sdp);

	return err;

}


int32_t yang_p2p_doHandleSignal(P2pSdpResponseType* p2p,char* remoteIp,int32_t remotePort,char* localSdp) {
	int32_t err = Yang_Ok;

	char apiurl[256] ;


	memset(apiurl,0,sizeof(apiurl));

	sprintf(apiurl, "rtc/v1/");
	err=yang_sdp_queryP2p(p2p,remoteIp,remotePort,apiurl, localSdp);

	return err;
}

int32_t yang_p2p_connectRtcServer(char* ip,int32_t remotePort,char* localSdp,char* remoteSdp){
	int err=Yang_Ok;

	P2pSdpResponseType sdp;
	memset(&sdp,0,sizeof(P2pSdpResponseType));

	//char *tsdp=NULL;
	//conn->createOffer(session, &tsdp);
//	tsdp[strlen(tsdp)]='}';

    if ((err=yang_p2p_doHandleSignal(&sdp,ip,remotePort,localSdp))  == Yang_Ok) {
    	strcpy(remoteSdp,sdp.sdp);
    	//conn->setRemoteDescription(conn->session,sdp.sdp);
	}
    //yang_free(tsdp);
	yang_destroy_p2presponse(&sdp);
    return err;
}

