//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtcp.h>
#include <yangrtp/YangRtcpSR.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangSRtp.h>

#include <yangutil/buffer/YangBuffer.h>

#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangPlayStream.h>
#include <yangrtc/YangPlayTrackAudio.h>
#include <yangrtc/YangPlayTrackVideo.h>
#include <yangrtc/YangPlayTrackVideo.h>

#include <math.h>

int32_t yang_rtcplay_on_twcc(YangRtcContext *context, YangRtcPlayStream *play,uint16_t sn) {
	if (context == NULL || play == NULL)	return ERROR_RTC_PLAY;
	int32_t err = Yang_Ok;
#if Yang_Enable_TWCC
	if(!play->twcc->isSendtwcc&&play->twcc->newSnsIndex>0&&play->twcc->newSnsIndex<play->twcc->sns.vsize){
		if(play->twcc->newSnsIndex==play->twcc->sns.vsize-1){
			yang_clear_uint16Sort(&play->twcc->sns);
		}else{
			yang_clear_uint16Sort2(&play->twcc->sns,play->twcc->newSnsIndex);

		}
		play->twcc->newSnsIndex=0;
	}
	int32_t ind=YANG_GET_RECV_BUFFER_INDEX(sn);
	play->twcc->tss[ind]=yang_get_micro_time();
	yang_insert_uint16Sort(&play->twcc->sns,sn);
	play->twcc->newRecvSn=sn;
#endif

	return err;
}

#if Yang_Enable_RTC_Video
YangPlayTrackVideo* yang_rtcplay_get_video_track(YangRtcContext *context,
		YangRtcPlayStream *play, uint32_t ssrc) {
	if (context == NULL || play == NULL)
		return NULL;

	if (play->videoTrack->has_ssrc(context, &play->videoTrack->session.track, ssrc)) {
		return play->videoTrack;
	}


	return NULL;
}
#endif


#if Yang_Enable_RTC_Audio
YangPlayTrackAudio* yang_rtcplay_get_audio_track(YangRtcContext *context,
		YangRtcPlayStream *play, uint32_t ssrc) {
	if (context == NULL || play == NULL)
		return NULL;

	if (play->audioTrack->has_ssrc(context, &play->audioTrack->session, ssrc)) {
		return play->audioTrack;
	}


	return NULL;
}
#endif
void yang_rtcplay_update_rtt(YangRtcContext *context, YangRtcPlayStream *play,
		uint32_t ssrc, int32_t rtt) {
	if (context == NULL || play == NULL)
		return;
#if Yang_Enable_RTC_Video
	YangPlayTrackVideo *video_track = yang_rtcplay_get_video_track(context,
			play, ssrc);

	if (video_track) {
		return video_track->update_rtt(&video_track->session.track, rtt);
	}
#endif
#if Yang_Enable_RTC_Audio
	YangPlayTrackAudio *audio_track = yang_rtcplay_get_audio_track(context, play,
			ssrc);
	if (audio_track) {
		return audio_track->update_rtt(&audio_track->session, rtt);
	}
#endif
}

void yang_rtcplay_update_send_report_time(YangRtcContext *context,
		YangRtcPlayStream *play, uint32_t ssrc, YangNtp *ntp, uint32_t rtp_time) {
	if (context == NULL || play == NULL)
		return;
#if Yang_Enable_RTC_Video
	YangPlayTrackVideo *video_track = yang_rtcplay_get_video_track(context,	play, ssrc);
	if (video_track) {

		video_track->update_send_report_time(&video_track->session.track, ntp,	rtp_time);
	}
#endif
#if Yang_Enable_RTC_Audio
	YangPlayTrackAudio *audio_track = yang_rtcplay_get_audio_track(context, play,ssrc);
	if (audio_track) {
		audio_track->update_send_report_time(&audio_track->session, ntp, rtp_time);

	}
#endif
}

int32_t yang_rtcplay_send_rtcp_rr(YangRtcContext *context,
		YangRtcPlayStream *play) {
	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;
	int32_t err = Yang_Ok;

#if Yang_Enable_RTC_Video
	if ((err = play->videoTrack->send_rtcp_rr(context, &play->videoTrack->session.track))
			!= Yang_Ok) {
		return yang_error_wrap(err, "track=%u", play->videoTrack->session.track.ssrc);
	}
#endif

#if Yang_Enable_RTC_Audio
	if ((err = play->audioTrack->send_rtcp_rr(context, &play->audioTrack->session))
			!= Yang_Ok) {
		return yang_error_wrap(err, "track=%u", play->audioTrack->session.ssrc);
	}
#endif

	return err;
}

