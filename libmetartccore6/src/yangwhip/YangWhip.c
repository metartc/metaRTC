//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangwhip/YangWhip.h>

#include <yangrtc/YangRtcConnection.h>

#include <yangice/YangRtcStun.h>
#include <yangice/YangRtcSocket.h>

#include <yangsdp/YangSdp.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCUrl.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttpSocket.h>

#define Yang_SDP_BUFFERLEN 1024*12

int32_t yang_whip_getSignal(YangRtcSession* session,char* url,char** premoteSdp,char* localSdp) {
	int32_t err=Yang_Ok;
	YangUrlData urlData={0};
	yang_http_url_parse(session->context.avinfo->sys.familyType,url,&urlData);

	char* remoteSdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);

	if(yang_http_post(yangtrue,session->context.avinfo->sys.familyType,remoteSdp,urlData.server,
			urlData.port, urlData.stream, (uint8_t*)localSdp, yang_strlen(localSdp))){
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

int32_t yang_whip_connectPeer(YangRtcConnection* conn,char* url){
	YangRtcSession* session=conn->session;
	int err=Yang_Ok;
	char* remoteSdp=NULL;


	char *localSdp=NULL;
	conn->createOffer(session, &localSdp);

	if ((err=yang_whip_getSignal(conn->session,url,&remoteSdp,localSdp))  == Yang_Ok) {
		if(remoteSdp) conn->setRemoteDescription(conn->session,remoteSdp);
	}
	yang_free(localSdp);
	yang_free(remoteSdp);
    return err;
}
