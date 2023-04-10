//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangwhip/YangWhip.h>

#include <yangrtc/YangRtcConnection.h>

#include <yangice/YangRtcStun.h>
#include <yangice/YangRtcSocket.h>

#include <yangsdp/YangSdp.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttpSocket.h>

#define Yang_SDP_BUFFERLEN 1024*12



int32_t yang_whip_getSignal(YangRtcSession* session,yangbool isHttpUrl,char** premoteSdp,char* localSdp) {
	int32_t err=Yang_Ok;
	YangStreamOptType role=session->context.streamConfig->streamOptType;
	char* whipUrl=session->context.avinfo->sys.whipUrl;
	char* whepUrl=session->context.avinfo->sys.whepUrl;

	char apiurl[256] ;
	yang_memset(apiurl,0,sizeof(apiurl));
	yang_trace("\nsession->context.avinfo->sys.whipUrl=%s",session->context.avinfo->sys.whipUrl);
	if(isHttpUrl){
		yang_strcpy(apiurl,role==Yang_Stream_Publish?whipUrl:whepUrl);
	}else{
		if(role==Yang_Stream_Publish)
			yang_sprintf(apiurl, whipUrl, session->context.streamConfig->app,session->context.streamConfig->stream);
		else
			yang_sprintf(apiurl, whepUrl, session->context.streamConfig->app,session->context.streamConfig->stream);
	}
	char* remoteSdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);


	if(yang_http_post(yangtrue,session->context.avinfo->sys.familyType,remoteSdp,(char*)session->context.streamConfig->remoteIp,
			session->context.streamConfig->remotePort, apiurl, (uint8_t*)localSdp, yang_strlen(localSdp))){
		yang_free(remoteSdp);
		return yang_error_wrap(1,"query whip sdp failure!");
	}

	if(yang_strstr(remoteSdp,"201")&&yang_strstr(remoteSdp,"Created")){
		char* p=yang_strstr(remoteSdp,"\r\n\r\n");
		if(p){
			p=p+4;
			int32_t len=yang_strlen(p);
			char* sdp=(char*)yang_calloc(len+1,1);
			yang_cstr_replace(p,sdp, "\r\n", "\n");
			*premoteSdp=sdp;
			err=Yang_Ok;
		}
	}else{
		err=ERROR_RTC_Whip;
	}
	return err;

}

int32_t yang_whip_connectPeer(YangRtcConnection* conn,yangbool isHttpUrl){
	YangRtcSession* session=conn->session;
	int err=Yang_Ok;
	char* remoteSdp=NULL;


	char *localSdp=NULL;
	conn->createOffer(session, &localSdp);


	if ((err=yang_whip_getSignal(conn->session,isHttpUrl,&remoteSdp,localSdp))  == Yang_Ok) {
		if(remoteSdp) conn->setRemoteDescription(conn->session,remoteSdp);
	}
	yang_free(localSdp);
	yang_free(remoteSdp);
    return err;
}