int32_t yang_rtcplay_send_rtcp_xr_rrtr(YangRtcContext *context,	YangRtcPlayStream *play) {
	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;
	int32_t err = Yang_Ok;

#if Yang_Enable_RTC_Video
	if ((err = play->videoTrack->send_rtcp_xr_rrtr(context,&play->videoTrack->session.track)) != Yang_Ok) {
		return yang_error_wrap(err, "track=%u", play->videoTrack->session.track.ssrc);
	}
#endif

#if Yang_Enable_RTC_Audio
	if ((err = play->audioTrack->send_rtcp_xr_rrtr(context, &play->audioTrack->session))
			!= Yang_Ok) {
		return yang_error_wrap(err, "track=%u", play->audioTrack->session.ssrc);
	}
#endif

	return err;
}


int32_t yang_rtcplay_on_rtp(YangRtcContext *context, YangRtcPlayStream *play,
		char *data, int32_t nb_data) {
	if (context == NULL || play == NULL)	return ERROR_RTC_PLAY;
	int32_t err = Yang_Ok;
	int32_t nb_plaintext = nb_data;
	uint32_t ssrc = 0;
	char *plaintext = data;
	//YangPlayTrackAudio *audio_track = NULL;
//	YangPlayTrackVideo *video_track = NULL;

#if Yang_Enable_TWCC
	 if (play->twccEnabled&&(data[0]&0x10)) {
		 uint16_t twcc_sn = 0;
		 if ((err = yang_rtp_fast_parse_twcc(data, nb_data, play->twccId, &twcc_sn))== Yang_Ok) {
			 if((err = yang_rtcplay_on_twcc(context,play,twcc_sn)) != Yang_Ok) {
				 return yang_error_wrap(err, "on twcc");
			 }
		 }
	 }
#endif


#if Yang_Enable_Dtls
	if ((err = yang_dec_rtp(&context->srtp, plaintext, &nb_plaintext))!= Yang_Ok) {
		if (err == srtp_err_status_replay_fail)
			return Yang_Ok;
		YangBuffer b;
		yang_init_buffer(&b, data, nb_data);
		YangRtpHeader h;
		yang_memset(&h, 0, sizeof(YangRtpHeader));
		h.ignore_padding = yangtrue;

		yang_decode_rtpHeader(&b, &h);
		return yang_error_wrap(err,
				"marker=%u, pt=%u, seq=%u, ts=%u, ssrc=%u, pad=%u, payload=%uB",
				h.marker, h.payload_type, h.sequence, h.timestamp, h.ssrc,
				h.padding_length, nb_data - yang_buffer_pos(&b));
	}
#endif

	yang_reset_rtpPacket(&play->rtp);
	yang_init_buffer(&play->buf,yang_wrap_rtpPacket(&play->rtp, play->rtpBuffer, plaintext,	nb_plaintext), nb_plaintext);

	play->rtp.header.ignore_padding = yangfalse;

	if ((err = yang_decode_rtpPacket(&play->rtp, &play->buf)) != Yang_Ok) {
		return yang_error_wrap(err, "decode rtp packet");
	}

	ssrc = play->rtp.header.ssrc;
#if Yang_Enable_RTC_Audio
	YangPlayTrackAudio* audio_track = yang_rtcplay_get_audio_track(context, play,ssrc);



	if (audio_track) {
		play->rtp.frame_type = YangFrameTypeAudio;
		if ((err = audio_track->on_rtp(context, &audio_track->session,&play->rtp)) != Yang_Ok) {
			return yang_error_wrap(err, "on audio");
		}
		context->stats.on_play_audioRtp(&context->stats.recvStats,&play->rtp);
		if ((err = audio_track->on_nack(context, &audio_track->session, &play->rtp)) != Yang_Ok) {
			return yang_error_wrap(err, "on nack");
		}
		return err;

	}
#endif

#if Yang_Enable_RTC_Video
	YangPlayTrackVideo* video_track = yang_rtcplay_get_video_track(context,	play, ssrc);
	if (video_track) {
		play->rtp.frame_type = YangFrameTypeVideo;
		if ((err = video_track->on_rtp(context, &video_track->session, &play->rtp))!= Yang_Ok) {
			return yang_error_wrap(err, "on video");
		}
		context->stats.on_play_videoRtp(&context->stats.recvStats,&play->rtp);
		if ((err = video_track->on_nack(context, &video_track->session.track,	&play->rtp)) != Yang_Ok) {
				return yang_error_wrap(err, "on nack");
			}
		return err;
	}
#endif

		return yang_error_wrap(ERROR_RTC_RTP, "unknown ssrc=%u,sn=%hu,size=%d", ssrc,play->rtp.header.sequence,play->rtp.nb);



}

