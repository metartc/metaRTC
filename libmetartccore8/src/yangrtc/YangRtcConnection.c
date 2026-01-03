//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangice/YangRtcSocket.h>
#include <yangice/YangRtcStun.h>


#include <yangrtc/YangRtcRtcp.h>

#include <yangrtc/YangRtcConnection.h>

#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangPullStream.h>
#include <yangrtc/YangPushStream.h>

#include <yangrtp/YangRtcpCompound.h>


#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangSsrc.h>

#include <yangsdp/YangSdp.h>


static void g_session_receive(char *data, int32_t nb_data, void *user) {
	YangRtcConnection *conn = (YangRtcConnection*) user;
	if (user == NULL)
		return;

	conn->receive(conn->session,data,nb_data);

}

void yang_rtcconn_startStunTimer(YangRtcSession *session);

static void g_yang_startStunTimer(void *user) {
	YangRtcConnection *conn = (YangRtcConnection*) user;
	if (user == NULL)
		return;

	yang_rtcconn_startStunTimer(conn->session);

}

static void yang_onConnectionStateChange(YangRtcSession *session,YangRtcConnectionState state){
	if(session->context.peerCallback&&session->context.peerCallback->iceCallback.onConnectionStateChange){
						session->context.peerCallback->iceCallback.onConnectionStateChange(
								session->context.peerCallback->iceCallback.context,
								session->context.peerInfo->uid,
								state);
	}
}

static void yang_rtcconn_close(YangRtcSession *session) {
	if (session == NULL)	return;
#if Yang_Enable_Dtls

	session->context.dtls->session.isSendAlert = yangtrue;

	if (session->context.dtls&&!session->context.dtls->session.isRecvAlert&&session->context.dtls->sendDtlsAlert){
		session->context.dtls->sendDtlsAlert(&session->context.dtls->session);
	}

#else
	if(session->isControlled==0){
		char alerts[15];
		yang_memset(alerts,0,15);
		alerts[0]=30;
		for(int i=0;i<5;i++){
			yang_usleep(1000*20);
			if (yang_rtc_sendData(session->context.sock, alerts, 15)	!= Yang_Ok) {
				yang_error("send error");
			}
		}
	}
#endif
	session->context.state = Yang_Conn_State_Closed;
	yang_onConnectionStateChange(session,Yang_Conn_State_Closed);
}

static void g_yang_doTask(int32_t taskId, void *user) {
	YangRtcSession *session = (YangRtcSession*) user;
	if (user == NULL)
		return;

	//if(session->ice.session.iceState==YangIceFail){
	//	yang_rtcconn_close(session);
	//	return;
	//}

	if(session->context.state!=Yang_Conn_State_Connected)
		return;

	if (!session->isControlled && taskId == 1) {

		if (session->ice.sendRequestStun(&session->ice.session)==Yang_Ok) {

			if(session->context.state==Yang_Conn_State_New) {
				session->context.state=Yang_Conn_State_Connecting;
				yang_onConnectionStateChange(session,Yang_Conn_State_Connecting);
			}
		}

	}

	if (session->startRecv&&session->play) {

		if (taskId == 1) {

			if (session->play->send_rtcp_rr(&session->context,session->play->pullStream))	yang_error("RTCP Error:RR err ");
#if Yang_Enable_RtcpXr
			if (session->context.streamConfig->streamOptType==YangRecvonly && session->play->send_rtcp_xr_rrtr(&session->context,session->play->pullStream))
				yang_error("RTCP Error:XR err ");
#endif

		}

		if (taskId == 100) {
#if Yang_Enable_TWCC
			if (session->play->send_periodic_twcc(&session->context,session->play->pullStream))
				yang_error("RTCP Error:send twcc err ");
#endif

		}

	}

	if(session->push){
			if (taskId == 1) {
				if (session->push->send_rtcp_sr(&session->context,session->push->pubStream))		yang_error("send rtcp sr Error ");
				if (session->push->check_bandwidth(&session->context,session->push->pubStream))		yang_error("check bandwidth Error ");
				if (session->push->check_twcc(&session->context,session->push->pubStream))		yang_error("check twcc Error ");
#if Yang_Enable_RTC_Video
				if(session->play&&session->context.stats.recvStats.video.rtt>0 )
							session->play->update_rtt(&session->context,session->play->pullStream,session->play->pullStream->videoTrack->session.track.ssrc,session->context.stats.recvStats.video.rtt);
#endif

			}
		}
}
static void yang_rtcconn_initPlay(YangRtcSession *session){

	if(session->play ) return;
	if (session->context.videoDirection == YangRecvonly || session->context.videoDirection==YangSendrecv
			||session->context.audioDirection == YangRecvonly || session->context.audioDirection==YangSendrecv
			) {
		if(session->playRtpBuffer == NULL) {
			session->playRtpBuffer = (YangRtpBuffer*) yang_calloc(1,sizeof(YangRtpBuffer));
			yang_create_rtpBuffer(session->playRtpBuffer, 1500, kRtpPacketSize);
		}
		if (session->play == NULL) {
			session->play = (YangRtcPull*) yang_calloc(1,sizeof(YangRtcPull));
			yang_create_rtcpull(&session->context, session->play,session->playRtpBuffer);

		}
	}

}

