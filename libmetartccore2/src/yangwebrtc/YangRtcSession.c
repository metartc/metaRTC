#include <yangwebrtc/YangRtcSession.h>
#include <yangwebrtc/YangH264RtpEncode.h>
#include <yangwebrtc/YangH265RtpEncode.h>
#include <yangwebrtc/YangRtcDtls.h>
#include <yangwebrtc/YangUdpHandle.h>
#include <yangwebrtc/YangRtcConnection.h>
#include <yangwebrtc/YangRtcPlayStream.h>
#include <yangwebrtc/YangRtcPublishStream.h>
#include <yangwebrtc/YangRtcStun.h>
#include <yangrtp/YangRtcpCompound.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/yangavctype.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSRtp.h>


int32_t yang_rtcsession_dispatch_rtcp(YangRtcSession *session,YangRtcpCommon *rtcp);
/**
 bool yang_is_rtp_or_rtcp(const uint8_t *data, size_t len) {
 return (len >= 12 && (data[0] & 0xC0) == 0x80);
 }
 // For STUN packet, 0x00 is binding request, 0x01 is binding success response.


 // For RTCP, PT is [128, 223] (or without marker [0, 95]).
 // Literally, RTCP starts from 64 not 0, so PT is [192, 223] (or without marker [64, 95]).
 // @note For RTP, the PT is [96, 127], or [224, 255] with marker.
 bool yang_is_rtcp(const uint8_t *data, size_t len) {
 return (len >= 12) && (data[0] & 0x80) && (data[1] >= 192 && data[1] <= 223);
 }
 **/
bool yang_is_stun(const uint8_t *data, size_t size) {
	return size > 0 && (data[0] == 0 || data[0] == 1) ? 1 : 0;
}
// change_cipher_spec(20), alert(21), handshake(22), application_data(23)
// @see https://tools.ietf.org/html/rfc2246#section-6.2.1
bool yang_is_dtls(const uint8_t *data, size_t len) {
	return (len >= 13 && (data[0] > 19 && data[0] < 64)) ? 1 : 0;
}

void g_session_receive(char *data, int32_t nb_data, void *user) {
	if (user == NULL)
		return;
	YangRtcSession *session = (YangRtcSession*) user;

	yang_rtcsession_receive(session, data, nb_data);
}

void g_startStunTimer(void *user) {
	if (user == NULL)
		return;
	YangRtcSession *session = (YangRtcSession*) user;

	yang_rtcsession_startStunTimer(session);

}