int32_t yang_rtcplay_check_send_nacks(YangRtcContext *context,
		YangRtcPlayStream *play) {
	if (context == NULL || play == NULL)	return ERROR_RTC_PLAY;
	int32_t err = Yang_Ok;

#if Yang_Enable_RTC_Video
	if ((err = play->videoTrack->check_nacks(context, &play->videoTrack->session))
			!= Yang_Ok) {
		return yang_error_wrap(err, "video track=%d",
				play->videoTrack->session.track.ssrc);
	}
#endif

#if Yang_Enable_RTC_Audio
	if ((err = play->audioTrack->check_nacks(context, &play->audioTrack->session))
			!= Yang_Ok) {
		return yang_error_wrap(err, "audio track=%d", play->audioTrack->session.ssrc);
	}

#endif
	return err;

}



int32_t yang_rtcplay_send_periodic_twcc(YangRtcContext *context,YangRtcPlayStream *play) {
	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;
	int32_t err = Yang_Ok;
#if Yang_Enable_TWCC
	if(play->twcc->sns.vsize==0) return err;
	play->last_time_send_twcc = yang_get_system_time();
	// limit the max count=1024 to avoid dead loop.

	play->twcc->isSendtwcc=1;
	play->twcc->fb_pkt_count=play->twccFbCount;
	play->twccFbCount++;

	yang_send_rtcp_fb_twcc(context,play->twcc,play->twccFbCount);
	play->twcc->isSendtwcc=0;

#endif
	return err;
}



int32_t yang_rtcplay_on_rtcp_xr(YangRtcContext *context,
		YangRtcPlayStream *play, YangRtcpCommon *rtcp) {
	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;
	int32_t err = Yang_Ok;

	/*
	 @see: http://www.rfc-editor.org/rfc/rfc3611.html#section-2

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |V=2|P|reserved |   PT=XR=207   |             length            |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                              SSRC                             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 :                         report blocks                         :
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	/**
	 * @see: http://www.rfc-editor.org/rfc/rfc3611.html#section-4.4

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |     BT=4      |   reserved    |       block length = 2        |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |              NTP timestamp, most significant word             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |             NTP timestamp, least significant word             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 **/
	YangBuffer stream;
	yang_init_buffer(&stream, rtcp->data, rtcp->nb_data);
	yang_read_1bytes(&stream);
	uint8_t pt = yang_read_1bytes(&stream);
	if (pt != kXR)
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "invalid XR packet");

	uint16_t length = (yang_read_2bytes(&stream) + 1) * 4;
	yang_read_4bytes(&stream);

	if (length > rtcp->nb_data) {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
				"invalid XR packet, length=%u, nb_buf=%d", length,
				rtcp->nb_data);
	}

	while (yang_buffer_pos(&stream) + 4 < length) {
		uint8_t bt = yang_read_1bytes(&stream);
		yang_buffer_skip(&stream, 1);
		int32_t ptblocklength=yang_read_2bytes(&stream);
		uint16_t block_length = ( ptblocklength + 1) * 4;

		if (yang_buffer_pos(&stream) + block_length - 4 > rtcp->nb_data) {
			return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
					"invalid XR packet block, block_length=%u, nb_buf=%d",
					block_length, rtcp->nb_data);
		}

		if (bt == 5) {
			for (int32_t i = 4; i < block_length; i += 12) {
				uint32_t ssrc = yang_read_4bytes(&stream);
				uint32_t lrr = yang_read_4bytes(&stream);
				uint32_t dlrr = yang_read_4bytes(&stream);

				YangNtp cur_ntp;
				yang_ntp_from_time_ms(&cur_ntp,
				yang_update_system_time() / 1000);
				uint32_t compact_ntp = (cur_ntp.ntp_second << 16)
						| (cur_ntp.ntp_fractions >> 16);

				int32_t rtt_ntp = compact_ntp - lrr - dlrr;
				int32_t rtt = ((rtt_ntp * 1000) >> 16)
						+ ((rtt_ntp >> 16) * 1000);
				yang_trace("ssrc=%u, compact_ntp=%u, lrr=%u, dlrr=%u, rtt=%d",
						ssrc, compact_ntp, lrr, dlrr, rtt);
				if(context->streamConfig->direction==YangRecvonly)
					yang_rtcplay_update_rtt(context, play, ssrc, rtt);
			}
		}else if(bt == 4){
        	//uint32_t rrt_m = yang_read_4bytes(&stream);
        	//uint32_t rrt_l = yang_read_4bytes(&stream);
			yang_buffer_skip(&stream,4);
			yang_buffer_skip(&stream,4);
        }
	}

	return err;
}