static void yang_rtcconn_init(YangRtcSession *session) {
	if (session == NULL || session->isInited)
		return;

	session->isSendDtls = yangfalse;

	session->sessionTimeout=session->context.peerInfo->rtc.sessionTimeout;

	//  session->20ms
	session->tm_1s = (YangCTimer*) yang_calloc(1, sizeof(YangCTimer));
	yang_create_timer(session->tm_1s, session, 1, 1000);
	session->tm_1s->doTask = g_yang_doTask;

	session->tm_100ms = (YangCTimer*) yang_calloc(1, sizeof(YangCTimer));
	yang_create_timer(session->tm_100ms, session, 100, 100);
	session->tm_100ms->doTask = g_yang_doTask;

	session->startRecv = 0;


#if Yang_Enable_Dtls
	yang_create_rtcdtls(session->context.dtls,session->isControlled);
	session->context.dtls->session.sslCallback=&session->context.peerCallback->sslCallback;
	session->context.dtls->session.uid=session->context.peerInfo->uid;
#endif

	if(session->isControlled)
		yang_rtcconn_initPlay(session);


	if (session->context.videoDirection == YangSendonly || session->context.videoDirection==YangSendrecv
	||	session->context.audioDirection == YangSendonly || session->context.audioDirection==YangSendrecv
	)  {

		if (session->push == NULL) {
			session->push = (YangRtcPush*) yang_calloc(1,sizeof(YangRtcPush));
			yang_create_rtcpush(session->push,session->context.audioSsrc, session->context.videoSsrc);
		}
	}
#if Yang_Enable_Dtls
	#if Yang_Enable_Datachannel
	if(session->enableDatachannel){
		if(session->datachannel==NULL){
			session->datachannel=(YangDatachannel*)yang_calloc(sizeof(YangDatachannel),1);
			yang_create_datachannel(session->datachannel,&session->context);
		}
	}
	#endif
#endif
	session->activeState = yangtrue;
	session->isInited = yangtrue;
}

static int32_t yang_rtcconn_on_rtcp_feedback_twcc(YangRtcSession *session,YangRtcpCommon *rtcp) {
#if Yang_Enable_TWCC
	session->context.twcc.decode(&session->context.twcc.session,rtcp);
#endif
	return Yang_Ok;
}

static int32_t yang_rtcconn_on_rtcp_feedback_remb(YangRtcSession *session,	YangRtcpCommon *rtcp) {

	return Yang_Ok;
}

static int32_t yang_rtcconn_dispatch_rtcp(YangRtcSession *session,YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;
	uint16_t rtcpType = rtcp->header.type;
	// For TWCC packet.
	if (YangRtcpType_rtpfb == rtcpType && 15 == rtcp->header.rc) {
		return yang_rtcconn_on_rtcp_feedback_twcc(session, rtcp);
	}

	// For REMB packet.
	if (YangRtcpType_psfb == rtcpType) {

		if (15 == rtcp->header.rc) {
			return yang_rtcconn_on_rtcp_feedback_remb(session, rtcp);
		}
	}

	// Ignore special packet.
	if (YangRtcpType_rr == rtcpType) {
		if (rtcp->rb->ssrc == 0) {
			return err;
		}
	}

	if (session->push	&& Yang_Ok	!= (err = session->push->on_rtcp(&session->context,session->push->pubStream, rtcp))) {
		return yang_error_wrap(err, "handle publish rtcp");
	}

	if (session->play	&& Yang_Ok!= (err = session->play->on_rtcp(&session->context,	session->play->pullStream, rtcp))) {
		return yang_error_wrap(err, "handle play rtcp");
	}

	return err;
}

