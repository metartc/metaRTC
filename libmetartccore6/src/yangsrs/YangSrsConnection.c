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
#include <yangutil/sys/YangHttpSocket.h>

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
	if(yang_http_post(session->context.avinfo->sys.familyType,sdp,ip, port, purl, (uint8_t*)psdp, yang_strlen(psdp))){
		yang_free(sdp);
		return yang_error_wrap(1,"query srs sdp failure!");
	}

#if Yang_Enable_Json
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

#else
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

		if ((p = yang_strstr(strs.str[i], "\"code\":"))) {
			char *buf = (char*) yang_calloc(1, yang_strlen(p) + 1);
			yang_cstr_replace(p + yang_strlen("\"code\":"), buf, "\"", "");
			srs->retcode = atoi(buf);
			yang_free(buf);
			if ((err = srs->retcode) != 0)	break;

			continue;
		}

		if ((p = yang_strstr(strs.str[i], "\"server\":"))) {
			srs->serverIp = (char*) yang_calloc(1, yang_strlen(p) + 1);
			yang_cstr_replace(p + yang_strlen("\"server\":"), srs->serverIp, "\"", "");

			continue;
		}

		if ((p = yang_strstr(strs.str[i], "\"sessionid\":"))) {

			srs->sessionid = (char*) yang_calloc(1, yang_strlen(p) + 1);
			yang_cstr_replace(p + yang_strlen("\"sessionid\":"), srs->sessionid, "\"","");

			continue;
		}

		if ((p = yang_strstr(strs.str[i], "\"sdp\":"))) {
			srs->sdp = (char*) yang_calloc(1, yang_strlen(p) + 1);
			yang_cstr_replace(p + yang_strlen("\"sdp\":"), srs->sdp, "\"", "");

			continue;
		}

	}



	yang_destroy_strings(&strs);
	yang_free(sdp);
	return err;
#endif

}

int32_t yang_srs_getSignal(YangRtcSession* session,SrsSdpResponseType* srs,char* sdp) {
	int32_t err = Yang_Ok;
	YangStreamOptType role=session->context.streamConfig->streamOptType;
	const char* roleStr=role==Yang_Stream_Play?"play":"publish";
#if Yang_Enable_Json

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

#else
	char *srsSdp = (char*)yang_calloc(1,1024*12);
	char qt = '"';
	yang_sprintf(srsSdp,
			"{%capi%c:%chttp://%s:%d/rtc/v1/%s/%c,%cstreamurl%c:%cwebrtc://%s:%d/%s/%s%c,%cclientip%c:null,%csdp%c:%c%s%c}",
			qt, qt, qt, session->context.streamConfig->remoteIp, session->context.streamConfig->remotePort,roleStr ,qt, qt, qt, qt, session->context.streamConfig->remoteIp,
			session->context.streamConfig->remotePort, session->context.streamConfig->app, session->context.streamConfig->stream, qt, qt, qt, qt, qt, qt, sdp, qt);
#endif

	char apiurl[256] ;
	yang_memset(apiurl,0,sizeof(apiurl));

	yang_sprintf(apiurl, "rtc/v1/%s/", roleStr);
	err=yang_sdp_querySrs(session,srs,role==Yang_Stream_Play?1:0,(char*)session->context.streamConfig->remoteIp,session->context.streamConfig->remotePort,apiurl, srsSdp);

#if Yang_Enable_Json
	yang_destroy_jsonWriter(&writer);
#else
	yang_free(srsSdp);
#endif


	return err;
}

int32_t yang_srs_connectRtcServer(YangRtcConnection* conn){
	YangRtcSession* session=conn->session;
	int err=Yang_Ok;
	SrsSdpResponseType srs;
	yang_memset(&srs,0,sizeof(SrsSdpResponseType));
#if Yang_Enable_Json
	session->context.avinfo->rtc.enableHttpServerSdp=yangfalse;
#else
	session->context.avinfo->rtc.enableHttpServerSdp=yangtrue;
#endif
	char *tsdp=NULL;
	conn->createOffer(session, &tsdp);


	if ((err=yang_srs_getSignal(conn->session,&srs,tsdp))  == Yang_Ok) {
		conn->setRemoteDescription(conn->session,srs.sdp);
	}
	yang_free(tsdp);
	yang_destroy_srsresponse(&srs);
    return err;
}
