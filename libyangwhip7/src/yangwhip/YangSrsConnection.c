//
// Copyright (c) 2019-2025 yanggaofeng
//

#include <yangwhip/YangSrsConnection.h>

#include <yangjson/YangJson.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangUrl.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttp.h>


typedef struct{
	int32_t retcode;
	char* serverIp;
	char* sdp;
	char* sessionid;
}SrsSdpResponseType;

#define Yang_SDP_BUFFERLEN 1024*12

static void yang_destroy_srsresponse(SrsSdpResponseType* srs){
	if(srs==NULL) return;
	yang_free(srs->sdp);
	yang_free(srs->serverIp);
	yang_free(srs->sessionid);
}

static int32_t yang_sdp_querySrs(YangMetaConnection *conn,YangPeer* peer,SrsSdpResponseType* srs,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp)
{

	int32_t err=Yang_Ok;
	char *p;
	char* sdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);

	YangJson *jcode,*server,*sessionid,*jsdp;
	YangJsonReader reader;

	if(yang_http_post(yangfalse,peer->peerInfo.familyType,sdp,ip, port, purl, (uint8_t*)psdp, yang_strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query srs sdp failure!");
	}

	p=yang_strstr(sdp,"{");
	if(p==NULL)
		return ERROR_STRING;

	if(yang_create_jsonReader(&reader,p)!=Yang_Ok){
		yang_error("read srs response json error!");
		err=ERROR_STRING;
		goto cleanup;
	}

	jcode = reader.getObjectItemCaseSensitive(reader.session, "code");
	if (reader.isNumber(jcode))
	{
		err=jcode->valueint==0?Yang_Ok:ERROR_SERVER_ConnectFailure;
		if(err!=Yang_Ok) goto cleanup;
	}

	srs->retcode=jcode->valueint;

	server = reader.getObjectItemCaseSensitive(reader.session, "server");
	sessionid = reader.getObjectItemCaseSensitive(reader.session, "sessionid");
	jsdp = reader.getObjectItemCaseSensitive(reader.session, "sdp");

	if (reader.isString(server) && (server->valuestring != NULL))
	{
		srs->serverIp=(char*)yang_calloc(yang_strlen(server->valuestring)+1,1);
		yang_memcpy(srs->serverIp,server->valuestring,yang_strlen(server->valuestring));

	}

	if (reader.isString(sessionid) && (sessionid->valuestring != NULL))
	{
		srs->sessionid=(char*)yang_calloc(yang_strlen(sessionid->valuestring)+1,1);
		yang_memcpy(srs->sessionid,sessionid->valuestring,yang_strlen(sessionid->valuestring));

	}

	if (reader.isString(jsdp) && (jsdp->valuestring != NULL))
	{
		srs->sdp=(char*)yang_calloc(yang_strlen(jsdp->valuestring)+1,1);
		yang_cstr_replace(jsdp->valuestring,srs->sdp, "\r\n", "\n");
	}

	cleanup:
	yang_destroy_jsonReader(&reader);
	yang_free(sdp);
	return err;
}

static int32_t yang_srs_getSignal(YangMetaConnection *conn,YangPeer* peer,char* url,SrsSdpResponseType* srs,char* sdp) {
	int32_t err = Yang_Ok;
	YangRtcDirection role=peer->peerInfo.direction;
	char *srsSdp;
	const char* roleStr=role==YangRecvonly?"play":"publish";
	char apiStr[256]={0};
	char streamurl[1024]={0};
	char apiurl[512] ;
	YangUrlData urlData;
	YangJsonWriter writer;

	yang_memset(urlData.server,0,sizeof(urlData.server));
	urlData.port=0;

	if(yang_url_parse(peer->peerInfo.familyType,url,&urlData))
		return 1;

	yang_sprintf(apiStr,"http://%s:%d/rtc/v1/%s/",urlData.server, urlData.port,roleStr);
	yang_sprintf(streamurl,"webrtc://%s:%d/%s/%s",urlData.server,
			urlData.port, urlData.app, urlData.stream);


	yang_create_jsonWriter(&writer);

	writer.addStringToObject(writer.session,"capi",apiStr);
	writer.addStringToObject(writer.session,"streamurl",streamurl);
	writer.addNullToObject(writer.session,"clientip");
	writer.addStringToObject(writer.session,"sdp",sdp);
	srsSdp=writer.printUnformatted(writer.session);

	yang_memset(apiurl,0,sizeof(apiurl));

	yang_sprintf(apiurl, "rtc/v1/%s/", roleStr);
	err=yang_sdp_querySrs(conn,peer,srs,role==YangRecvonly?1:0,urlData.server,urlData.port,apiurl, srsSdp);


	yang_destroy_jsonWriter(&writer);


	return err;
}

int32_t yang_srs_connectRtcServer(YangMetaConnection *conn,YangPeer* peer,char* url){

	int32_t err=Yang_Ok;
	char *tsdp=NULL;
	SrsSdpResponseType srs;

	yang_memset(&srs,0,sizeof(SrsSdpResponseType));

	peer->peerInfo.iceMode = YangIceModeLite;
	conn->createOffer(peer, &tsdp);
	conn->setLocalDescription(peer,tsdp);

	if ((err=yang_srs_getSignal(conn,peer,url,&srs,tsdp))  == Yang_Ok) {
		conn->setRemoteDescription(peer,srs.sdp);

	}
	yang_free(tsdp);
	yang_destroy_srsresponse(&srs);
    return err;
}
