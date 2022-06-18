//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangRtcSession.h>
#include <yangrtc/YangPushH264.h>
#include <yangrtc/YangPushH265.h>
#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangRtcDtls.h>
#include <yangrtc/YangRtcStun.h>
#include <yangrtc/YangUdpHandle.h>
#include <yangrtc/YangBandwidth.h>
#include <yangrtc/YangPlayStream.h>
#include <yangrtc/YangPushStream.h>
#include <yangrtp/YangRtcpCompound.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/yangavctype.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSRtp.h>


int32_t yang_rtcsession_dispatch_rtcp(YangRtcSession *session,YangRtcpCommon *rtcp);
void yang_rtcsession_startStunTimer(YangRtcSession *session);
void yang_rtcsession_receive(YangRtcSession *session, char *data, int32_t size);
void g_session_receive(char *data, int32_t nb_data, void *user) {
	if (user == NULL)
		return;
	YangRtcSession *session = (YangRtcSession*) user;

	yang_rtcsession_receive(session, data, nb_data);
}

void g_startStunTimer(void *user) {
	if (user == NULL)		return;
	YangRtcSession *session = (YangRtcSession*) user;
	yang_rtcsession_startStunTimer(session);
}

void g_yang_doTask(int32_t taskId, void *user) {
	if (user == NULL)	return;

	YangRtcSession *session = (YangRtcSession*) user;
	if (session->isSendStun && taskId == 1) {
		if (session->context.stun.data&& session->context.stun.nb > 0) {
			yang_rtc_sendData(session->context.udp,session->context.stun.data, session->context.stun.nb);
		}

	}
	if(!session->context.state) return;


	if (session->startRecv&&session->play) {

		if (taskId == 1) {

			if (session->play->send_rtcp_rr(&session->context,session->play->playStream))	yang_error("RTCP Error:RR err ");
			if (session->play->send_rtcp_xr_rrtr(&session->context,session->play->playStream))	yang_error("RTCP Error:XR err ");

		}
#if Yang_Using_TWCC
		if (taskId == 100) {
			if (session->play->send_periodic_twcc(&session->context,session->play->playStream))
				yang_error("RTCP Error:send twcc err ");

		}
#endif
	}

	if(session->push){
		if (taskId == 1) {
			if (session->push->check_rrt(&session->context,session->push->pubStream))		yang_error("check rtt Error ");
			if (session->push->check_twcc(&session->context,session->push->pubStream))		yang_error("check twcc Error ");
		}
	}

}



