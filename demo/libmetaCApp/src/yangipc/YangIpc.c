//
// Copyright (c) 2019-2022 yanggaofeng
//


#include <yangipc/YangIpc.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangMath.h>
#ifndef __ANDROID__
int32_t yang_ipc_startRtc(YangIpcSession* session,char* remoteIp,char* sdp,char* response );

void g_ipc_receive(char *data, int32_t nb_data,char* response,char* remoteIp, void *user) {
	if (user == NULL)	return;
	YangIpcSession *session = (YangIpcSession*) user;

	yang_ipc_startRtc(session,remoteIp, data,response);
}

void g_yang_ipc_sendRequest(void* context,int32_t puid,uint32_t ssrc,YangRequestType req){
	YangIpcSession *session = (YangIpcSession*) context;
	session->publish->sendRequest(&session->publish->session,puid,ssrc,req);

}

int32_t yang_ipc_initRtc(YangIpcSession* session){

	if (session->publish == NULL) {
		session->publish = (YangIpcPublish*)calloc(sizeof(YangIpcPublish),1);
		yang_create_ipcPublish(session->publish,&session->avinfo);

		session->publish->session.rtc.session.rtcCallback.context=session;
		session->publish->session.rtc.session.rtcCallback.sendRequest=g_yang_ipc_sendRequest;

		session->publish->session.encoder.session.out_videoBuffer=session->videoBuffer;
		session->publish->session.rtc.session.in_videoBuffer=session->videoBuffer;

		session->publish->startRtc(&session->publish->session);
	}


	return Yang_Ok;
}

int32_t yang_ipc_initRtc2(YangIpcSession* session,yangbool hasPlay){
	if(!session->isInitRtc){
		session->publish->init(&session->publish->session);
		session->publish->startVideoEncoding(&session->publish->session);
		session->isInitRtc=yangtrue;
	}

	return Yang_Ok;
}

int32_t yang_ipc_startRtc(YangIpcSession* session,char* remoteIp,char* sdp,char* response ) {
	int32_t localPort=session->avinfo.sys.rtcLocalPort++;
	int err = Yang_Ok;
	yang_ipc_initRtc(session);
	int hasplay=0;

	if ((err = session->publish->addPeer(&session->publish->session,sdp, response,remoteIp,localPort,&hasplay)) != Yang_Ok) {
		return yang_error_wrap(err, " connect server failure!");
	}

	yang_ipc_initRtc2(session,hasplay?true:false);
	return err;
}


void yang_ipc_listen(YangIpcSession* session){

	yang_create_p2pserver(&session->p2pServer,session->avinfo.sys.httpPort);
	session->p2pServer.receive=g_ipc_receive;
	session->p2pServer.user=session;
	yang_start_p2pserver(&session->p2pServer);

}

void yang_ipc_initAvinfo(YangIpcSession* session){
	session->avinfo.audio.enableMono=0;
	session->avinfo.audio.sample=48000;
	session->avinfo.audio.channel=2;
	session->avinfo.audio.enableAec=1;
	session->avinfo.audio.audioCacheNum=8;
	session->avinfo.audio.audioCacheSize=8;
	session->avinfo.audio.audioPlayCacheNum=8;

	session->avinfo.video.videoCacheNum=10;
	session->avinfo.video.evideoCacheNum=10;
	session->avinfo.video.videoPlayCacheNum=10;

	session->avinfo.audio.audioEncoderType=Yang_AED_OPUS;
	session->avinfo.sys.rtcLocalPort=17000;
	session->avinfo.enc.enc_threads=4;
}

void yang_ipc_init(YangIpcSession* session){

}

void yang_ipc_checkAlive(YangIpcSession* session){
	session->publish->session.rtc.checkAlive(&session->publish->session.rtc.session);
}

void yang_create_ipc(YangIpc* ipc){

	if(ipc==NULL) return;

	YangIpcSession* session=&ipc->session;
	YangIni ini;
	yang_create_ini(&ini,(char*)"yang_config.ini");
	ini.initAvinfo(ini.filename,&session->avinfo);
	yang_destroy_ini(&ini);

	session->avinfo.video.videoEncoderFormat=YangI420;

	session->avinfo.enc.createMeta=0;

	yang_ipc_initAvinfo(session);

	yang_setLogLevel(session->avinfo.sys.logLevel);
	yang_setLogFile(session->avinfo.sys.enableLogFile);

	session->avinfo.sys.httpPort=1988;
	session->avinfo.sys.mediaServer=Yang_Server_P2p;//Yang_Server_Srs/Yang_Server_Zlm/Yang_Server_P2p
	session->avinfo.sys.rtcLocalPort=10000+yang_random()%15000;

	memset(session->avinfo.sys.localIp,0,sizeof(session->avinfo.sys.localIp));
	yang_getLocalInfo(session->avinfo.sys.localIp);

	if(session->avinfo.rtc.iceUsingLocalIp){
		char ipstr[32]={0};
		strcpy(ipstr,session->avinfo.rtc.iceLocalIP);
		memset(session->avinfo.rtc.iceLocalIP,0,sizeof(session->avinfo.rtc.iceLocalIP));
		yang_getIp(ipstr,session->avinfo.rtc.iceLocalIP);
	}

	session->hasAudio=yangtrue;

	//using h264 h265
	session->avinfo.video.videoEncoderType=Yang_VED_264;//Yang_VED_265;

	session->avinfo.audio.enableAec=1;

	session->avinfo.rtc.enableDatachannel=1;

	strcpy(session->avinfo.rtc.iceServerIP,"182.92.163.143");
	session->avinfo.rtc.iceServerPort=3478;
	session->avinfo.rtc.iceCandidateType=0;

	session->videoBuffer = (YangVideoEncoderBuffer2*)calloc(sizeof(YangVideoEncoderBuffer2),1);
	yang_create_videoEncoderBuffer2(session->videoBuffer,2);


	yang_ipc_initRtc(session);

	ipc->start=yang_ipc_listen;
	ipc->init=yang_ipc_init;
	ipc->checkAlive=yang_ipc_checkAlive;

}

void yang_destroy_ipc(YangIpc* ipc){
	if(ipc==NULL) return;
	YangIpcSession* session=&ipc->session;
	yang_destroy_ipcPublish(session->publish);
	yang_destroy_p2pserver(&session->p2pServer);
	yang_destroy_videoEncoderBuffer2(session->videoBuffer);
	yang_free(session->videoBuffer);

}

#endif
