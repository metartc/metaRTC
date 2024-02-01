//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcStun.h>
#include <yangice/YangRtcSocket.h>
#include <yangzlm/YangZlmConnection.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangsdp/YangSdp.h>
#include <yangjson/YangJson.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttp.h>
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
	if(yang_http_post(yangfalse,session->context.avinfo->sys.familyType,sdp,ip, port, purl, (uint8_t*)psdp, yang_strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query zlm sdp failure!");
	}

	YangJsonReader reader;
	char* p=yang_strstr(sdp,"{");
	if(p==NULL) return ERROR_STRING;

	if(yang_create_jsonReader(&reader,p)!=Yang_Ok){
		yang_error("read zlm response json error!");
		err=ERROR_STRING;
		goto cleanup;
	}

	YangJson* jcode = reader.getObjectItemCaseSensitive(reader.session, "code");
	if (reader.isNumber(jcode))
	{
		err=jcode->valueint==0?Yang_Ok:ERROR_SERVER_ConnectFailure;
		if(err!=Yang_Ok) goto cleanup;
	}

	zlm->retcode=jcode->valueint;

	YangJson* sessionid = reader.getObjectItemCaseSensitive(reader.session, "id");
	YangJson* jsdp = reader.getObjectItemCaseSensitive(reader.session, "sdp");


	if (reader.isString(sessionid) && (sessionid->valuestring != NULL))
	{
		zlm->id=(char*)yang_calloc(yang_strlen(sessionid->valuestring)+1,1);
		yang_memcpy(zlm->id,sessionid->valuestring,yang_strlen(sessionid->valuestring));

	}

	if (reader.isString(jsdp) && (jsdp->valuestring != NULL))
	{
		zlm->sdp=(char*)yang_calloc(yang_strlen(jsdp->valuestring)+1,1);
		yang_cstr_replace(jsdp->valuestring,zlm->sdp, "\r\n", "\n");
	}

	cleanup:
	yang_destroy_jsonReader(&reader);
	yang_free(sdp);
	return err;

}


int32_t yang_zlm_doHandleSignal(YangRtcSession* session,ZlmSdpResponseType* zlm,char* sdp,int32_t localport, YangRtcDirection role) {
	int32_t err = Yang_Ok;

	char apiurl[256] ;
	yang_memset(apiurl,0,sizeof(apiurl));

	yang_sprintf(apiurl, "index/api/webrtc?app=%s&stream=%s&type=%s", session->context.streamConfig->app,session->context.streamConfig->stream,role==YangRecvonly?"play":"push");
	err=yang_zlm_query(session,zlm,role==YangRecvonly?1:0,(char*)session->context.streamConfig->remoteIp,session->context.streamConfig->remotePort,apiurl, sdp);

	return err;
}

int32_t yang_zlm_connectRtcServer(YangRtcConnection* conn){
	int err=Yang_Ok;
	ZlmSdpResponseType zlm;
	YangRtcSession* session=conn->session;
	yang_memset(&zlm,0,sizeof(ZlmSdpResponseType));
	char *tsdp=NULL;
	conn->createOffer(session, &tsdp);
	if(tsdp) conn->setLocalDescription(session,tsdp);
    if ((err=yang_zlm_doHandleSignal(session,&zlm,tsdp,session->context.streamConfig->localPort,session->context.streamConfig->direction))  == Yang_Ok) {
		conn->setRemoteDescription(conn->session,zlm.sdp);
	}
	yang_free(tsdp);
	yang_destroy_zlmresponse(&zlm);
    return err;
}
