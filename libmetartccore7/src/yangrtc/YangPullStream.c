//
// Copyright (c) 2019-2025 yanggaofeng
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
#include <yangrtc/YangPullStream.h>
#include <yangrtc/YangPullTrackAudio.h>
#include <yangrtc/YangPullTrackVideo.h>
#include <yangrtc/YangPullTrackVideo.h>

#include <math.h>

int32_t yang_rtcplay_on_twcc(YangRtcContext *context, YangRtcPullStream *play,uint16_t sn) {
	int32_t err = Yang_Ok;

	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;

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
static YangPullTrackVideo* yang_get_video_track(YangRtcContext *context,
		YangRtcPullStream *play, uint32_t ssrc) {
	if (context == NULL || play == NULL)
		return NULL;

	if (play->videoTrack->has_ssrc(context, &play->videoTrack->session.track, ssrc)) {
		return play->videoTrack;
	}


	return NULL;
}
#endif


#if Yang_Enable_RTC_Audio
static YangPullTrackAudio* yang_get_audio_track(YangRtcContext *context,
		YangRtcPullStream *play, uint32_t ssrc) {
	if (context == NULL || play == NULL)
		return NULL;

	if (play->audioTrack->has_ssrc(context, &play->audioTrack->session, ssrc)) {
		return play->audioTrack;
	}


	return NULL;
}
#endif
static void yang_update_rtt(YangRtcContext *context, YangRtcPullStream *play,
		uint32_t ssrc, int32_t rtt) {
	if (context == NULL || play == NULL)
		return;
#if Yang_Enable_RTC_Video
	YangPullTrackVideo *video_track = yang_get_video_track(context,
			play, ssrc);

	if (video_track) {
		return video_track->update_rtt(&video_track->session.track, rtt);
	}
#endif
#if Yang_Enable_RTC_Audio
	YangPullTrackAudio *audio_track = yang_get_audio_track(context, play,
			ssrc);
	if (audio_track) {
		return audio_track->update_rtt(&audio_track->session, rtt);
	}
#endif
}

static void yang_update_send_report_time(YangRtcContext *context,
		YangRtcPullStream *play, uint32_t ssrc, YangNtp *ntp, uint32_t rtp_time) {
	YangPullTrackAudio *audio_track;
	YangPullTrackVideo *video_track;
	if (context == NULL || play == NULL)
		return;
#if Yang_Enable_RTC_Video
	video_track = yang_get_video_track(context,	play, ssrc);
	if (video_track) {

		video_track->update_send_report_time(&video_track->session.track, ntp,	rtp_time);
	}
#endif
#if Yang_Enable_RTC_Audio
	audio_track = yang_get_audio_track(context, play,ssrc);
	if (audio_track) {
		audio_track->update_send_report_time(&audio_track->session, ntp, rtp_time);

	}
#endif
}

static int32_t yang_pullStream_send_rtcp_rr(YangRtcContext *context,
		YangRtcPullStream *play) {
	int32_t err = Yang_Ok;

	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;


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

static int32_t yang_pullStream_send_rtcp_xr_rrtr(YangRtcContext *context,	YangRtcPullStream *play) {
	int32_t err = Yang_Ok;

	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;


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


static int32_t yang_on_rtp(YangRtcContext *context, YangRtcPullStream *play,
		char *data, int32_t nb_data) {
	int32_t err = Yang_Ok;
	int32_t nb_plaintext = nb_data;
	uint32_t ssrc = 0;
	char *plaintext = data;
	YangPullTrackAudio* audio_track;
	YangPullTrackVideo* video_track;

	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;


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
	audio_track = yang_get_audio_track(context, play,ssrc);
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
	video_track = yang_get_video_track(context,	play, ssrc);
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
		YangRtcPullStream *play) {
	int32_t err = Yang_Ok;

	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;


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



static int32_t yang_send_periodic_twcc(YangRtcContext *context,YangRtcPullStream *play) {
	int32_t err = Yang_Ok;

	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;

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



static int32_t yang_on_rtcp_xr(YangRtcContext *context,
		YangRtcPullStream *play, YangRtcpCommon *rtcp) {
	uint8_t pt;
	uint8_t bt;
	uint16_t length;
	uint16_t block_length;
	int32_t ptblocklength,i;
	int32_t err = Yang_Ok;

	uint32_t ssrc;
	uint32_t lrr;
	uint32_t dlrr;
	uint32_t compact_ntp;
	int32_t rtt_ntp;
	int32_t rtt ;

	YangBuffer stream;

	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;


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

	yang_init_buffer(&stream, rtcp->data, rtcp->nb_data);
	yang_read_1bytes(&stream);
	pt = yang_read_1bytes(&stream);
	if (pt != kXR)
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK, "invalid XR packet");

	length = (yang_read_2bytes(&stream) + 1) * 4;
	yang_read_4bytes(&stream);

	if (length > rtcp->nb_data) {
		return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
				"invalid XR packet, length=%u, nb_buf=%d", length,
				rtcp->nb_data);
	}


	while (yang_buffer_pos(&stream) + 4 < length) {
		bt = yang_read_1bytes(&stream);
		yang_buffer_skip(&stream, 1);
		ptblocklength=yang_read_2bytes(&stream);
		block_length = ( ptblocklength + 1) * 4;

		if (yang_buffer_pos(&stream) + block_length - 4 > rtcp->nb_data) {
			return yang_error_wrap(ERROR_RTC_RTCP_CHECK,
					"invalid XR packet block, block_length=%u, nb_buf=%d",
					block_length, rtcp->nb_data);
		}

		if (bt == 5) {
			for (i = 4; i < block_length; i += 12) {
				ssrc = yang_read_4bytes(&stream);
				lrr = yang_read_4bytes(&stream);
				dlrr = yang_read_4bytes(&stream);

				YangNtp cur_ntp;
				yang_ntp_from_time_ms(&cur_ntp,
				yang_update_system_time() / 1000);
				compact_ntp = (cur_ntp.ntp_second << 16)
						| (cur_ntp.ntp_fractions >> 16);

				rtt_ntp = compact_ntp - lrr - dlrr;
				rtt = ((rtt_ntp * 1000) >> 16)
						+ ((rtt_ntp >> 16) * 1000);
				yang_trace("ssrc=%u, compact_ntp=%u, lrr=%u, dlrr=%u, rtt=%d",
						ssrc, compact_ntp, lrr, dlrr, rtt);
				if(context->peerInfo->direction==YangRecvonly)
					yang_update_rtt(context, play, ssrc, rtt);
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




static int32_t yang_on_rtcp_sr(YangRtcContext *context,YangRtcPullStream *play, YangRtcpCommon *rtcp) {
	int32_t err = Yang_Ok;
	YangNtp yang_ntp;

	if (context == NULL || play == NULL)
		return ERROR_RTC_PLAY;

	yang_ntp_to_time_ms(&yang_ntp, rtcp->sr->ntp);
	yang_update_send_report_time(context, play, rtcp->ssrc, &yang_ntp,
			rtcp->sr->rtp_ts);

	return err;
}

static int32_t yang_on_rtcp(YangRtcContext *context, YangRtcPullStream *play, YangRtcpCommon *rtcp) {
	if (context == NULL || play == NULL)	return ERROR_RTC_PLAY;
	if (YangRtcpType_sr == rtcp->header.type) {

		return yang_on_rtcp_sr(context, play, rtcp);
	} else if (YangRtcpType_xr == rtcp->header.type) {

		return yang_on_rtcp_xr(context, play, rtcp);
	} else if (YangRtcpType_sdes == rtcp->header.type) {
		//ignore RTCP SDES
		return Yang_Ok;
	} else if (YangRtcpType_bye == rtcp->header.type) {

		return Yang_Ok;
	}
	return Yang_Ok;
}

void yang_create_rtcpull(YangRtcContext *context, YangRtcPull *pull,YangRtpBuffer *rtpBuffer) {
	YangRtcPullStream* pullStream;

	if (context == NULL || pull == NULL)
		return;

	pullStream = (YangRtcPullStream*) yang_calloc(1,sizeof(YangRtcPullStream));
	pull->pullStream=pullStream;
	pullStream->request_keyframe = yangfalse;
	pullStream->twccEnabled = yangfalse;

	yang_memset(&pullStream->rtp, 0, sizeof(YangRtpPacket));
	yang_create_rtpPacket(&pullStream->rtp);

	pullStream->rtpBuffer = rtpBuffer;
#if Yang_Enable_RTC_Audio
	pullStream->audioTrack = (YangPullTrackAudio*) yang_calloc(1, sizeof(YangPullTrackAudio));
	yang_create_pullTrackAudio(context, pullStream->audioTrack);
#endif
#if Yang_Enable_RTC_Video
	pullStream->videoTrack = (YangPullTrackVideo*) yang_calloc(1,	sizeof(YangPullTrackVideo));
	yang_create_pullVideoTrack(context, pullStream->videoTrack);
#endif
	pullStream->twccEnabled = context->twccId>0?1:0;

#if Yang_Enable_TWCC
	int32_t twcc_id = context->twccId;
	if (pullStream->twccEnabled) {
		if(pullStream->twcc==NULL){
			pullStream->twcc=(YangRecvTWCC*)yang_calloc(1,sizeof(YangRecvTWCC));
			yang_create_recvtwcc(pullStream->twcc);
		}
		pullStream->twccId = twcc_id;
		pullStream->twcc->mediaSsrc=pullStream->videoTrack->session.track.ssrc;
	}
#endif

	pull->update_rtt=yang_update_rtt;
	pull->on_rtcp=yang_on_rtcp;
	pull->on_rtp=yang_on_rtp ;
	pull->on_rtcp_sr=yang_on_rtcp_sr ;
#if Yang_Enable_RTC_Audio
	pull->get_audio_track=yang_get_audio_track;
#endif
#if Yang_Enable_RTC_Video
	pull->get_video_track=yang_get_video_track;
#endif
	pull->on_rtcp_xr=yang_on_rtcp_xr;
	pull->update_send_report_time=yang_update_send_report_time;
	pull->send_rtcp_xr_rrtr=yang_pullStream_send_rtcp_xr_rrtr;
	pull->send_rtcp_rr=yang_pullStream_send_rtcp_rr;
	pull->send_periodic_twcc=yang_send_periodic_twcc;

}

void yang_destroy_rtcpull(YangRtcPull *pull) {
	YangRtcPullStream* pullStream;

	if (pull == NULL||pull->pullStream==NULL)
		return;

	pullStream=pull->pullStream;
#if Yang_Enable_RTC_Audio
	yang_destroy_pullTrackAudio(pullStream->audioTrack);
	yang_free(pullStream->audioTrack);
#endif
#if Yang_Enable_RTC_Video
	yang_destroy_playVideoTrack(pullStream->videoTrack);
	yang_free(pullStream->videoTrack);
#endif
	yang_destroy_rtpPacket(&pullStream->rtp);
	if(pullStream->twcc){
		yang_free(pullStream->twcc);
	}
	yang_free(pull->pullStream);
}
