//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcStun.h>
#include <yangice/YangRtcSocket.h>
#include <yangzlm/YangZlmConnection.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangsdp/YangSdp.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttpSocket.h>

#define Yang_SDP_BUFFERLEN 1024*12

typedef struct{
	int32_t retcode;
	char* sdp;
	char* id;
}ZlmSdpResponseType;

void yang_destroy_zlmresponse(ZlmSdpResponseType* zlm){
	if(zlm==NULL) return;
	yang_free(zlm->sdp);
	yang_free(zlm->id);
}

int32_t yang_zlm_query(YangRtcSession* session,ZlmSdpResponseType* zlm,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp)
{

	int32_t err=Yang_Ok;

	char* sdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);
	if(yang_http_post(session->context.avinfo->sys.familyType,sdp,ip, port, purl, (uint8_t*)psdp, yang_strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query zlm sdp failure!");
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
			yang_cstr_replace(p + yang_strlen("\"code\" : "), buf, "\"", "");

			zlm->retcode = abs(atoi(buf));
			yang_free(buf);

			if ((err = zlm->retcode) != 0)	break;

			continue;
		}



		if ((p = yang_strstr(strs.str[i], "\"id\""))) {
			zlm->id = (char*) yang_calloc(1, yang_strlen(p) + 1);
			yang_cstr_replace(p + yang_strlen("\"id\" : "), zlm->id, "\"","");
			continue;
		}

		if ((p = yang_strstr(strs.str[i], "\"sdp\""))) {
			char* sdptmp=(char*) yang_calloc(1, yang_strlen(p) + 1);
			zlm->sdp = (char*) yang_calloc(1, yang_strlen(p) + 1);

            yang_cstr_replace(p + yang_strlen("\"sdp\" : "), sdptmp, "\"", "");
            yang_cstr_replace(sdptmp, zlm->sdp, "\r", "");

			continue;
		}

	}

	yang_destroy_strings(&strs);
	yang_free(sdp);

	return err;

}


int32_t yang_zlm_doHandleSignal(YangRtcSession* session,ZlmSdpResponseType* zlm,char* sdp,int32_t localport, YangStreamOptType role) {
	int32_t err = Yang_Ok;

	char apiurl[256] ;
	yang_memset(apiurl,0,sizeof(apiurl));

	yang_sprintf(apiurl, "index/api/webrtc?app=%s&stream=%s&type=%s", session->context.streamConfig->app,session->context.streamConfig->stream,role==Yang_Stream_Play?"play":"push");
	err=yang_zlm_query(session,zlm,role==Yang_Stream_Play?1:0,(char*)session->context.streamConfig->remoteIp,80,apiurl, sdp);

	return err;
}

int32_t yang_zlm_connectRtcServer(YangRtcConnection* conn){

	int err=Yang_Ok;
	ZlmSdpResponseType zlm;
	YangRtcSession* session=conn->session;
	yang_memset(&zlm,0,sizeof(ZlmSdpResponseType));
	char *tsdp=NULL;
	conn->createOffer(session, &tsdp);
    if ((err=yang_zlm_doHandleSignal(session,&zlm,tsdp,session->context.streamConfig->localPort,session->context.streamConfig->streamOptType))  == Yang_Ok) {

		conn->setRemoteDescription(conn->session,zlm.sdp);
	}
	yang_free(tsdp);
	yang_destroy_zlmresponse(&zlm);
    return err;
}