static void yang_rtcconn_setSsrc(YangRtcSession *session, uint32_t audioSsrc,
		uint32_t videoSsrc) {
	if(session==NULL)
		return;


}

void yang_rtcconn_startStunTimer(YangRtcSession *session) {

	if (session->tm_1s&&!session->tm_1s->isStart)
		yang_timer_start(session->tm_1s);
}

static void yang_rtcconn_startTimers(YangRtcSession *session) {
	if (session->tm_1s&&!session->tm_1s->isStart)
		yang_timer_start(session->tm_1s);

	// if (session->20ms&&!session->20ms->session->isStart)		session->20ms->start();
#if Yang_Enable_TWCC
	YangRtcDirection direction=session->context.direction;
	if (session->context.twccId>0&&(direction==YangRecvonly||direction==YangSendrecv)&&session->tm_100ms&&!session->tm_100ms->isStart)
		yang_timer_start(session->tm_100ms);
#endif
}

static int32_t yang_rtcconn_on_rtcp(YangRtcSession *session, char *data,int32_t nb_data) {
	int32_t i;
	int32_t err = Yang_Ok;
	int32_t nb_unprotected_buf = nb_data;
	YangRtcpCommon *rtcp = NULL;
	char *unprotected_buf = data;
	YangBuffer buffer;

#if Yang_Enable_Dtls
	if ((err = yang_dec_rtcp(&session->context.srtp, data, &nb_unprotected_buf))!= Yang_Ok) {
		if (err == srtp_err_status_replay_fail)	return Yang_Ok;
		return yang_error_wrap(err, "rtcp unprotect");
	}
#endif

	yang_init_buffer(&buffer, unprotected_buf, nb_unprotected_buf);

	if (Yang_Ok != (err = yang_decode_rtcpCompound(&session->rtcp_compound, &buffer))) {
		return yang_error_wrap(err, "decode rtcp plaintext=%u",	nb_unprotected_buf);
	}

	for (i = 0; i < session->rtcp_compound.rtcpVector.vsize; i++) {
		rtcp = &session->rtcp_compound.rtcpVector.payload[i];
		err = yang_rtcconn_dispatch_rtcp(session, rtcp);
		if (Yang_Ok != err) {
			yang_rtcpCompound_clear(&session->rtcp_compound);
			return yang_error_wrap(err,
					"cipher=%u, plaintext=%u,  rtcp=(%u,%u,%u,%u)", nb_data,
					nb_unprotected_buf, rtcp->nb_data, rtcp->header.rc,
					rtcp->header.type, rtcp->ssrc);
		}
	}
	yang_rtcpCompound_clear(&session->rtcp_compound);
	return err;

}

static yangbool yang_rtcconn_isAlive(YangRtcSession* session){
	if(session==NULL || session->context.state!=Yang_Conn_State_Connected)
		return yangfalse;

	return session->lastStunTime + session->sessionTimeout > yang_get_system_time();
}


static void yang_rtcconn_startudp(YangRtcSession *session) {
	yang_rtcconn_init(session);
	yang_rtcconn_setSsrc(session, session->context.audioSsrc,session->context.videoSsrc);
}



static int32_t yang_rtcconn_onVideo(YangRtcSession *session,  YangPushData *videoData) {

#if Yang_Enable_RTC_Video
#if Yang_Enable_Dtls
	if (session==NULL||videoData==NULL||session->context.state!=Yang_Conn_State_Connected||session->context.dtls->session.state!=YangDtlsStateClientDone)
		return Yang_Ok;
#else
	if (session==NULL||videoData==NULL||session->context.state!=Yang_Conn_State_Connected)	return Yang_Ok;
#endif
	if(session->push)
		return session->push->on_video(&session->context,session->push->pubStream,videoData);
#endif
	return Yang_Ok;

}