void yang_rtcsession_init(YangRtcSession *session, YangStreamOptType role) {
	if (session == NULL)	return;

	session->codec =(YangVideoCodec) session->context.avinfo->video.videoEncoderType;
	session->isSendDtls = 0;
	session->pushH264 = NULL;
	session->sessionTimeout=session->context.avinfo->rtc.sessionTimeout;
    if (role == Yang_Stream_Publish || role==Yang_Stream_Both)  {
        if (session->codec == Yang_VED_264) {
            session->pushH264 = (YangPushH264*) calloc(1,sizeof(YangPushH264));
        }
#if	Yang_H265_Encoding
        session->pushH265 = NULL;
        if (session->codec == Yang_VED_265) {
            session->pushH265 = (YangPushH265*) calloc(1,sizeof(YangPushH265));
        }
#endif
    }
	//  session->20ms
	session->tm_1s = (YangCTimer*) calloc(1, sizeof(YangCTimer));
	yang_create_timer(session->tm_1s, session, 1, 1000);
	session->tm_1s->doTask = g_yang_doTask;

	session->tm_100ms = (YangCTimer*) calloc(1, sizeof(YangCTimer));
	yang_create_timer(session->tm_100ms, session, 100, 100);
	session->tm_100ms->doTask = g_yang_doTask;

	session->startRecv = 0;
	session->isSendStun = 0;

#if Yang_HaveDtls
	yang_create_rtcdtls(session->context.dtls,session->isServer);
	session->context.dtls->sslCallback=&session->context.streamConfig->sslCallback;
	session->context.dtls->uid=session->context.streamConfig->uid;
#endif


	if (role == Yang_Stream_Play || role==Yang_Stream_Both) {
		if(session->playRtpBuffer == NULL) {
			session->playRtpBuffer = (YangRtpBuffer*) calloc(1,sizeof(YangRtpBuffer));
			yang_create_rtpBuffer(session->playRtpBuffer, 1500, kRtpPacketSize);
		}
		if (session->play == NULL) {
			session->play = (YangRtcPlay*) calloc(1,sizeof(YangRtcPlay));
			yang_create_rtcplay(&session->context, session->play,session->playRtpBuffer);

		}
	}

	if (role == Yang_Stream_Publish || role==Yang_Stream_Both)  {

		if(session->pushAudioRtpBuffer == NULL) {
			session->pushAudioRtpBuffer = (YangRtpBuffer*) calloc(1,sizeof(YangRtpBuffer));
			yang_create_rtpBuffer(session->pushAudioRtpBuffer, 100, kRtpPacketSize);
		}

		if(session->pushVideoRtpBuffer == NULL) {
			session->pushVideoRtpBuffer = (YangRtpBuffer*) calloc(1,sizeof(YangRtpBuffer));
			yang_create_rtpBuffer(session->pushVideoRtpBuffer, 1400, kRtpPacketSize);
		}

		if (session->pushH264) {
			yang_create_pushH264(session->pushH264, session->pushAudioRtpBuffer,session->pushVideoRtpBuffer);
		}
#if	Yang_H265_Encoding
		if (session->pushH265) {
			yang_create_pushH265(session->pushH265,  session->pushAudioRtpBuffer,session->pushVideoRtpBuffer);
		}
#endif
		if (session->push == NULL) {
			session->push = (YangRtcPush*) calloc(1,sizeof(YangRtcPush));
			yang_create_rtcpush(session->push,session->context.audioSsrc, session->context.videoSsrc);
		}
	}
#if Yang_HaveDtls
#if Yang_HaveDatachannel
	if(session->usingDatachannel){
		if(session->datachannel==NULL){
			session->datachannel=(YangDatachannel*)calloc(sizeof(YangDatachannel),1);
			yang_create_datachannel(session->datachannel,&session->context);
		}
	}
#endif
#endif
	session->activeState = true;
}



void yang_rtcsession_setSsrc(YangRtcSession *session, uint32_t audioSsrc,
		uint32_t videoSsrc) {
	if (session->pushH264) {
		session->pushH264->push->audioSsrc = audioSsrc;
		session->pushH264->push->videoSsrc = videoSsrc;
	}
#if	Yang_H265_Encoding
	if (session->pushH265) {
		session->pushH265->push->audioSsrc = audioSsrc;
		session->pushH265->push->videoSsrc = videoSsrc;
	}
#endif

}

void yang_rtcsession_startStunTimer(YangRtcSession *session) {

	if (session->tm_1s)
		yang_timer_start(session->tm_1s);
	session->isSendStun = 1;
}

void yang_rtcsession_startTimers(YangRtcSession *session) {
	YangStreamOptType opt=session->context.streamConfig->streamOptType;
	// if (session->20ms&&!session->20ms->session->isStart)		session->20ms->start();
	if (session->context.twccId>0&&(opt==Yang_Stream_Play||opt==Yang_Stream_Both)&&session->tm_100ms&&!session->tm_100ms->isStart)
		yang_timer_start(session->tm_100ms);
}