void g_yang_doTask(int32_t taskId, void *user) {
	if (user == NULL)
		return;

	YangRtcSession *session = (YangRtcSession*) user;
	if (session->isSendStun && taskId == 1) {
		if (session->context.stun.data&& session->context.stun.nb > 0) {
			yang_rtc_sendData(session->context.udp,
					session->context.stun.data, session->context.stun.nb);
		}

	}
	if (!session->startRecv)
		return;
	if (session->playStream) {
		//int32_t err=0;
		// if (taskId == 20) {
		//	session->play->check_send_nacks();
		//}
		if (taskId == 1) {

			if (yang_rtcplay_send_rtcp_rr(&session->context,
					session->playStream))
				yang_error("RTCP Error:RR err ");
			if (yang_rtcplay_send_rtcp_xr_rrtr(&session->context,
					session->playStream))
				yang_error("RTCP Error:XR err ");
		}
		if (taskId == 100) {
			if (yang_rtcplay_send_periodic_twcc(&session->context,
					session->playStream))
				yang_error("RTCP Error:send twcc err ");
			//if(session->play->request_keyframe(this->session->videoSsrc)) yang_error("RTCP Error:request keyframe  err ");
		}
	}
}
void yang_init_rtcsession_context(YangRtcSession* session,YangAVContext* avcontext,void* user){
	if (session == NULL)
		return;
	memset(session,0,sizeof(YangRtcSession));
	memset(&session->context,0,sizeof(YangRtcContext));
	yang_init_rtcContext(&session->context);
	session->context.user = user;
	session->context.udp->user = session;
	session->context.udp->receive = g_session_receive;
	session->context.udp->startStunTimer = g_startStunTimer;

	session->context.avcontext->avinfo=avcontext->avinfo;

	session->context.avcontext->sendRequest=avcontext->sendRequest;
	session->context.avcontext->setPlayMediaConfig=avcontext->setPlayMediaConfig;
	session->context.avcontext->recvcb.receiveAudio=avcontext->recvcb.receiveAudio;
	session->context.avcontext->recvcb.receiveVideo=avcontext->recvcb.receiveVideo;

}
void yang_init_rtcsession(YangRtcSession *session, YangStreamOptType role) {
	if (session == NULL)
		return;

	session->codec =(YangVideoCodec) session->context.avcontext->avinfo->video.videoEncoderType;
	session->isSendDtls = 0;
	session->twcc_id_ = 0;
	session->h264Encoder = NULL;

	if (session->codec == Yang_VED_264) {
		session->h264Encoder = (YangH264RtpEncode*) calloc(1,sizeof(YangH264RtpEncode));
	}
#if	Yang_H265_Encoding
	session->h265Encoder = NULL;
	if (session->codec == Yang_VED_265) {
		session->h265Encoder = (YangH265RtpEncode*) calloc(1,sizeof(YangH265RtpEncode));
	}
#endif
	//  session->20ms


	session->tm_1s = (YangCTimer*) calloc(1, sizeof(YangCTimer));
	yang_init_timer(session->tm_1s, session, 1, 1000);
	session->tm_1s->doTask = g_yang_doTask;

	session->tm_100ms = (YangCTimer*) calloc(1, sizeof(YangCTimer));
	yang_init_timer(session->tm_100ms, session, 100, 100);
	session->tm_100ms->doTask = g_yang_doTask;

	session->startRecv = 0;
	session->isSendStun = 0;

	session->rtpBuffer = (YangRtpBuffer*) calloc(1,sizeof(YangRtpBuffer));
	session->activeState = true;

	yang_init_rtcdtls(session->context.dtls);

	yang_init_rtpBuffer(session->rtpBuffer, 1500, 1500);
	if (role == Yang_Stream_Publish) {
		if (session->h264Encoder) {
			yang_init_h264RtpEncode(session->h264Encoder, session->rtpBuffer);
		}
#if	Yang_H265_Encoding
		if (session->h265Encoder) {
			yang_init_h265RtpEncode(session->h265Encoder, session->rtpBuffer);
		}
#endif
	}
	if (role == Yang_Stream_Play) {
		if (session->playStream == NULL) {
			session->playStream = (YangRtcPlayStream*) calloc(1,sizeof(YangRtcPlayStream));
			yang_init_rtcplay(&session->context, session->playStream,session->rtpBuffer);

		}
	} else {
		if (session->pubStream == NULL) {
			session->pubStream = (YangRtcPublishStream*) calloc(1,sizeof(YangRtcPublishStream));
			yang_init_rtcpublish(session->pubStream,session->context.audioSsrc, session->context.videoSsrc);

		}
	}
}
void yang_destroy_rtcsession(YangRtcSession *session) {
	if (session == NULL)		return;
	session->activeState = false;
	//yang_stop(session->20ms);
	yang_timer_stop(session->tm_1s);
	yang_timer_stop(session->tm_100ms);
	yang_destroy_timer(session->tm_1s);
	yang_destroy_timer(session->tm_100ms);
	// yang_free(session->20ms);
	yang_free(session->tm_1s);
	yang_free(session->tm_100ms);

	yang_destroy_h264RtpEncode(session->h264Encoder);
	yang_free(session->h264Encoder);
#if	Yang_H265_Encoding
	yang_destroy_h265RtpEncode(session->h265Encoder);
	yang_free(session->h265Encoder);
#endif

	yang_free(session->remote_audio);
	yang_free(session->remote_video);

	yang_destroy_rtcpublish(session->pubStream);
	yang_free(session->pubStream);

	yang_destroy_rtcplay(session->playStream);
	yang_free(session->playStream);

	yang_destroy_rtcContext(&session->context);

	yang_destroy_rtpBuffer(session->rtpBuffer);
	yang_free(session->rtpBuffer);
}

void yang_rtcsession_setSsrc(YangRtcSession *session, uint32_t audioSsrc,
		uint32_t videoSsrc) {
	if (session->h264Encoder) {
		session->h264Encoder->audioSsrc = audioSsrc;
		session->h264Encoder->videoSsrc = videoSsrc;
	}
#if	Yang_H265_Encoding
	if (session->h265Encoder) {
		session->h265Encoder->audioSsrc = audioSsrc;
		session->h265Encoder->videoSsrc = videoSsrc;
	}
#endif

}

