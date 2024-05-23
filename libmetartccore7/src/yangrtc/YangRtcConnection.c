//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangice/YangRtcSocket.h>
#include <yangice/YangRtcStun.h>

#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangPushH264.h>
#include <yangrtc/YangPushH265.h>
#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangPlayStream.h>
#include <yangrtc/YangPushStream.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtcpCompound.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavctype.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangSRtp.h>

#include <yangsdp/YangSdp.h>
#include <yangsdp/YangAnswerSdp.h>


static void g_session_receive(char *data, int32_t nb_data, void *user) {
	if (user == NULL)		return;
	YangRtcConnection *conn = (YangRtcConnection*) user;
	conn->receive(conn->session,data,nb_data);

}

void yang_rtcconn_startStunTimer(YangRtcSession *session);

static void g_yang_startStunTimer(void *user) {
	if (user == NULL)		return;
	YangRtcConnection *conn = (YangRtcConnection*) user;
	yang_rtcconn_startStunTimer(conn->session);

}

static void yang_onConnectionStateChange(YangRtcSession *session,YangRtcConnectionState state){
	if(session->context.streamConfig&&session->context.streamConfig->iceCallback.onConnectionStateChange){
						session->context.streamConfig->iceCallback.onConnectionStateChange(
								session->context.streamConfig->iceCallback.context,
								session->context.streamConfig->uid,
								state);
	}
}

static void g_yang_doTask(int32_t taskId, void *user) {
	if (user == NULL)	return;
	YangRtcSession *session = (YangRtcSession*) user;

	if (!session->isControlled && session->isSendStun && taskId == 1) {

		if (session->context.stun.data&& session->context.stun.nb > 0) {

			if(session->context.sock->write(&session->context.sock->session,session->context.stun.data, session->context.stun.nb)!=Yang_Ok){
				yang_error("send stun fail!");
			}
			if(session->context.state==Yang_Conn_State_New) {
				session->context.state=Yang_Conn_State_Connecting;
				yang_onConnectionStateChange(session,Yang_Conn_State_Connecting);
			}
		}

	}
	if(session->context.state!=Yang_Conn_State_Connected) return;


	if (session->startRecv&&session->play) {

		if (taskId == 1) {

			if (session->play->send_rtcp_rr(&session->context,session->play->playStream))	yang_error("RTCP Error:RR err ");
#if Yang_Enable_RtcpXr
			if (session->context.streamConfig->streamDirection==YangRecvonly && session->play->send_rtcp_xr_rrtr(&session->context,session->play->playStream))
				yang_error("RTCP Error:XR err ");
#endif

		}

		if (taskId == 100) {
#if Yang_Enable_TWCC
			if (session->play->send_periodic_twcc(&session->context,session->play->playStream))
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
							session->play->update_rtt(&session->context,session->play->playStream,session->play->playStream->videoTrack->session.track.ssrc,session->context.stats.recvStats.video.rtt);
#endif

			}
		}


}