static int32_t yang_rtcconn_onAudio(YangRtcSession *session, YangPushData *audioData) {
#if Yang_Enable_Dtls
	if (session==NULL || session->context.state!=Yang_Conn_State_Connected ||session->context.dtls->session.state!=YangDtlsStateClientDone || audioData==NULL)
		return Yang_Ok;
#else
	if (session==NULL||session->context.state!=Yang_Conn_State_Connected||audioData==NULL)
		return Yang_Ok;
#endif
#if Yang_Enable_RTC_Audio
	if(session->push)
		return session->push->on_audio(&session->context,session->push->pubStream,audioData);

#endif
	return Yang_Ok;

}

static int32_t yang_rtcconn_onMessage(YangRtcSession *session, YangFrame *p) {
#if Yang_Enable_Dtls
#if Yang_Enable_Datachannel
	if(session==NULL||p==NULL||session->context.state!=Yang_Conn_State_Connected
			||session->context.dtls->session.isRecvAlert||session->context.dtls->session.state!=YangDtlsStateClientDone)
		return Yang_Ok;

	if(session->datachannel&&session->datachannel->send_message) session->datachannel->send_message(session->datachannel->context,p);
#endif
#endif
	return Yang_Ok;
}



static int32_t yang_rtcconn_notify(YangRtcSession *session, YangRtcMessageType mess) {
#if Yang_Enable_RTC_Video
	YangPullTrackVideo *videotrack;
	if(session==NULL||session->play==NULL||session->play->pullStream->videoTrack==NULL)
		return ERROR_RTC_CALLBACK;

	videotrack=session->play->pullStream->videoTrack;
	if (mess == YangRTC_Decoder_Error) {
		if(videotrack) videotrack->setRequestKeyframeState(&videotrack->session,yangfalse);
		return yang_send_rtcp_fb_pli(&session->context, videotrack->session.track.ssrc);
	}
#endif
	return Yang_Ok;
}

