//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangice/YangRtcStun.h>
#include <yangice/YangRtcSocket.h>
#include <yangsrs/YangSrsConnection.h>

#include <yangsdp/YangSdp.h>
#include <yangjson/YangJson.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangrtc/YangRtcConnection.h>
#include <yangutil/sys/YangHttp.h>
typedef struct{
	int32_t retcode;
	char* serverIp;
	char* sdp;
	char* sessionid;
}SrsSdpResponseType;

#define Yang_SDP_BUFFERLEN 1024*12

void yang_destroy_srsresponse(SrsSdpResponseType* srs){
	if(srs==NULL) return;
	yang_free(srs->sdp);
	yang_free(srs->serverIp);
	yang_free(srs->sessionid);
}

int32_t yang_sdp_querySrs(YangRtcSession* session,SrsSdpResponseType* srs,int32_t isplay,char* ip,int32_t port,char* purl, char* psdp)
{

	int32_t err=Yang_Ok;

	char* sdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);
	if(yang_http_post(yangfalse,session->context.avinfo->sys.familyType,sdp,ip, port, purl, (uint8_t*)psdp, yang_strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query srs sdp failure!");
	}

	YangJsonReader reader;
	char* p=yang_strstr(sdp,"{");
	if(p==NULL) return ERROR_STRING;

	if(yang_create_jsonReader(&reader,p)!=Yang_Ok){
		yang_error("read srs response json error!");
		err=ERROR_STRING;
		goto cleanup;
	}

	YangJson* jcode = reader.getObjectItemCaseSensitive(reader.session, "code");
	if (reader.isNumber(jcode))
	{
		err=jcode->valueint==0?Yang_Ok:ERROR_SERVER_ConnectFailure;
		if(err!=Yang_Ok) goto cleanup;
	}

	srs->retcode=jcode->valueint;
	YangJson* server = reader.getObjectItemCaseSensitive(reader.session, "server");
	YangJson* sessionid = reader.getObjectItemCaseSensitive(reader.session, "sessionid");
	YangJson* jsdp = reader.getObjectItemCaseSensitive(reader.session, "sdp");
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

int32_t yang_srs_getSignal(YangRtcSession* session,SrsSdpResponseType* srs,char* sdp) {
	int32_t err = Yang_Ok;
	YangRtcDirection role=session->context.streamConfig->direction;
	const char* roleStr=role==YangRecvonly?"play":"publish";

	char apiStr[256]={0};
	char streamurl[256]={0};

	yang_sprintf(apiStr,"http://%s:%d/rtc/v1/%s/",session->context.streamConfig->remoteIp, session->context.streamConfig->remotePort,roleStr);
	yang_sprintf(streamurl,"webrtc://%s:%d/%s/%s",session->context.streamConfig->remoteIp,
			session->context.streamConfig->remotePort, session->context.streamConfig->app, session->context.streamConfig->stream);

	YangJsonWriter writer;
	yang_create_jsonWriter(&writer);

	writer.addStringToObject(writer.session,"capi",apiStr);
	writer.addStringToObject(writer.session,"streamurl",streamurl);
	writer.addNullToObject(writer.session,"clientip");
	writer.addStringToObject(writer.session,"sdp",sdp);
	char *srsSdp=writer.printUnformatted(writer.session);
	char apiurl[256] ;
	yang_memset(apiurl,0,sizeof(apiurl));

	yang_sprintf(apiurl, "rtc/v1/%s/", roleStr);
	err=yang_sdp_querySrs(session,srs,role==YangRecvonly?1:0,(char*)session->context.streamConfig->remoteIp,session->context.streamConfig->remotePort,apiurl, srsSdp);


	yang_free(srsSdp);

	return err;
}

int32_t yang_srs_connectRtcServer(YangRtcConnection* conn){
	YangRtcSession* session=conn->session;
	int err=Yang_Ok;
	SrsSdpResponseType srs;
	yang_memset(&srs,0,sizeof(SrsSdpResponseType));

	//session->context.avinfo->rtc.enableHttpServerSdp=yangtrue;

	char *tsdp=NULL;
	conn->createOffer(session, &tsdp);

	if(tsdp) conn->setLocalDescription(session,tsdp);
	if ((err=yang_srs_getSignal(conn->session,&srs,tsdp))  == Yang_Ok) {
		conn->setRemoteDescription(conn->session,srs.sdp);
	}
	yang_free(tsdp);
	yang_destroy_srsresponse(&srs);
    return err;
}