int32_t yang_rtcsession_on_rtcp(YangRtcSession *session, char *data,int32_t nb_data) {
	int32_t err = Yang_Ok;
	int32_t nb_unprotected_buf = nb_data;
#if Yang_HaveDtls
	if ((err = yang_dec_rtcp(&session->context.srtp, data, &nb_unprotected_buf))!= Yang_Ok) {
		if (err == srtp_err_status_replay_fail)	return Yang_Ok;
		return yang_error_wrap(err, "rtcp unprotect");
	}
#endif
	char *unprotected_buf = data;
	YangBuffer buffer;
	yang_init_buffer(&buffer, unprotected_buf, nb_unprotected_buf);


	YangRtcpCompound rtcp_compound;
	memset(&rtcp_compound,0,sizeof(YangRtcpCompound));
	yang_create_rtcpCompound(&rtcp_compound);
	if (Yang_Ok != (err = yang_decode_rtcpCompound(&rtcp_compound, &buffer))) {
		return yang_error_wrap(err, "decode rtcp plaintext=%u",	nb_unprotected_buf);
	}

	YangRtcpCommon *rtcp = NULL;
	for (int i = 0; i < rtcp_compound.rtcpVector.vsize; i++) {
		rtcp = &rtcp_compound.rtcpVector.payload[i];
		err = yang_rtcsession_dispatch_rtcp(session, rtcp);
		if (Yang_Ok != err) {
			return yang_error_wrap(err,
					"cipher=%u, plaintext=%u,  rtcp=(%u,%u,%u,%u)", nb_data,
					nb_unprotected_buf, rtcp->nb_data, rtcp->header.rc,
					rtcp->header.type, rtcp->ssrc);
		}
	}
	yang_destroy_rtcpCompound(&rtcp_compound);
	return err;

}
int32_t yang_rtcsession_isAlive(YangRtcSession* session){
	if(session==NULL) return 0;
	return session->lastStunTime + session->sessionTimeout > yang_get_system_time()?1:0;
}


void yang_rtcsession_startudp(YangRtcSession *session) {
	yang_rtcsession_init(session, session->context.streamConfig->streamOptType);
	yang_rtcsession_setSsrc(session, session->context.audioSsrc,session->context.videoSsrc);
}

