//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangcsrs/YangSrsConnection.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>
#include <yangwebrtc/YangRtcSession.h>
#include <yangwebrtc/YangRtcStun.h>
#include <yangsdp/YangSdp.h>
#include <yangcsrs/YangSrsSdp.h>

int32_t yang_srs_doHandleSignal(YangRtcSession* session,SrsSdpResponseType* srs,int32_t localport, YangStreamOptType role);

int32_t g_localport=10000;

int32_t yang_srs_connectRtcServer(YangRtcSession* session){

	g_localport=session->context.streamConf->localPort;
	int err=Yang_Ok;
	SrsSdpResponseType srs;
	memset(&srs,0,sizeof(SrsSdpResponseType));
	g_localport+=yang_random()%15000;

	if ((err=yang_srs_doHandleSignal(session,&srs,g_localport,session->context.streamConf->streamOptType))  == Yang_Ok) {
		yang_stun_createStunPacket(session,srs.sessionid,session->remoteIcePwd);
		yang_rtcsession_startudp(session,g_localport);
		g_localport++;
	}

	yang_destroy_srsresponse(&srs);
    return err;
}

int32_t yang_srs_doHandleSignal(YangRtcSession* session,SrsSdpResponseType* srs,int32_t localport, YangStreamOptType role) {
	int32_t err = Yang_Ok;
	char *srsSdp = (char*)calloc(1,1024*12);
	char *tsdp=NULL;


	yang_sdp_genLocalSdp(session,localport, &tsdp,role);

	char qt = '"';
	const char* roleStr=role==Yang_Stream_Play?"play":"publish";
	sprintf(srsSdp,
			"{%capi%c:%chttp://%s:%d/rtc/v1/%s/%c,%cstreamurl%c:%cwebrtc://%s:%d/%s/%s%c,%cclientip%c:null,%csdp%c:%c%s%c}",
			qt, qt, qt, session->context.streamConf->serverIp, session->context.streamConf->serverPort,roleStr ,qt, qt, qt, qt, session->context.streamConf->serverIp,
			session->context.streamConf->serverPort, session->context.streamConf->app, session->context.streamConf->stream, qt, qt, qt, qt, qt, qt, tsdp, qt);


	char apiurl[256] ;
	memset(apiurl,0,sizeof(apiurl));

	sprintf(apiurl, "rtc/v1/%s/", roleStr);
	err=yang_sdp_querySrs(session,srs,role==Yang_Stream_Play?1:0,(char*)session->context.streamConf->serverIp,session->context.streamConf->serverPort,apiurl, srsSdp);


	yang_free(srsSdp);
	yang_free(tsdp);

	return err;
}
