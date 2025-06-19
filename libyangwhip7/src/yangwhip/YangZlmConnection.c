//
// Copyright (c) 2019-2025 yanggaofeng
//

#include <yangwhip/YangZlmConnection.h>

#include <yangutil/yangtype.h>
#include <yangjson/YangJson.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangUrl.h>
#include <yangutil/sys/YangHttp.h>
#include <yangutil/sys/YangCString.h>

#define Yang_SDP_BUFFERLEN 1024*12

typedef struct{
	int32_t retcode;
	char* sdp;
	char* id;
}ZlmSdpResponseType;

static void yang_destroy_zlmresponse(ZlmSdpResponseType* zlm){
	if(zlm==NULL) return;
	yang_free(zlm->sdp);
	yang_free(zlm->id);
}

static int32_t yang_zlm_query(YangMetaConnection *conn,YangPeer* peer,ZlmSdpResponseType* zlm,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp)
{

	int32_t err=Yang_Ok;

	char *p;
	char* sdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);
	YangJson *jcode,*sessionid,*jsdp;

	YangJsonReader reader;

	if(yang_http_post(yangfalse,peer->peerInfo.familyType,sdp,ip, port, purl, (uint8_t*)psdp, yang_strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query zlm sdp failure!");
	}

	p=yang_strstr(sdp,"{");
	if(p==NULL)
		return ERROR_STRING;

	if(yang_create_jsonReader(&reader,p)!=Yang_Ok){
		yang_error("read zlm response json error!");
		err=ERROR_STRING;
		goto cleanup;
	}

	jcode = reader.getObjectItemCaseSensitive(reader.session, "code");
	if (reader.isNumber(jcode))
	{
		err=jcode->valueint==0?Yang_Ok:ERROR_SERVER_ConnectFailure;
		if(err!=Yang_Ok) goto cleanup;
	}

	zlm->retcode=jcode->valueint;

	sessionid = reader.getObjectItemCaseSensitive(reader.session, "id");
	jsdp = reader.getObjectItemCaseSensitive(reader.session, "sdp");

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


static int32_t yang_zlm_doHandleSignal(YangMetaConnection *conn,YangPeer* peer,char* url,ZlmSdpResponseType* zlm,char* sdp,int32_t localport) {
	int32_t err = Yang_Ok;
	YangRtcDirection role;
	char apiurl[1024] ;
	YangUrlData urlData;
	yang_memset(urlData.server,0,sizeof(urlData.server));
	urlData.port=0;

    if(yang_url_parse(peer->peerInfo.familyType,url,&urlData)){
    	return 1;
    }

	yang_memset(apiurl,0,sizeof(apiurl));
	role=peer->peerInfo.direction;
	yang_sprintf(apiurl, "index/api/webrtc?app=%s&stream=%s&type=%s", urlData.app,urlData.stream,role==YangRecvonly?"play":"push");
	err=yang_zlm_query(conn,peer,zlm,role==YangRecvonly?1:0,urlData.server,urlData.port,apiurl, sdp);

	return err;
}

int32_t yang_zlm_connectRtcServer(YangMetaConnection *conn,YangPeer* peer,char* url){

	int32_t err=Yang_Ok;
	ZlmSdpResponseType zlm;

	char *sdp=NULL;
	char* localSdp=(char*)yang_calloc(1,1024*12);

	yang_memset(&zlm,0,sizeof(ZlmSdpResponseType));

	peer->peerInfo.iceMode = YangIceModeLite;
	conn->createOffer(peer, &sdp);
	yang_cstr_replace(sdp, localSdp, (char*) "\r\n", (char*) "\n");
	conn->setLocalDescription(peer,localSdp);

    if ((err=yang_zlm_doHandleSignal(conn,peer,url,&zlm,localSdp,peer->peerInfo.rtc.rtcLocalPort))  == Yang_Ok) {
		conn->setRemoteDescription(peer,zlm.sdp);
	}

	yang_free(sdp);
	yang_free(localSdp);
	yang_destroy_zlmresponse(&zlm);
    return err;
}