void yang_rtcsession_startStunTimer(YangRtcSession *session) {

	if (session->tm_1s)
		yang_timer_start(session->tm_1s);

	session->isSendStun = 1;
}
void yang_rtcsession_startTimers(YangRtcSession *session) {
	// if (session->20ms&&!session->20ms->session->isStart)		session->20ms->start();
	if (session->context.avcontext && session->context.avcontext->avinfo->rtc.sendTwcc
			&& session->tm_100ms && !session->tm_100ms->isStart)
		yang_timer_start(session->tm_100ms);		//->start();
}

int32_t yang_rtcsession_on_rtcp(YangRtcSession *session, char *data,
		int32_t nb_data) {
	int32_t err = Yang_Ok;

	int32_t nb_unprotected_buf = nb_data;
	if ((err = yang_dec_rtcp(&session->context.srtp, data, &nb_unprotected_buf))
			!= Yang_Ok) {
		return yang_error_wrap(err, "rtcp unprotect");
	}

	char *unprotected_buf = data;
	YangBuffer buffer;
	yang_init_buffer(&buffer, unprotected_buf, nb_unprotected_buf);


	YangRtcpCompound rtcp_compound;
	memset(&rtcp_compound,0,sizeof(YangRtcpCompound));
	yang_init_rtcpCompound(&rtcp_compound);
	if (Yang_Ok != (err = yang_decode_rtcpCompound(&rtcp_compound, &buffer))) {
		return yang_error_wrap(err, "decode rtcp plaintext=%u",
				nb_unprotected_buf);
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
void yang_rtcsession_setStun(YangRtcSession *session, char *data, int32_t nb) {
	if (session == NULL)
		return;
	if (session->context.stun.data == NULL)
		session->context.stun.data = (char*) calloc(1, nb);
	memcpy(session->context.stun.data, data, nb);
	session->context.stun.nb = nb;
}
void yang_rtcsession_startudp(YangRtcSession *session, int32_t localport) {
	yang_init_rtcudp(session->context.udp,
			session->context.streamConf->serverIp, localport);
	yang_init_rtcsession(session, session->context.streamConf->streamOptType);

	yang_rtcsession_setSsrc(session, session->context.audioSsrc,
			session->context.videoSsrc);
	yang_start_rtcudp(session->context.udp);
}
void yang_rtcsession_receive(YangRtcSession *session, char *data, int32_t size) {

	if (session==NULL||!session->activeState)	return;

	bool is_rtp_or_rtcp = (size >= 12 && (data[0] & 0xC0) == 0x80);
	bool is_rtcp = (size >= 12) && (data[0] & 0x80)
			&& (data[1] >= 192 && data[1] <= 223);

	if (is_rtp_or_rtcp && !is_rtcp) {
		session->startRecv = 1;
		if (session->playStream)
			yang_rtcplay_on_rtp(&session->context, session->playStream,
					data, size);
		return;

	}
	if (is_rtp_or_rtcp && is_rtcp) {
		yang_rtcsession_on_rtcp(session, data, size);
		return;
	}

	if (!is_rtp_or_rtcp && yang_is_stun((uint8_t*) data, size)) {

		int32_t err = 0;
		if ((err = yang_decode_stun2(data, size)) != Yang_Ok) {
			yang_error("decode stun packet failed");
			return;
		}

		if (!session->isSendDtls) {
			if (yang_startHandShake(session->context.dtls)) {
				yang_error("dtls start handshake failed!");
			}

			session->isSendDtls = 1;
		}
		return;
	}
	if (size >= 13 && yang_is_dtls((uint8_t*) data, size)) {

		if (session->context.dtls) {
			if (yang_decodeHandshake(session->context.dtls, data,size) == Yang_Ok) {


				if (session->context.dtls->state == YangDtlsStateClientDone) {
					yang_rtcsession_startTimers(session);
					session->context.state = 1;

					if (session->context.avcontext)

						session->context.avcontext->sendRequest(session->context.streamConf->uid, 0,
								Yang_Req_Connected,session->context.user);
				}
			}
		}

	}
}

int32_t yang_rtcsession_send_video_meta(YangRtcSession *session, YangFrame *p) {

	if (session->h264Encoder)
		return yang_pub_h264_package_stap_a(session,session->h264Encoder, p);
#if	Yang_H265_Encoding
	if (session->h265Encoder)
		return yang_pub_h265_package_stap_a(session,session->h265Encoder, p);
#endif
	return Yang_Ok;
}

int32_t yang_rtcsession_publishVideo(YangRtcSession *session, YangFrame *p) {

	if (p->frametype == YANG_Frametype_Spspps)
		return yang_rtcsession_send_video_meta(session, p);
	if (session->h264Encoder)
		return yang_pub_h264_video(session, session->h264Encoder, p);
#if	Yang_H265_Encoding
	if (session->h265Encoder)
		return yang_pub_h265_video(session, session->h265Encoder, p);
#endif
	return Yang_Ok;

}

int32_t yang_rtcsession_publishAudio(YangRtcSession *session, YangFrame *p) {

	if (session->h264Encoder)
		return yang_pub_h264_audio(session, session->h264Encoder, p);
#if	Yang_H265_Encoding
	if (session->h265Encoder)
		return yang_pub_h265_audio(session, session->h265Encoder, p);
#endif

	return Yang_Ok;

}

void yang_rtcsession_disconnect(YangRtcSession *session) {
	if (session == NULL)
		return;
	if (session->context.dtls)
		yang_sendDtlsAlert(session->context.dtls);
}


int32_t yang_rtcsession_send_rtcp(YangRtcSession *session, char *data,
		int32_t nb_data) {
	int32_t err = Yang_Ok;
	int32_t nb_buf = nb_data;

	if ((err = yang_enc_rtcp(&session->context.srtp, data, &nb_buf)) != Yang_Ok) {
		return yang_error_wrap(err, "protect rtcp");
	}

	if ((err = yang_rtc_sendData(session->context.udp, data, nb_buf))
			!= Yang_Ok) {
		return yang_error_wrap(err, "send");
	}

	return err;
}

int32_t yang_rtcsession_on_rtcp_feedback_twcc(YangRtcSession *session,
		char *data, int32_t nb_data) {
	return Yang_Ok;
}

int32_t yang_rtcsession_on_rtcp_feedback_remb(YangRtcSession *session,
		YangRtcpCommon *rtcp) {
	//ignore REMB
	return Yang_Ok;
}
void yang_rtcsession_do_request_keyframe(YangRtcSession *session, uint32_t ssrc) {
	if (session->context.avcontext)
		session->context.avcontext->sendRequest(session->context.streamConf->uid, ssrc, Yang_Req_Sendkeyframe,session->context.user);

}

int32_t yang_rtcsession_dispatch_rtcp(YangRtcSession *session,
		YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;
	uint16_t rtcpType = rtcp->header.type;
	// For TWCC packet.
	if (YangRtcpType_rtpfb == rtcpType && 15 == rtcp->header.rc) {
		return yang_rtcsession_on_rtcp_feedback_twcc(session, rtcp->data,
				rtcp->nb_data);
	}

	// For REMB packet.
	if (YangRtcpType_psfb == rtcpType) {

		if (15 == rtcp->header.rc) {
			return yang_rtcsession_on_rtcp_feedback_remb(session, rtcp);
		}
	}

	// Ignore special packet.
	if (YangRtcpType_rr == rtcpType) {
		if (rtcp->rb->ssrc == 0) { //for native client
			return err;
		}
	}

	if (session->pubStream
			&& Yang_Ok
					!= (err = yang_rtcpublish_on_rtcp(&session->context,
							session->pubStream, rtcp))) {
		return yang_error_wrap(err, "handle rtcp");
	}
	if (session->playStream
			&& Yang_Ok
					!= (err = yang_rtcplay_on_rtcp(&session->context,
							session->playStream, rtcp))) {
		return yang_error_wrap(err, "handle rtcp");
	}

	return err;
}
int32_t yang_send_avpacket(YangRtcSession *session, YangRtpPacket *pkt,
		YangBuffer *pbuf) {
	int32_t err = Yang_Ok;
	int32_t nn_encrypt = yang_buffer_pos(pbuf);

	if (session->pubStream)
		yang_rtcpublish_cache_nack(session->pubStream, pkt, pbuf->data,
				nn_encrypt);
	if ((err = yang_enc_rtp(&session->context.srtp, pbuf->data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}

	return yang_rtc_sendData(session->context.udp, pbuf->data, nn_encrypt);
}

int32_t yang_recvvideo_notify(YangRtcSession *session, YangRtcMessageType mess) {
	if(session==NULL||session->playStream==NULL||session->playStream->video_track==NULL) return 1;
	YangVideoRecvTrack *videorecv=session->playStream->video_track;
	if (mess == YangRTC_Decoder_Input) {
		if (videorecv->h264Track)
			videorecv->h264Track->hasRequestKeyframe = false;
		if (videorecv->h265Track)
			videorecv->h265Track->hasRequestKeyframe = false;
		return yang_send_rtcp_fb_pli(&session->context, videorecv->recv.ssrc_);
	}

	return Yang_Ok;
}