static void yang_rtcconn_init(YangRtcSession *session, YangRtcDirection role) {
	if (session == NULL)	return;

	session->videoCodec =(YangVideoCodec) session->context.avinfo->video.videoEncoderType;
	session->isSendDtls = 0;

	session->sessionTimeout=session->context.avinfo->rtc.sessionTimeout;
    if (role == YangSendonly || role==YangSendrecv)  {
#if Yang_Enable_RTC_Audio
    	if(session->pushAudio==NULL){
    		session->pushAudio=(YangPushAudio*) yang_calloc(1,sizeof(YangPushAudio));
    	}
#endif
#if Yang_Enable_RTC_Video

    	session->pushH264 = NULL;
        if (session->videoCodec == Yang_VED_H264) {
            session->pushH264 = (YangPushH264*) yang_calloc(1,sizeof(YangPushH264));
        }
	#if	Yang_Enable_H265_Encoding
        session->pushH265 = NULL;
        if (session->videoCodec == Yang_VED_H265) {
            session->pushH265 = (YangPushH265*) yang_calloc(1,sizeof(YangPushH265));
        }
	#endif


#endif
    }
	//  session->20ms
	session->tm_1s = (YangCTimer*) yang_calloc(1, sizeof(YangCTimer));
	yang_create_timer(session->tm_1s, session, 1, 1000);
	session->tm_1s->doTask = g_yang_doTask;

	session->tm_100ms = (YangCTimer*) yang_calloc(1, sizeof(YangCTimer));
	yang_create_timer(session->tm_100ms, session, 100, 100);
	session->tm_100ms->doTask = g_yang_doTask;

	session->startRecv = 0;
	session->isSendStun = yangfalse;

#if Yang_Enable_Dtls
	yang_create_rtcdtls(session->context.dtls,session->isControlled);
	session->context.dtls->session.sslCallback=&session->context.streamConfig->sslCallback;
	session->context.dtls->session.uid=session->context.streamConfig->uid;
#endif


	if (role == YangRecvonly || role==YangSendrecv) {
		if(session->playRtpBuffer == NULL) {
			session->playRtpBuffer = (YangRtpBuffer*) yang_calloc(1,sizeof(YangRtpBuffer));
			yang_create_rtpBuffer(session->playRtpBuffer, 1500, kRtpPacketSize);
		}
		if (session->play == NULL) {
			session->play = (YangRtcPlay*) yang_calloc(1,sizeof(YangRtcPlay));
			yang_create_rtcplay(&session->context, session->play,session->playRtpBuffer);

		}
	}

	if (role == YangSendonly || role==YangSendrecv)  {

#if Yang_Enable_RTC_Video
		if(session->pushVideoRtpBuffer == NULL) {
			session->pushVideoRtpBuffer = (YangRtpBuffer*) yang_calloc(1,sizeof(YangRtpBuffer));
			yang_create_rtpBuffer(session->pushVideoRtpBuffer, 1400, kRtpPacketSize);
		}
#endif
#if Yang_Enable_RTC_Audio
		if(session->pushAudio){
			if(session->pushAudioRtpBuffer == NULL) {
					session->pushAudioRtpBuffer = (YangRtpBuffer*) yang_calloc(1,sizeof(YangRtpBuffer));
					yang_create_rtpBuffer(session->pushAudioRtpBuffer, 100, kRtpPacketSize);
			}
			yang_create_pushAudio(session->pushAudio, session->pushAudioRtpBuffer);
		}
#endif

#if Yang_Enable_RTC_Video
		if (session->pushH264) {
			yang_create_pushH264(session->pushH264, session->pushVideoRtpBuffer);
		}
	#if	Yang_Enable_H265_Encoding
		if (session->pushH265) {
			yang_create_pushH265(session->pushH265,  session->pushVideoRtpBuffer);
		}
	#endif

#endif
		if (session->push == NULL) {
			session->push = (YangRtcPush*) yang_calloc(1,sizeof(YangRtcPush));
			yang_create_rtcpush(session->push,session->context.audioSsrc, session->context.videoSsrc);
		}
	}

	session->activeState = yangtrue;
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

	if (session->play	&& Yang_Ok!= (err = session->play->on_rtcp(&session->context,	session->play->playStream, rtcp))) {
		return yang_error_wrap(err, "handle play rtcp");
	}

	return err;
}

static void yang_rtcconn_setSsrc(YangRtcSession *session, uint32_t audioSsrc,
		uint32_t videoSsrc) {
	if(session==NULL) return;
#if Yang_Enable_RTC_Audio
	if(session->pushAudio)
		session->pushAudio->push->audioSsrc=audioSsrc;
#endif
#if Yang_Enable_RTC_Video
	if (session->pushH264)
			session->pushH264->push->videoSsrc = videoSsrc;
	#if	Yang_Enable_H265_Encoding
	if (session->pushH265)
		session->pushH265->push->videoSsrc = videoSsrc;
	#endif

	#if Yang_Enable_AV1_Encoding

	#endif

	#if	Yang_Enable_MJpeg_Encoding


	#endif
#endif



}

 void yang_rtcconn_startStunTimer(YangRtcSession *session) {

	if (session->tm_1s&&!session->tm_1s->isStart)
		yang_timer_start(session->tm_1s);
	session->isSendStun = yangtrue;
}