static void yang_rtcconn_receive(YangRtcSession *session, char *data, int32_t size) {
	uint8_t bt;
	int32_t err = 0;
	if (session==NULL||!session->activeState)
		return;

	bt=(uint8_t)data[0];
	session->lastStunTime=yang_get_system_time();

	//is rtp rtcp
	if(bt > 127 && bt < 192 && size>12){

		bt=(uint8_t)data[1];
		if (bt>= 192 && bt <= 223) {//rtcp
			yang_rtcconn_on_rtcp(session, data, size);
			return;
		}
		//rtp
		session->startRecv = 1;
		if (session->play)	session->play->on_rtp(&session->context, session->play->pullStream,data, size);
		return;
	}
	//is stun
	if(size>0&&(bt==0x00 || bt==0x01)){
		if(session->ice.onStunRtp&&(err=session->ice.onStunRtp(&session->ice.session,data,size))!=Yang_Ok){
			if(err==ERROR_RTC_ICE_STUN) {
				session->context.state=Yang_Conn_State_Failed;
				yang_onConnectionStateChange(session,Yang_Conn_State_Failed);
			}
			return;
		}

		if(!session->isControlled&&bt==0x01&&data[1]==0x01&&session->ice.session.iceState==YangIceSuccess){

#if Yang_Enable_Dtls
			if (!session->isSendDtls) {
				if (session->context.dtls->startHandShake(&session->context.dtls->session)) yang_error("dtls start handshake failed!");
				session->isSendDtls = yangtrue;
			}
#else
			if(session->context.state==Yang_Conn_State_Connecting)	goto client_sucess;

#endif
		}

		return;
	}

	//is dtls
		if (bt > 19 && bt < 64) {

#if Yang_Enable_Dtls
			if(session->context.dtls==NULL) return;
#if Yang_Enable_Datachannel
			if (session->context.dtls->processData(session->datachannel,&session->context.dtls->session, data,size) == Yang_Ok && session->context.state == Yang_Conn_State_Connecting) {
#else
			if (session->context.dtls->processData(NULL,&session->context.dtls->session, data,size) == Yang_Ok && session->context.state == Yang_Conn_State_Connecting) {
#endif

				if(session->isControlled){

                    if(session->ice.session.iceState==YangIceNew){
                        session->ice.session.iceState=YangIceSuccess;
						session->ice.onIceStateChange(&session->ice.session,YangIceSuccess);
					}
					if(session->context.dtls->session.handshake_done ) {
						session->context.state = Yang_Conn_State_Connected;
						yang_onConnectionStateChange(session,Yang_Conn_State_Connected);
						yang_rtcconn_startTimers(session);
					}

					return;
				}else if (session->context.dtls->session.state == YangDtlsStateClientDone) {
					goto client_sucess;
				}
			}

#else
		if(session->isControlled){
			void* context=session->context.streamConfig->sslCallback.context;
			session->context.streamConfig->sslCallback.sslAlert(context,session->context.streamConfig->uid,"warning","CN");
		}
#endif
		}

	client_sucess:
	if(session->context.state == Yang_Conn_State_Connecting) {
		session->context.state = Yang_Conn_State_Connected;
		yang_onConnectionStateChange(session,Yang_Conn_State_Connected);
		yang_rtcconn_startTimers(session);
		if (session->context.peerCallback->rtcCallback.sendRequest)
				session->context.peerCallback->rtcCallback.sendRequest(session->context.peerCallback->rtcCallback.context,session->context.peerInfo->uid, 0,Yang_Req_Connected);
	}
	return;
}

static void yang_rtcconn_on_ice(YangRtcSession *session,char* remoteIp,int32_t port) {
	if(session==NULL || remoteIp==NULL) return;
	session->context.sock->updateRemoteAddress(&session->context.sock->session,remoteIp,port);
}


static void yang_rtcconn_turn_receive(void *psession, char *data, int32_t size){
	YangRtcSession *session=(YangRtcSession*)psession;
	if(psession==NULL || data==NULL)
		return;

	yang_rtcconn_receive(session,data,size);
}

static int32_t yang_rtcconn_turn_sendData(YangRtcSocketSession *psession, char *data, int32_t nb) {
	YangRtcConnection* conn=(YangRtcConnection*)psession->user;
	YangTurnConnection* turn;
	if (psession == NULL ||  data==NULL)
		return ERROR_RTC_SOCKET;

	turn=conn->session->ice.session.turnconn;

	if(turn&&turn->sendData) {
		return turn->sendData(&turn->session,conn->session->ice.session.turnUid,data,nb);
	}
	return  ERROR_RTC_TURN;
}

static void yang_rtcconn_turn_start(void *psession){
	YangRtcSession *session=(YangRtcSession*)psession;
	if(psession==NULL)
		return;

	session->context.sock->write=yang_rtcconn_turn_sendData;
	yang_rtcconn_startudp(session);
	g_yang_startStunTimer(session->context.sock->session.user);
}

static int32_t yang_rtcconn_createOffer(YangRtcSession *session, char **psdp){
	int32_t localport;
	if(session==NULL) return ERROR_RTC_CONNECT;

	localport=session->context.peerInfo->rtc.rtcLocalPort;

	return yang_sdp_genLocalSdp(session,localport, psdp,session->context.audioDirection,session->context.videoDirection,YangIceModeFull);
}

static int32_t yang_rtcconn_createAnswer(YangRtcSession *session, char *answer){
	int32_t localport;
	if(session==NULL || answer==NULL)
		return ERROR_RTC_CONNECT;

	session->context.peerInfo->rtc.isControlled=yangtrue;
	session->isControlled=yangtrue;

	localport=session->context.peerInfo->rtc.rtcLocalPort;

	return yang_sdp_genLocalSdp2(session,localport, answer,session->context.audioDirection,session->context.videoDirection,YangIceModeFull);
}


static int32_t yang_rtcconn_setRemoteDescription(YangRtcSession* session,char* sdpstr){
	int32_t err = Yang_Ok;
	YangSdp sdp;
	if(session==NULL || sdpstr==NULL)
		return ERROR_RTC_PEERCONNECTION;

	session->isControlled=session->context.peerInfo->rtc.isControlled;
	yang_memset(&sdp,0,sizeof(YangSdp));
	yang_create_rtcsdp(&sdp);

	if((err=yang_rtcsdp_parse(&sdp,sdpstr))!=Yang_Ok){
		yang_error("sdp parse error!");
	}

	if(err==Yang_Ok&&(err=yang_sdp_parseRemoteSdp(session,&sdp))!=Yang_Ok){
		yang_error("parseRemoteSdp error!");
	}
	yang_destroy_rtcsdp(&sdp);

	if(!session->isControlled)
		yang_rtcconn_initPlay(session);

	session->ice.session.stun.createRequestStunPacket(session,session->ice.session.remoteIcePwd);
	return err;
}

static int32_t yang_rtcconn_setLocalDescription(YangRtcSession* session,char* sdp){
	int32_t err=Yang_Ok;
	if(session==NULL || sdp==NULL)
		return ERROR_RTC_PEERCONNECTION;

	if(session->iceInited)
		return err;

	err=yang_create_rtcsocket(session->context.sock,session->context.peerInfo->familyType,(YangSocketProtocol)session->context.peerInfo->rtc.rtcSocketProtocol,session->context.peerInfo->rtc.rtcLocalPort);
	while(yangtrue){
		if(session->context.sock->listen&&session->context.sock->listen(&session->context.sock->session)==Yang_Ok)
			break;
		++session->context.peerInfo->rtc.rtcLocalPort;
		yang_addr_updatePort(&session->context.sock->session.local_addr,session->context.peerInfo->rtc.rtcLocalPort);
		if (session->context.peerInfo->rtc.rtcLocalPort > 65000)
			session->context.peerInfo->rtc.rtcLocalPort = 15000;
	}
	yang_trace("\nstartRtc,port=%d",session->context.peerInfo->rtc.rtcLocalPort);
    if(err!=Yang_Ok)
        return yang_error_wrap(err,"setLocalDescription error!");

	session->context.sock->session.rtcSession=session;
	yang_rtcconn_startudp(session);
	session->context.sock->start(&session->context.sock->session);

	if(session->ice.startIceAgent)		{
		session->ice.startIceAgent(&session->ice.session);
	}

	if(session->context.state==Yang_Conn_State_New) {
			session->context.state=Yang_Conn_State_Connecting;
			yang_onConnectionStateChange(session,Yang_Conn_State_Connecting);
	}
	session->iceInited=yangtrue;
	return err;

}


static yangbool yang_rtcconn_isConnected(YangRtcSession* session){
	if(session==NULL)
		return yangfalse;

	return session->context.state == Yang_Conn_State_Connected;
}

static int32_t yang_rtcconn_addIceCandidate(YangRtcSession *session,char* candidateStr){
	if(session==NULL || candidateStr==NULL ||session->ice.addIceCandidate==NULL)
		return ERROR_RTC_ICE;

	return session->ice.addIceCandidate(&session->ice.session,candidateStr);
}

static void yang_rtcconn_updateAddress(void* user,YangIpAddress* address){
	YangRtcConnection* conn=(YangRtcConnection*)user;
	if(user==NULL||address==NULL)
		return;

	conn->session->ice.addIceCandidate2(&conn->session->ice.session,address);
}


static int32_t yang_rtcconn_createDataChannel(YangRtcSession* session){
	if(session==NULL)
		return ERROR_RTC_CONNECT;
#if Yang_Enable_Datachannel
	session->enableDatachannel=yangtrue;
	if(session->datachannel==NULL){
		session->datachannel=(YangDatachannel*)yang_calloc(sizeof(YangDatachannel),1);
		yang_create_datachannel(session->datachannel,&session->context);
	}
#endif
	return Yang_Ok;
}

static int32_t yang_rtcconn_addAudioTrack(YangRtcSession *session,YangAudioCodec codec){
	if(session==NULL)
		return ERROR_RTC_CONNECT;

	session->context.audioEncodeCodec=codec;
	session->context.enableAudioTrack=yangtrue;

	switch(codec){
	case Yang_AED_OPUS:session->context.audioPayloadType = YangOpusPayloadType;break;
	case Yang_AED_PCMU:{
		session->context.audioPayloadType = YangPcmuPayloadType;
		break;
	}
	case Yang_AED_PCMA:{
		session->context.audioPayloadType = YangPcmaPayloadType;
		break;
	}

	default:break;
	}

	return Yang_Ok;
}

static int32_t yang_rtcconn_addVideoTrack(YangRtcSession *session,YangVideoCodec codec){
	if(session==NULL)
		return ERROR_RTC_CONNECT;

	session->context.videoEncodeCodec=codec;
	session->context.enableVideoTrack=yangtrue;
	return Yang_Ok;
}

static int32_t yang_rtcconn_addTransceiver(YangRtcSession *session,YangMediaTrack media,YangRtcDirection direction){
	if(session==NULL)
		return ERROR_RTC_CONNECT;

	if(media==YangMediaAudio)
		session->context.audioDirection=direction;

	if(media==YangMediaVideo)
		session->context.videoDirection=direction;
	return Yang_Ok;
}


int32_t yang_create_rtcConnection(YangRtcConnection* conn,YangPeerInfo* peerInfo,YangPeerCallback* peerCallback){
	 YangRtcSession* session=NULL;

	if (conn == NULL ||peerCallback == NULL || peerInfo==NULL)
		return ERROR_RTC_CONNECT;

    session=(YangRtcSession*)yang_calloc(sizeof(YangRtcSession),1);

	conn->session=session;
	yang_create_rtcContext(&session->context);


	yang_create_ice(&session->ice,session,peerInfo,peerCallback,yang_rtcconn_turn_receive,yang_rtcconn_turn_start);
	yang_memset(&session->rtcp_compound,0,sizeof(YangRtcpCompound));
	yang_create_rtcpCompound(&session->rtcp_compound);


	session->context.peerCallback = peerCallback;
	session->context.sock->session.user = conn;
	session->context.sock->session.receive = g_session_receive;
	session->context.sock->session.startStunTimer = g_yang_startStunTimer;
	session->context.sock->session.updateAddress=yang_rtcconn_updateAddress;

	session->context.peerInfo=peerInfo;

	session->iceInited=yangfalse;
	session->isInited = yangfalse;
	session->isControlled=yangfalse;

	conn->close=yang_rtcconn_close;

	conn->on_video=yang_rtcconn_onVideo;
	conn->on_audio=yang_rtcconn_onAudio;
	conn->on_message=yang_rtcconn_onMessage;
	conn->notify=yang_rtcconn_notify;
	conn->isAlive=yang_rtcconn_isAlive;

	conn->receive=yang_rtcconn_receive;

	conn->addIceCandidate=yang_rtcconn_addIceCandidate;
	conn->updateCandidateAddress=yang_rtcconn_on_ice;
	conn->onConnectionStateChange=yang_onConnectionStateChange;
	conn->setLocalDescription=yang_rtcconn_setLocalDescription;
	conn->setRemoteDescription=yang_rtcconn_setRemoteDescription;
	conn->createOffer=yang_rtcconn_createOffer;
	conn->createAnswer=yang_rtcconn_createAnswer;

	conn->createDataChannel=yang_rtcconn_createDataChannel;

	conn->isConnected=yang_rtcconn_isConnected;


	conn->addAudioTrack=yang_rtcconn_addAudioTrack;
	conn->addVideoTrack=yang_rtcconn_addVideoTrack;
	conn->addTransceiver=yang_rtcconn_addTransceiver;

	return Yang_Ok;

}

void yang_destroy_rtcConnection(YangRtcConnection *conn) {
	YangRtcSession* session;
	if (conn == NULL)
		return;

	session=(YangRtcSession*)conn->session;
	session->activeState = yangfalse;
	//yang_stop(session->20ms);
	yang_timer_stop(session->tm_1s);
	yang_timer_stop(session->tm_100ms);
	yang_destroy_timer(session->tm_1s);
	yang_destroy_timer(session->tm_100ms);
	// yang_free(session->20ms);
	yang_free(session->tm_1s);
	yang_free(session->tm_100ms);

	yang_destroy_rtcpush(session->push);
	yang_free(session->push);

	yang_destroy_rtcpull(session->play);
	yang_free(session->play);

#if Yang_Enable_Datachannel
	yang_destroy_datachannel(session->datachannel);
	yang_free(session->datachannel);
#endif

	yang_destroy_ice(&session->ice);
	yang_destroy_rtcContext(&session->context);

	if(session->playRtpBuffer){
		yang_destroy_rtpBuffer(session->playRtpBuffer);
		yang_free(session->playRtpBuffer);
	}
	yang_destroy_rtcpCompound(&session->rtcp_compound);

	yang_free(conn->session);
}

