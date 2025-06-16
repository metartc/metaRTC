//
// Copyright (c) 2019-2025 yanggaofeng
//


#include <yangrtc/YangWhip.h>

#include "YangSrsConnection.h"
#include "YangZlmConnection.h"
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCUrl.h>
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangHttp.h>

#define Yang_SDP_BUFFERLEN 1024*12

typedef struct{
	void* conn;
	YangPeerInfo peerInfo;
	YangPeerCallback peerCallback;
}YangWhipSession;


static int32_t yang_whip_getSignal(YangMetaConnection *conn,YangPeer* peer,char* url,char** premoteSdp,char* localSdp) {
	int32_t err=Yang_Ok;
	YangUrlData urlData={0};
	yang_http_url_parse(peer->peerInfo.familyType,url,&urlData);

	char* remoteSdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);

	if(yang_http_post(yangtrue,peer->peerInfo.familyType,remoteSdp,urlData.server,
			urlData.port, urlData.stream, (uint8_t*)localSdp, yang_strlen(localSdp))){
		char* endp=yang_strstr(remoteSdp,"\r\n\r\n");
		if(endp) yang_error("\n%s",endp+4);
		yang_free(remoteSdp);
		return yang_error_wrap(1,"query whip sdp failure!");
	}

	if(yang_strstr(remoteSdp,"201")&&yang_strstr(remoteSdp,"Created")){
		char* p2=yang_strstr(remoteSdp,"\r\n\r\n");
		if(p2==NULL) return ERROR_RTC_Whip;
		char* p=yang_strstr(p2,"v=0");
		if(p){
			int32_t len=yang_strlen(p);
			char* sdp=(char*)yang_calloc(len+1,1);
			yang_cstr_replace(p,sdp, "\r\n", "\n");
			p=yang_strstr(sdp,"\n\n");
			if(p) *p=0;
			*premoteSdp=sdp;
			err=Yang_Ok;
		}
	}else{
		err=ERROR_RTC_Whip;
	}
	yang_free(remoteSdp);
	return err;

}

int32_t yang_whip_connectPeer(YangMetaConnection *conn,YangPeer* peer,char* url){

	int err=Yang_Ok;
	char* remoteSdp=NULL;



	char *localSdp=NULL;
	if((err=conn->createOffer(peer, &localSdp))!=Yang_Ok){
		return yang_error_wrap(err,"create offer fail!");
	}

	if(localSdp) conn->setLocalDescription(peer,localSdp);


	if ((err=yang_whip_getSignal(conn,peer,url,&remoteSdp,localSdp))  == Yang_Ok) {
		if(remoteSdp) conn->setRemoteDescription(peer,remoteSdp);
	}

	yang_free(localSdp);
	yang_free(remoteSdp);
    return err;
}

int32_t yang_whip_connectWhipWhepServer(YangPeer* peer,char* url){
	YangMetaConnection conn={0};
	yang_create_metaConnection(&conn);

	if(peer==NULL)
		return  ERROR_RTC_PEERCONNECTION;


	if(conn.isConnected(peer))
		return Yang_Ok;

	return yang_whip_connectPeer(&conn,peer,url);

}

int32_t yang_whip_connectSfuServer(YangPeer* peer,char* url,int32_t mediaServer){
	YangMetaConnection conn={0};
	yang_create_metaConnection(&conn);

	if(peer==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	if(conn.isConnected(peer))
		return Yang_Ok;


	if(mediaServer==Yang_Server_Zlm)
		return yang_zlm_connectRtcServer(&conn,peer,url);
	if(mediaServer==Yang_Server_Srs)
		return yang_srs_connectRtcServer(&conn,peer,url);

	return ERROR_RTC_PEERCONNECTION;
}



