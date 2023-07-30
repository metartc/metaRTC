//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangice/YangRtcSocket.h>
#include <yangice/YangRtcStun.h>

#include <yangp2p/YangP2pConnection.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangsdp/YangSdp.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttp.h>
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

int32_t yang_sdp_queryP2p(P2pSdpResponseType* p2p,YangIpFamilyType familyType,char* ip,int32_t port,char* purl, char* psdp)
{

	int32_t err=Yang_Ok;

	char* sdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);
	if(yang_http_post(yangfalse,familyType,sdp,ip, port, purl, (uint8_t*)psdp, yang_strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query p2p sdp failure!");
	}

	char* sBuffer=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);

	yang_cstr_replace(sdp,sBuffer, "{", "");
	yang_memset(sdp,0,Yang_SDP_BUFFERLEN);
	yang_strcpy(sdp,sBuffer);
	yang_memset(sBuffer,0,Yang_SDP_BUFFERLEN);
	yang_cstr_replace(sdp,sBuffer, "}", "");

	yang_memset(sdp,0,Yang_SDP_BUFFERLEN);
	yang_strcpy(sdp,sBuffer);
	yang_memset(sBuffer,0,Yang_SDP_BUFFERLEN);
	yang_cstr_replace(sdp,sBuffer, "\\r\\n", "\n");

	YangStrings strs;
	yang_memset(&strs,0,sizeof(YangStrings));
	yang_cstr_split(sBuffer, (char*)",",&strs);


	char* p=NULL;
	for (int32_t i = 0; i < strs.vsize; i++) {

		if ((p = yang_strstr(strs.str[i], "\"code\""))) {
			char *buf = (char*) yang_calloc(1, yang_strlen(p) + 1);
            yang_cstr_replace(p + yang_strlen("\"code\":"), buf, "\"", "");

			p2p->retcode = abs(atoi(buf));
			yang_free(buf);

			if ((err = p2p->retcode) != 0)	break;

			continue;
		}



		if ((p = yang_strstr(strs.str[i], "\"id\""))) {
			p2p->id = (char*) yang_calloc(1, yang_strlen(p) + 1);
			yang_cstr_replace(p + yang_strlen("\"id\" : "), p2p->id, "\"","");
			continue;
		}

		if ((p = yang_strstr(strs.str[i], "\"sdp\""))) {
			char* sdptmp=(char*) yang_calloc(1, yang_strlen(p) + 1);
			p2p->sdp = (char*) yang_calloc(1, yang_strlen(p) + 1);

			yang_cstr_replace(p + yang_strlen("\"sdp\":"), sdptmp, "\"", "");
			yang_cstr_replace(sdptmp, p2p->sdp, "\r", "");

			continue;
		}

	}

	yang_destroy_strings(&strs);
	yang_free(sdp);

	return err;

}


int32_t yang_p2p_doHandleSignal(P2pSdpResponseType* p2p,YangIpFamilyType familyType,char* remoteIp,int32_t remotePort,char* localSdp) {
	int32_t err = Yang_Ok;

	char apiurl[256] ;


	yang_memset(apiurl,0,sizeof(apiurl));

	yang_sprintf(apiurl, "rtc/v1/");
	err=yang_sdp_queryP2p(p2p,familyType,remoteIp,remotePort,apiurl, localSdp);

	return err;
}

int32_t yang_p2p_connectRtcServer(YangIpFamilyType familyType,char* ip,int32_t remotePort,char* localSdp,char* remoteSdp){
	int err=Yang_Ok;

	P2pSdpResponseType sdp;
	yang_memset(&sdp,0,sizeof(P2pSdpResponseType));

	//char *tsdp=NULL;
	//conn->createOffer(session, &tsdp);
//	tsdp[yang_strlen(tsdp)]='}';

    if ((err=yang_p2p_doHandleSignal(&sdp,familyType,ip,remotePort,localSdp))  == Yang_Ok) {
    	yang_strcpy(remoteSdp,sdp.sdp);
    	//conn->setRemoteDescription(conn->session,sdp.sdp);
	}
    //yang_free(tsdp);
	yang_destroy_p2presponse(&sdp);
    return err;
}