static void yang_rtcconn_startTimers(YangRtcSession *session) {
	if (session->tm_1s&&!session->tm_1s->isStart)		yang_timer_start(session->tm_1s);

	// if (session->20ms&&!session->20ms->session->isStart)		session->20ms->start();
#if Yang_Enable_TWCC
	YangStreamDirection opt=session->context.streamConfig->streamDirection;
	if (session->context.twccId>0&&(opt==YangRecvonly||opt==YangSendrecv)&&session->tm_100ms&&!session->tm_100ms->isStart)
		yang_timer_start(session->tm_100ms);
#endif
}

static int32_t yang_rtcconn_on_rtcp(YangRtcSession *session, char *data,int32_t nb_data) {
	int32_t err = Yang_Ok;
	int32_t nb_unprotected_buf = nb_data;
#if Yang_Enable_Dtls
	if ((err = yang_dec_rtcp(&session->context.srtp, data, &nb_unprotected_buf))!= Yang_Ok) {
		if (err == srtp_err_status_replay_fail)	return Yang_Ok;
		return yang_error_wrap(err, "rtcp unprotect");
	}
#endif
	char *unprotected_buf = data;
	YangBuffer buffer;
	yang_init_buffer(&buffer, unprotected_buf, nb_unprotected_buf);

	if (Yang_Ok != (err = yang_decode_rtcpCompound(&session->rtcp_compound, &buffer))) {
		return yang_error_wrap(err, "decode rtcp plaintext=%u",	nb_unprotected_buf);
	}

	YangRtcpCommon *rtcp = NULL;
	for (int i = 0; i < session->rtcp_compound.rtcpVector.vsize; i++) {
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
	if(session==NULL||session->context.state!=Yang_Conn_State_Connected) return yangfalse;
	return session->lastStunTime + session->sessionTimeout > yang_get_system_time();
}


static void yang_rtcconn_startudp(YangRtcSession *session) {
	yang_rtcconn_init(session, session->context.streamConfig->direction);
	yang_rtcconn_setSsrc(session, session->context.audioSsrc,session->context.videoSsrc);
}


static int32_t yang_rtcconn_send_video_meta(YangRtcSession *session, YangFrame *p) {
#if Yang_Enable_RTC_Video
	if (session->pushH264)
		return session->pushH264->on_spspps(session,session->pushH264->push, p);
	#if	Yang_Enable_H265_Encoding
	if (session->pushH265)
		return session->pushH265->on_spspps(session,session->pushH265->push, p);
	#endif

#endif
	return Yang_Ok;
}

static int32_t yang_rtcconn_onVideo(YangRtcSession *session, YangFrame *p) {
#if Yang_Enable_RTC_Video
	#if Yang_Enable_Dtls
	if (session==NULL || p==NULL || session->context.state!=Yang_Conn_State_Connected||session->context.dtls->session.state!=YangDtlsStateClientDone)	return Yang_Ok;
	#else
	if (session==NULL|| p==NULL || session->context.state!=Yang_Conn_State_Connected)	return Yang_Ok;
	#endif
	if (p->frametype == YANG_Frametype_Spspps)
		return yang_rtcconn_send_video_meta(session, p);
	if (session->pushH264)
		return session->pushH264->on_video(session, session->pushH264->push, p);
	#if	Yang_Enable_H265_Encoding
	if (session->pushH265)
		return session->pushH265->on_video(session, session->pushH265->push, p);
	#endif

#endif
	return Yang_Ok;

}

static int32_t yang_rtcconn_onAudio(YangRtcSession *session, YangFrame *p) {

#if Yang_Enable_Dtls
	if (session==NULL||p==NULL||session->context.state!=Yang_Conn_State_Connected||session->context.dtls->session.state!=YangDtlsStateClientDone)	return Yang_Ok;
#else
	if (session==NULL||p==NULL||session->context.state!=Yang_Conn_State_Connected)	return Yang_Ok;
#endif
#if Yang_Enable_RTC_Audio
	if (session->pushAudio)
		return session->pushAudio->on_audio(session, session->pushAudio->push, p);
#endif

	return Yang_Ok;

}

static int32_t yang_rtcconn_onMessage(YangRtcSession *session, YangFrame *p) {
#if Yang_Enable_Dtls
#if Yang_Enable_Datachannel
	if(session==NULL || p==NULL ||session->context.state!=Yang_Conn_State_Connected||
			session->context.dtls->session.isRecvAlert||session->context.dtls->session.state!=YangDtlsStateClientDone)
		return Yang_Ok;

	if(session->datachannel&&session->datachannel->send_message) session->datachannel->send_message(session->datachannel->context,p);
#endif
#endif
	return Yang_Ok;
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


static int32_t yang_rtcconn_notify(YangRtcSession *session, YangRtcMessageType mess) {
#if Yang_Enable_RTC_Video
	if(session==NULL||session->play==NULL||session->play->playStream->videoTrack==NULL) return ERROR_RTC_CALLBACK;
	YangPlayTrackVideo *videotrack=session->play->playStream->videoTrack;
	if (mess == YangRTC_Decoder_Error) {
		if(videotrack) videotrack->setRequestKeyframeState(&videotrack->session,yangfalse);
		return yang_send_rtcp_fb_pli(&session->context, videotrack->session.track.ssrc);
	}
#endif
	return Yang_Ok;
}

static void yang_rtcconn_receive(YangRtcSession *session, char *data, int32_t size) {
	if (session==NULL||!session->activeState)	return;

	uint8_t bt=(uint8_t)data[0];
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
		if (session->play)	session->play->on_rtp(&session->context, session->play->playStream,data, size);
		return;
	}
	//is stun
	if(size>0&&(bt==0x00 || bt==0x01)){
		int32_t err = 0;
		if(bt==0x00){
			YangStunPacket request;
			yang_memset(&request,0,sizeof(YangStunPacket));

			if ((err = session->ice.session.stun.decode(&request,data, size)) != Yang_Ok) {
				yang_error("decode stun packet failed");
				session->context.state=Yang_Conn_State_Failed;
				yang_onConnectionStateChange(session,Yang_Conn_State_Failed);
				return;
			}
			if ((err =session->ice.session.stun.createResponseStunPacket(&request,session)) != Yang_Ok) {
				yang_error("create response stun packet failed");
				return;
			}
			if(session->context.state==Yang_Conn_State_New) {
				session->context.state=Yang_Conn_State_Connecting;
				yang_onConnectionStateChange(session,Yang_Conn_State_Connecting);
			}

		}else if(bt==0x01&&data[1]==0x01){
			if ((err = session->ice.session.stun.decode2(data, size)) != Yang_Ok) {
				yang_error("decode stun packet failed");
				return;
			}

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
					if( session->context.dtls->session.handshake_done ) {
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
		if (session->context.streamConfig&&session->context.streamConfig->rtcCallback.sendRequest)
				session->context.streamConfig->rtcCallback.sendRequest(session->context.streamConfig->rtcCallback.context,session->context.streamConfig->uid, 0,Yang_Req_Connected);
	}
	return;
}

static void yang_rtcconn_on_ice(YangRtcSession *session,char* remoteIp,int32_t port) {
	if(session==NULL || remoteIp==NULL) return;
	session->context.sock->updateRemoteAddress(&session->context.sock->session,remoteIp,port);
}
static int32_t yang_rtcconn_addAudioTrack(YangRtcSession *session,YangAudioCodec codec){
	if(session==NULL) return ERROR_RTC_CONNECT;
	session->context.audioCodec=codec;
	session->context.enableAudioTrack=yangtrue;
	if(codec==Yang_AED_OPUS)
		session->audioPayloadType = YangAudioPayloadType;

	return Yang_Ok;
}

static int32_t yang_rtcconn_addVideoTrack(YangRtcSession *session,YangVideoCodec codec){
	if(session==NULL) return ERROR_RTC_CONNECT;
	session->context.videoCodec=codec;
	session->context.enableVideoTrack=yangtrue;
	return Yang_Ok;
}

static int32_t yang_rtcconn_addTransceiver(YangRtcSession *session,YangRtcDirection direction){
	if(session==NULL) return ERROR_RTC_CONNECT;
	session->context.direction=direction;
	return Yang_Ok;
}

static int32_t yang_rtcconn_createDataChannel(YangRtcSession* session){
	if(session==NULL) return ERROR_RTC_CONNECT;
	session->enableDatachannel=yangtrue;
#if Yang_Enable_Datachannel
	if(session->datachannel==NULL){
		session->datachannel=(YangDatachannel*)yang_calloc(sizeof(YangDatachannel),1);
		yang_create_datachannel(session->datachannel,&session->context);
	}
#endif
	return Yang_Ok;
}


 int32_t yang_rtcconn_getRemoteSdp(YangRtcSession *session, char *sdpstr){
	if(session==NULL) return ERROR_RTC_SDP;
	int32_t err = Yang_Ok;

	YangSdp sdp;
	yang_memset(&sdp,0,sizeof(YangSdp));
	yang_create_rtcsdp(&sdp);
#if Yang_Enable_RTC_Audio
	if(session->remote_audio==NULL) session->remote_audio=(YangAudioParam*)yang_calloc(sizeof(YangAudioParam),1);
#endif
#if Yang_Enable_RTC_Video
	if(session->remote_video==NULL) session->remote_video=(YangVideoParam*)yang_calloc(sizeof(YangVideoParam),1);
#endif
	if((err=yang_rtcsdp_parse(&sdp,sdpstr))!=Yang_Ok){
		yang_error("sdp parse error!");
	}
	if((err=yang_sdp_parseRemoteSdp(session,&sdp))!=Yang_Ok){
		yang_error("parseRemoteSdp error!");
	}
	yang_destroy_rtcsdp(&sdp);

	return err;
}




static void yang_rtcconn_turn_receive(void *psession, char *data, int32_t size){
	if(psession==NULL || data==NULL) return;
	YangRtcSession *session=(YangRtcSession*)psession;
	yang_rtcconn_receive(session,data,size);
}

static int32_t yang_rtcconn_turn_sendData(YangRtcSocketSession *psession, char *data, int32_t nb) {
	if (psession == NULL ||  data==NULL)	return ERROR_RTC_SOCKET;
	YangRtcConnection* conn=(YangRtcConnection*)psession->user;
	YangTurnConnection* turn=conn->session->ice.session.turnconn;

	if(turn&&turn->sendData) {
		return turn->sendData(&turn->session,conn->session->ice.session.uid,data,nb);
	}
	return  ERROR_RTC_TURN;
}

static int32_t yang_rtcconn_createOffer(YangRtcSession *session, char **psdp){
	if(session==NULL) return ERROR_RTC_CONNECT;

	int32_t localport=session->context.streamConfig->localPort;
	YangRtcDirection role=session->context.streamConfig->direction;
	int32_t err = Yang_Ok;
	if(!session->context.avinfo->rtc.iceUsingLocalIp&&(err=session->ice.initIce(&session->ice.session))!=Yang_Ok){
		return yang_error_wrap(err,"ice request fail(%s)",session->ice.session.candidateType==YangIceStun?"STUN":"TURN");
	}

	return yang_sdp_genLocalSdp(session,localport, psdp,role);
}


static int32_t yang_rtcconn_createAnswer(YangRtcSession *session, char *answer){
	if(session==NULL || answer==NULL) return ERROR_RTC_CONNECT;
	session->context.streamConfig->isControlled=yangtrue;
	session->isControlled=yangtrue;

	int32_t localport=session->context.streamConfig->localPort;
	YangRtcDirection role=session->context.streamConfig->direction;
	return yang_sdp_genLocalSdp2(session,localport, answer,role);
}

static int32_t yang_rtcconn_createHttpAnswer(YangRtcSession *session, char *answer){
	if(session==NULL || answer==NULL) return ERROR_RTC_CONNECT;
	return yang_sdp_getAnswerSdp(session, answer);
}


static int32_t yang_rtcconn_setLocalDescription(YangRtcSession* session,char* sdp){
	if(session==NULL || sdp==NULL) return ERROR_RTC_PEERCONNECTION;

	int32_t err=Yang_Ok;
	yang_trace("\nstartRtc,port=%d",session->context.streamConfig->localPort);
	if(session->context.avinfo->sys.mediaServer==Yang_Server_P2p&&!session->context.avinfo->rtc.iceUsingLocalIp){

		if(session->ice.session.candidateType>YangIceHost)
			yang_trace("\nstart ice %s",session->ice.session.candidateType==YangIceTurn?"TRUN":"STUN");


		if(session->ice.session.candidateType==YangIceTurn&&session->ice.session.iceState==YangIceSuccess)
			return Yang_Ok;


	}

	err=yang_create_rtcsocket(session->context.sock,session->context.avinfo->sys.familyType,(YangSocketProtocol)session->context.avinfo->rtc.rtcSocketProtocol,session->context.streamConfig->localPort);
	session->context.sock->session.isControlled=session->isControlled;
	if(err!=Yang_Ok) return yang_error_wrap(err,"setLocalDescription error!");
#if Yang_Enable_Tcp_Srs
	if(session->context.avinfo->sys.mediaServer==Yang_Server_Srs)
		yang_create_rtcsocket_srs(session->context.sock,(YangSocketProtocol)session->context.avinfo->rtc.rtcSocketProtocol);
#endif
	yang_rtcconn_startudp(session);
	session->context.sock->start(&session->context.sock->session);
	return err;
}

static int32_t yang_rtcconn_setRemoteDescription(YangRtcSession* session,char* sdpstr){
	if(session==NULL || sdpstr==NULL) return ERROR_RTC_PEERCONNECTION;
		int32_t err=Yang_Ok;

		if(session->context.avinfo->sys.mediaServer==Yang_Server_P2p && !session->context.avinfo->rtc.iceUsingLocalIp
					&& session->ice.session.iceState==YangIceFail) {
			yang_error("p2p ice error!");
			return ERROR_RTC_PEERCONNECTION;
		}
		session->isControlled=session->context.streamConfig->isControlled;

		YangSdp sdp;
		yang_memset(&sdp,0,sizeof(YangSdp));
		yang_create_rtcsdp(&sdp);
	#if Yang_Enable_RTC_Audio
		if(session->remote_audio==NULL) session->remote_audio=(YangAudioParam*)yang_calloc(sizeof(YangAudioParam),1);
	#endif
	#if Yang_Enable_RTC_Video
		if(session->remote_video==NULL) session->remote_video=(YangVideoParam*)yang_calloc(sizeof(YangVideoParam),1);
	#endif
		if((err=yang_rtcsdp_parse(&sdp,sdpstr))!=Yang_Ok){
			yang_error("sdp parse error!");
		}
		if(err==Yang_Ok&&(err=yang_sdp_parseRemoteSdp(session,&sdp))!=Yang_Ok){
			yang_error("parseRemoteSdp error!");
		}
		yang_destroy_rtcsdp(&sdp);

		session->ice.session.stun.createRequestStunPacket(session,session->remoteIcePwd);
		if(session->context.avinfo->sys.mediaServer==Yang_Server_P2p&&!session->context.avinfo->rtc.iceUsingLocalIp){
			if((err=session->ice.iceHandle(&session->ice.session,session,yang_rtcconn_turn_receive,
						session->context.streamConfig->remoteIp,session->context.streamConfig->remotePort))!=Yang_Ok){
					return yang_error_wrap(err,"setLocalDescription error!");
				}


			if(session->ice.session.candidateType==YangIceTurn&&session->ice.session.iceState==YangIceSuccess){
				err=yang_create_rtcsocket(session->context.sock,session->context.avinfo->sys.familyType,Yang_Socket_Protocol_Udp,session->context.streamConfig->localPort);
				if(err!=Yang_Ok) return yang_error_wrap(err,"setLocalDescription error!");
				session->context.sock->write=yang_rtcconn_turn_sendData;
				yang_rtcconn_startudp(session);
				g_yang_startStunTimer(session->context.sock->session.user);
				return Yang_Ok;
			}
		}

        if(!session->isControlled)
            session->context.sock->updateRemoteAddress(&session->context.sock->session,session->context.streamConfig->remoteIp,session->isControlled?0:session->context.streamConfig->remotePort);


		return err;
}


static yangbool yang_rtcconn_isConnected(YangRtcSession* session){
	if (session == NULL)		return yangfalse;
	return session->context.state == Yang_Conn_State_Connected;
}

int32_t yang_create_rtcConnection(YangRtcConnection* conn,YangStreamConfig* streamconfig,YangAVInfo* avinfo){
	if (conn == NULL ||streamconfig == NULL || avinfo==NULL)		return ERROR_RTC_CONNECT;
    YangRtcSession* session=(YangRtcSession*)yang_calloc(sizeof(YangRtcSession),1);
	conn->session=session;
	yang_create_rtcContext(&session->context);
	yang_create_ice(&session->ice,streamconfig,avinfo);
	yang_memset(&session->rtcp_compound,0,sizeof(YangRtcpCompound));
	yang_create_rtcpCompound(&session->rtcp_compound);

	session->context.streamConfig = streamconfig;
	session->context.sock->session.user = conn;
	session->context.sock->session.receive = g_session_receive;
	session->context.sock->session.startStunTimer = g_yang_startStunTimer;

	session->context.avinfo=avinfo;

	if(streamconfig->remotePort==0)
		streamconfig->remotePort=8000;

	session->isControlled=yangfalse;
	session->enableDatachannel=yangfalse;
	session->h264PayloadType=YangH264PayloadType;
	session->h265PayloadType=YangH265PayloadType;
	session->audioPayloadType = YangAudioPayloadType;


	conn->close=yang_rtcconn_close;
	conn->on_video=yang_rtcconn_onVideo;
	conn->on_audio=yang_rtcconn_onAudio;
	conn->on_message=yang_rtcconn_onMessage;
	conn->notify=yang_rtcconn_notify;
	conn->isAlive=yang_rtcconn_isAlive;

	conn->receive=yang_rtcconn_receive;
	conn->updateCandidateAddress=yang_rtcconn_on_ice;
	conn->onConnectionStateChange=yang_onConnectionStateChange;
	conn->setLocalDescription=yang_rtcconn_setLocalDescription;
	conn->setRemoteDescription=yang_rtcconn_setRemoteDescription;
	conn->createOffer=yang_rtcconn_createOffer;
	conn->createAnswer=yang_rtcconn_createAnswer;
	conn->createHttpAnswer=yang_rtcconn_createHttpAnswer;
	conn->createDataChannel=yang_rtcconn_createDataChannel;
	conn->isConnected=yang_rtcconn_isConnected;

	conn->addAudioTrack=yang_rtcconn_addAudioTrack;
	conn->addVideoTrack=yang_rtcconn_addVideoTrack;
	conn->addTransceiver=yang_rtcconn_addTransceiver;
	return Yang_Ok;

}

void yang_destroy_rtcConnection(YangRtcConnection *conn) {
	if (conn == NULL)		return;
	YangRtcSession* session=(YangRtcSession*)conn->session;
	session->activeState = yangfalse;

	//yang_stop(session->20ms);
	yang_timer_stop(session->tm_1s);
	yang_timer_stop(session->tm_100ms);
	yang_destroy_timer(session->tm_1s);
	yang_destroy_timer(session->tm_100ms);
	// yang_free(session->20ms);
	yang_free(session->tm_1s);
	yang_free(session->tm_100ms);

#if Yang_Enable_RTC_Audio
	yang_destroy_pushAudio(session->pushAudio);
	yang_free(session->remote_audio);
#endif

#if Yang_Enable_RTC_Video
	yang_destroy_pushH264(session->pushH264);
	yang_free(session->pushH264);
	#if	Yang_Enable_H265_Encoding
	yang_destroy_pushH265(session->pushH265);
	yang_free(session->pushH265);
	#endif
	yang_free(session->remote_video);
#endif

	yang_destroy_rtcpush(session->push);
	yang_free(session->push);

	yang_destroy_rtcplay(session->play);
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

#if Yang_Enable_RTC_Audio
	if(session->pushAudioRtpBuffer){
		yang_destroy_rtpBuffer(session->pushAudioRtpBuffer);
		yang_free(session->pushAudioRtpBuffer);
	}
#endif

#if Yang_Enable_RTC_Video
	if(session->pushVideoRtpBuffer){
		yang_destroy_rtpBuffer(session->pushVideoRtpBuffer);
		yang_free(session->pushVideoRtpBuffer);
	}
#endif
	yang_free(conn->session);
}