void yang_rtcsession_receive(YangRtcSession *session, char *data, int32_t size) {
	if (session==NULL||!session->activeState)	return;
	uint8_t bt=(uint8_t)data[0];
	session->lastStunTime=yang_get_system_time();
	//is rtp rtcp
	if(bt > 127 && bt < 192 && size>12){
		bt=(uint8_t)data[1];
		if (bt>= 192 && bt <= 223) {//rtcp
			yang_rtcsession_on_rtcp(session, data, size);
			return;
		}
		//rtp
		session->startRecv = 1;
		if (session->play)	session->play->on_rtp(&session->context, session->play->playStream,data, size);
		return;
	}
	//is stun
	if(size>0&&(bt==0x00||bt==0x01)){
		int32_t err = 0;
		if(session->isServer){
			YangStunPacket request;
			memset(&request,0,sizeof(YangStunPacket));
			if ((err = yang_decode_stun(&request,data, size)) != Yang_Ok) {
				yang_error("decode stun packet failed");
				return;
			}
			if ((err =yang_stun_createResponseStunPacket(&request,session)) != Yang_Ok) {
				yang_error("create response stun packet failed");
				return;
			}
			if(!session->context.state)	session->context.state=1;
		//	session->lastStunTime=yang_get_system_time();

		}else{
			if ((err = yang_decode_stun2(data, size)) != Yang_Ok) {
				yang_error("decode stun packet failed");
				return;
			}

#if Yang_HaveDtls
			if (!session->isSendDtls) {
				if (yang_startHandShake(session->context.dtls)) yang_error("dtls start handshake failed!");
				session->isSendDtls = 1;
			}
#else
			if(!session->context.state)	goto client_sucess;

#endif
		}

		return;
	}

	//is dtls
		if (bt > 19 && bt < 64) {
#if Yang_HaveDtls
			if(session->context.dtls==NULL) return;
#if Yang_HaveDatachannel
			if (yang_process_dtls_data(session->datachannel,session->context.dtls, data,size) == Yang_Ok && session->context.state ==0) {
#else
            if (yang_process_dtls_data(NULL,session->context.dtls, data,size) == Yang_Ok && session->context.state ==0) {
#endif

				if(session->isServer){
					if(session->context.dtls->handshake_done) session->context.state = 1;
					return;
				}else if (session->context.dtls->state == YangDtlsStateClientDone) {
					goto client_sucess;
				}
			}

#else
		if(session->isServer){
			void* context=session->context.streamConfig->sslCallback.context;
			session->context.streamConfig->sslCallback.sslAlert(context,session->context.streamConfig->uid,"warning","CN");
		}
#endif
		}

	client_sucess:
	yang_rtcsession_startTimers(session);
	session->context.state = 1;
	if (session->context.streamConfig&&session->context.streamConfig->rtcCallback.sendRequest)
		session->context.streamConfig->rtcCallback.sendRequest(session->context.streamConfig->rtcCallback.context,session->context.streamConfig->uid, 0,Yang_Req_Connected);
	return;
}


int32_t yang_rtcsession_send_video_meta(YangRtcSession *session, YangFrame *p) {
	if (session->pushH264)
		return session->pushH264->on_spspps(session,session->pushH264->push, p);
#if	Yang_H265_Encoding
	if (session->pushH265)
		return session->pushH265->on_spspps(session,session->pushH265->push, p);
#endif
	return Yang_Ok;
}

int32_t yang_rtcsession_publishVideo(YangRtcSession *session, YangFrame *p) {
#if Yang_HaveDtls
	if (session==NULL||session->context.dtls->state!=YangDtlsStateClientDone)	return Yang_Ok;
#else
	if (session==NULL||!session->context.state)	return Yang_Ok;
#endif
	if (p->frametype == YANG_Frametype_Spspps)
		return yang_rtcsession_send_video_meta(session, p);
	if (session->pushH264)
		return session->pushH264->on_video(session, session->pushH264->push, p);
#if	Yang_H265_Encoding
	if (session->pushH265)
		return session->pushH265->on_video(session, session->pushH265->push, p);
#endif
	return Yang_Ok;

}

int32_t yang_rtcsession_publishAudio(YangRtcSession *session, YangFrame *p) {
#if Yang_HaveDtls
	if (session==NULL||session->context.dtls->state!=YangDtlsStateClientDone)	return Yang_Ok;
#else
	if (session==NULL||!session->context.state)	return Yang_Ok;
#endif
	if (session->pushH264)
		return session->pushH264->on_audio(session, session->pushH264->push, p);
#if	Yang_H265_Encoding
	if (session->pushH265)
		return session->pushH265->on_audio(session, session->pushH265->push, p);
#endif

	return Yang_Ok;

}
int32_t yang_rtcsession_publishMsg(YangRtcSession *session, YangFrame *p) {
#if Yang_HaveDtls
#if Yang_HaveDatachannel
	if(session==NULL||session->context.dtls->state!=YangDtlsStateClientDone||session->context.dtls->isStop==1||session->context.state!=1)	return Yang_Ok;
	if(session->datachannel&&session->datachannel->send_message) session->datachannel->send_message(session->datachannel->context,p);
#endif
#endif
	return Yang_Ok;
}

void yang_rtcsession_disconnect(YangRtcSession *session) {
	if (session == NULL)	return;
#if Yang_HaveDtls
	if(yang_rtcsession_isAlive(session)){
		if (session->context.dtls&&!session->context.dtls->isStop)	yang_sendDtlsAlert(session->context.dtls);
	}else{

	}

#else
	if(session->isServer==0){
		char alerts[15];
		memset(alerts,0,15);
		alerts[0]=30;
		for(int i=0;i<5;i++){
			yang_usleep(1000*20);
			if (yang_rtc_sendData(session->context.udp, alerts, 15)	!= Yang_Ok) {
				yang_error("send error");
			}
		}
	}
#endif
}




int32_t yang_rtcsession_on_rtcp_feedback_twcc(YangRtcSession *session,YangRtcpCommon *rtcp) {
#if Yang_Using_TWCC
	yang_twcc_decode(&session->context.twcc,rtcp);
#endif
	return Yang_Ok;
}

int32_t yang_rtcsession_on_rtcp_feedback_remb(YangRtcSession *session,	YangRtcpCommon *rtcp) {

	return Yang_Ok;
}

void yang_rtcsession_do_request_keyframe(YangRtcSession *session, uint32_t ssrc) {
	if (session->context.streamConfig&&session->context.streamConfig->rtcCallback.sendRequest)
		session->context.streamConfig->rtcCallback.sendRequest(session->context.streamConfig->rtcCallback.context,session->context.streamConfig->uid, ssrc, Yang_Req_Sendkeyframe);

}

int32_t yang_rtcsession_dispatch_rtcp(YangRtcSession *session,YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;
	uint16_t rtcpType = rtcp->header.type;
	// For TWCC packet.
	if (YangRtcpType_rtpfb == rtcpType && 15 == rtcp->header.rc) {
		return yang_rtcsession_on_rtcp_feedback_twcc(session, rtcp);
	}

	// For REMB packet.
	if (YangRtcpType_psfb == rtcpType) {

		if (15 == rtcp->header.rc) {
			return yang_rtcsession_on_rtcp_feedback_remb(session, rtcp);
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



int32_t yang_recvvideo_notify(YangRtcSession *session, YangRtcMessageType mess) {
	if(session==NULL||session->play==NULL||session->play->playStream->videoTrack==NULL) return 1;
	YangPlayTrackVideo *videotrack=session->play->playStream->videoTrack;
	if (mess == YangRTC_Decoder_Error) {
		if(videotrack) videotrack->setRequestKeyframeState(&videotrack->session,false);
		return yang_send_rtcp_fb_pli(&session->context, videotrack->session.track.ssrc);
	}

	return Yang_Ok;
}

void yang_create_rtcConnection(YangRtcConnection* conn,YangAVInfo* avinfo){
	if (conn == NULL)		return;
	YangRtcSession* session=calloc(sizeof(YangRtcSession),1);
	conn->session=session;
	yang_create_rtcContext(&session->context);

	session->context.udp->user = session;
	session->context.udp->receive = g_session_receive;
	session->context.udp->startStunTimer = g_startStunTimer;

	session->context.avinfo=avinfo;

	session->sendTwccId=Yang_TWCC_ID;
	session->listenPort=8000;
	session->isServer=0;

	conn->disconnect=yang_rtcsession_disconnect;
	conn->init=yang_rtcsession_init;
	conn->publishVideo=yang_rtcsession_publishVideo;
	conn->publishAudio=yang_rtcsession_publishAudio;
	conn->publishMsg=yang_rtcsession_publishMsg;
	conn->notify=yang_recvvideo_notify;
	conn->isAlive=yang_rtcsession_isAlive;
	conn->startudp=yang_rtcsession_startudp;
	conn->receive=yang_rtcsession_receive;
}

void yang_destroy_rtcConnection(YangRtcConnection *conn) {
	if (conn == NULL)		return;
	YangRtcSession* session=(YangRtcSession*)conn->session;
	session->activeState = false;

	//yang_stop(session->20ms);
	yang_timer_stop(session->tm_1s);
	yang_timer_stop(session->tm_100ms);
	yang_destroy_timer(session->tm_1s);
	yang_destroy_timer(session->tm_100ms);
	// yang_free(session->20ms);
	yang_free(session->tm_1s);
	yang_free(session->tm_100ms);

	yang_destroy_pushH264(session->pushH264);
	yang_free(session->pushH264);
#if	Yang_H265_Encoding
	yang_destroy_pushH265(session->pushH265);
	yang_free(session->pushH265);
#endif

	yang_free(session->remote_audio);
	yang_free(session->remote_video);

	yang_destroy_rtcpush(session->push);
	yang_free(session->push);

	yang_destroy_rtcplay(session->play);
	yang_free(session->play);
#if Yang_HaveDatachannel
	yang_destroy_datachannel(session->datachannel);
	yang_free(session->datachannel);
#endif
	yang_destroy_rtcContext(&session->context);
	if(session->playRtpBuffer){
		yang_destroy_rtpBuffer(session->playRtpBuffer);
		yang_free(session->playRtpBuffer);
	}
	if(session->pushAudioRtpBuffer){
		yang_destroy_rtpBuffer(session->pushAudioRtpBuffer);
		yang_free(session->pushAudioRtpBuffer);
	}
	if(session->pushVideoRtpBuffer){
		yang_destroy_rtpBuffer(session->pushVideoRtpBuffer);
		yang_free(session->pushVideoRtpBuffer);
	}
	yang_free(conn->session);

}