int32_t yang_rtcplay_on_rtcp_sr(YangRtcContext *context,YangRtcPlayStream *play, YangRtcpCommon *rtcp) {
	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;
	int32_t err = Yang_Ok;
	YangNtp yang_ntp;
	yang_ntp_to_time_ms(&yang_ntp, rtcp->sr->ntp);
	yang_rtcplay_update_send_report_time(context, play, rtcp->ssrc, &yang_ntp,
			rtcp->sr->rtp_ts);

	return err;
}

int32_t yang_rtcplay_on_rtcp(YangRtcContext *context, YangRtcPlayStream *play, YangRtcpCommon *rtcp) {
	if (context == NULL || play == NULL)	return ERROR_RTC_PLAY;
	if (YangRtcpType_sr == rtcp->header.type) {

		return yang_rtcplay_on_rtcp_sr(context, play, rtcp);
	} else if (YangRtcpType_xr == rtcp->header.type) {

		return yang_rtcplay_on_rtcp_xr(context, play, rtcp);
	} else if (YangRtcpType_sdes == rtcp->header.type) {
		//ignore RTCP SDES
		return Yang_Ok;
	} else if (YangRtcpType_bye == rtcp->header.type) {

		return Yang_Ok;
	}
	return Yang_Ok;
}

void yang_create_rtcplay(YangRtcContext *context, YangRtcPlay *pplay,YangRtpBuffer *rtpBuffer) {
	if (context == NULL || pplay == NULL)		return;
	YangRtcPlayStream* play = (YangRtcPlayStream*) yang_calloc(1,sizeof(YangRtcPlayStream));
	pplay->playStream=play;
	play->request_keyframe = yangfalse;
	play->twccEnabled = yangfalse;

	yang_memset(&play->rtp, 0, sizeof(YangRtpPacket));
	yang_create_rtpPacket(&play->rtp);

	play->rtpBuffer = rtpBuffer;
#if Yang_Enable_RTC_Audio
	play->audioTrack = (YangPlayTrackAudio*) yang_calloc(1, sizeof(YangPlayTrackAudio));
	yang_create_playTrackAudio(context, play->audioTrack);
#endif
#if Yang_Enable_RTC_Video
	play->videoTrack = (YangPlayTrackVideo*) yang_calloc(1,	sizeof(YangPlayTrackVideo));
	yang_create_playvideoTrack(context, play->videoTrack);
#endif
	play->twccEnabled = context->twccId>0?1:0;

#if Yang_Enable_TWCC
	int32_t twcc_id = context->twccId;
	if (play->twccEnabled) {
		if(play->twcc==NULL){
			play->twcc=(YangRecvTWCC*)yang_calloc(1,sizeof(YangRecvTWCC));
			yang_create_recvtwcc(play->twcc);
		}
		play->twccId = twcc_id;
		play->twcc->mediaSsrc=play->videoTrack->session.track.ssrc;
	}
#endif

	pplay->update_rtt=yang_rtcplay_update_rtt;
	pplay->on_rtcp=yang_rtcplay_on_rtcp;
	pplay->on_rtp=yang_rtcplay_on_rtp ;
	pplay->on_rtcp_sr=yang_rtcplay_on_rtcp_sr ;
#if Yang_Enable_RTC_Audio
	pplay->get_audio_track=yang_rtcplay_get_audio_track;
#endif
#if Yang_Enable_RTC_Video
	pplay->get_video_track=yang_rtcplay_get_video_track;
#endif
	pplay->on_rtcp_xr=yang_rtcplay_on_rtcp_xr;
	pplay->update_send_report_time=yang_rtcplay_update_send_report_time;
	pplay->send_rtcp_xr_rrtr=yang_rtcplay_send_rtcp_xr_rrtr;
	pplay->send_rtcp_rr=yang_rtcplay_send_rtcp_rr;
	pplay->send_periodic_twcc=yang_rtcplay_send_periodic_twcc;

}

void yang_destroy_rtcplay(YangRtcPlay *pplay) {
	if (pplay == NULL||pplay->playStream==NULL)	return;
	YangRtcPlayStream* play=pplay->playStream;
#if Yang_Enable_RTC_Audio
	yang_destroy_playTrackAudio(play->audioTrack);
	yang_free(play->audioTrack);
#endif
#if Yang_Enable_RTC_Video
	yang_destroy_playvideoTrack(play->videoTrack);
	yang_free(play->videoTrack);
#endif
	yang_destroy_rtpPacket(&play->rtp);
	if(play->twcc){
		yang_free(play->twcc);
	}
	yang_free(pplay->playStream);
}
