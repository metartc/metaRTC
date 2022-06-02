//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangzlm/YangZlmConnection.h>
#include <yangrtc/YangRtcSession.h>
#include <yangrtc/YangRtcStun.h>

#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangsdp/YangSdp.h>
#include <yangzlm/YangZlmSdp.h>
#include <yangrtc/YangUdpHandle.h>
int32_t yang_zlm_doHandleSignal(YangRtcSession* session,ZlmSdpResponseType* zlm,int32_t localport, YangStreamOptType role);


int32_t yang_zlm_connectRtcServer(YangRtcSession* session){

	int err=Yang_Ok;
	ZlmSdpResponseType zlm;
	memset(&zlm,0,sizeof(ZlmSdpResponseType));
	if ((err=yang_zlm_doHandleSignal(session,&zlm,session->context.streamConf->localPort,session->context.streamConf->streamOptType))  == Yang_Ok) {
		yang_create_rtcudp(session->context.udp,session->context.streamConf->remoteIp,session->listenPort, session->context.streamConf->localPort);

		yang_stun_createRequestStunPacket(session,session->remoteIcePwd);
		yang_rtcsession_startudp(session);
		yang_start_rtcudp(session->context.udp);
	}

	yang_destroy_zlmresponse(&zlm);
    return err;
}

int32_t yang_zlm_doHandleSignal(YangRtcSession* session,ZlmSdpResponseType* zlm,int32_t localport, YangStreamOptType role) {
	int32_t err = Yang_Ok;

	char *tsdp=NULL;
	yang_sdp_genLocalSdp(session,localport, &tsdp,role);

	char apiurl[256] ;
	memset(apiurl,0,sizeof(apiurl));

	sprintf(apiurl, "index/api/webrtc?app=%s&stream=%s&type=%s", session->context.streamConf->app,session->context.streamConf->stream,role==Yang_Stream_Play?"play":"push");
	err=yang_zlm_query(session,zlm,role==Yang_Stream_Play?1:0,(char*)session->context.streamConf->remoteIp,80,apiurl, tsdp);

	yang_free(tsdp);

	return err;
}
