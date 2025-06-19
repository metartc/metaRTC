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
	int32_t err=Yang_Ok,len;
	YangUrlData urlData={0};

	yangbool isHttps=yang_memcmp(url,"https:",6)==0?yangtrue:yangfalse;
	char *remoteSdp,*p,*p2,*endp,*sdp;

	yang_http_url_parse(peer->peerInfo.familyType,url,&urlData);

	remoteSdp=(char*)yang_calloc(1,Yang_SDP_BUFFERLEN);
	if(yang_https_post(yangtrue,peer->peerInfo.familyType,remoteSdp,urlData.server,
			urlData.port, urlData.stream, (uint8_t*)localSdp, yang_strlen(localSdp))){
		endp=yang_strstr(remoteSdp,"\r\n\r\n");

		if(endp)
			yang_error("\n%s",endp+4);

		err=1;
		yang_error("query whip sdp failure!");
		goto cleanup;
	}

	if(yang_strstr(remoteSdp,"201")&&yang_strstr(remoteSdp,"Created")){
		p2=yang_strstr(remoteSdp,"\r\n\r\n");
		if(p2==NULL){
			err=ERROR_RTC_Whip;
			goto cleanup;
		}


		p=yang_strstr(p2,"v=0");
		if(p){
			len=yang_strlen(p);
			sdp=(char*)yang_calloc(len+1,1);
			yang_cstr_replace(p,sdp, "\r\n", "\n");
			p=yang_strstr(sdp,"\n\n");
			if(p)
				*p=0;

			*premoteSdp=sdp;
			err=Yang_Ok;
		}
	}else{
		err=ERROR_RTC_Whip;
	}
	cleanup:
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

int32_t yang_whip_connectSfuServer(YangPeer* peer,char* url,yangbool isHttps,int32_t mediaServer){
	YangMetaConnection conn={0};
    yang_create_metaConnection(&conn);

	if(peer==NULL)
		return  ERROR_RTC_PEERCONNECTION;

	if(conn.isConnected(peer))
		return Yang_Ok;


	if(mediaServer==Yang_Server_Zlm)
		return yang_zlm_connectRtcServer(&conn,peer,url,isHttps);
	if(mediaServer==Yang_Server_Srs)
		return yang_srs_connectRtcServer(&conn,peer,url,isHttps);

	return ERROR_RTC_PEERCONNECTION;
}

