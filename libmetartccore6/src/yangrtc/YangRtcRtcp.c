//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcSocket.h>

#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangRtcSession.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSRtp.h>

#include <yangrtp/YangRtpConstant.h>
#include <yangstream/YangStreamType.h>

int32_t yang_send_rtcppacket(YangRtcContext *context, char *data, int32_t nb) {
	int32_t err = Yang_Ok;
	int32_t nn_encrypt = nb;

#if Yang_Enable_Dtls
	if ((err = yang_enc_rtcp(&context->srtp, data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}
#endif

	return context->sock->write(&context->sock->session, data, nn_encrypt);
}


void yang_do_request_keyframe(YangRtcContext *context, uint32_t ssrc) {

	if (context && context->streamConfig && context->streamConfig->rtcCallback.sendRequest)
		context->streamConfig->rtcCallback.sendRequest(context->streamConfig->rtcCallback.context,context->streamConfig->uid, ssrc,
				Yang_Req_Sendkeyframe);

}

int32_t yang_send_rtcp_fb_pli(YangRtcContext *context, uint32_t ssrc) {

	char buf[kRtpPacketSize];
	YangBuffer stream;
	yang_init_buffer(&stream, buf, sizeof(buf));
	yang_write_1bytes(&stream, 0x81);
	yang_write_1bytes(&stream, kPsFb);
	yang_write_2bytes(&stream, 2);
	yang_write_4bytes(&stream, ssrc);
	yang_write_4bytes(&stream, ssrc);
	return yang_send_rtcppacket(context,stream.data,yang_buffer_pos(&stream));
}


int32_t yang_send_rtcp_fb_twcc(YangRtcContext *context, YangRecvTWCC* twcc,int32_t twcc_fb_count) {
	int32_t err = Yang_Ok;
	char pkt[kRtpPacketSize];
	YangBuffer stream ;
	yang_init_buffer(&stream,pkt, sizeof(pkt));
	if ((err = yang_twcc_recv_encode(twcc,&stream)) != Yang_Ok) {
		return yang_error_wrap(err, "encode, count=%u",twcc_fb_count);
	}

	return yang_send_rtcppacket(context,stream.data,yang_buffer_pos(&stream));

}

int32_t yang_send_rtcp_sr(YangRtcContext *context, yangbool isAudio,uint32_t ssrc) {

	YangSendStats* stats=&context->stats.sendStats;
	if(isAudio&&stats->audioStartTime==0) return Yang_Ok;
	if(!isAudio&&stats->videoStartTime==0) return Yang_Ok;
	uint64_t stamp=yang_get_system_time() - (isAudio?stats->audioStartTime:stats->videoStartTime);

	uint64_t ntp=yang_get_ntptime_fromms(stamp/1000);
	uint32_t ts=stamp*(isAudio?context->avinfo->audio.sample:90000)/YANG_UTIME_SECONDS;

	char buf[kRtpPacketSize];

	YangBuffer stream;
	yang_init_buffer(&stream, buf, sizeof(buf));
	yang_write_1bytes(&stream, 0x80);
	yang_write_1bytes(&stream, kSR);
	yang_write_2bytes(&stream, 6);
	yang_write_4bytes(&stream, ssrc);

	yang_write_8bytes(&stream, ntp);
	yang_write_4bytes(&stream, ts);
	yang_write_4bytes(&stream, isAudio?stats->audioRtpPacketCount:stats->videoRtpPacketCount);
	yang_write_4bytes(&stream, isAudio?stats->audioRtpBytes:stats->videoRtpBytes);

	return yang_send_rtcppacket(context,stream.data,yang_buffer_pos(&stream));
}



int32_t yang_send_rtcp_rr(YangRtcContext *context,yangbool isAudio, uint32_t ssrc,
		YangReceiveNackBuffer *rtp_queue, const uint64_t last_send_systime,
		YangNtp *last_send_ntp) {

	// @see https://tools.ietf.org/html/rfc3550#section-6.4.2
	uint8_t fraction_lost = 0;
	uint32_t cumulative_number_of_packets_lost = 0 & 0x7FFFFF;
	uint32_t extended_highest_sequence =
			yang_nackbuffer_get_extended_highest_sequence(rtp_queue);
	YangBuffer stream;
	char buf[kRtpPacketSize];
	yang_init_buffer(&stream, buf, sizeof(buf));
	yang_write_1bytes(&stream, 0x81);
	yang_write_1bytes(&stream, kRR);
	yang_write_2bytes(&stream, 7);
	yang_write_4bytes(&stream, ssrc);


	if(!isAudio){
		fraction_lost=context->stats.getFractionLost(&context->stats.recvStats.video);
		cumulative_number_of_packets_lost=context->stats.getLostCount(&context->stats.recvStats.video) & 0x7FFFFF;
	}

	uint32_t interarrival_jitter = 0;

	uint32_t rr_lsr = 0;
	uint32_t rr_dlsr = 0;

	if (last_send_systime > 0) {
		rr_lsr = (last_send_ntp->ntp_second << 16)	| (last_send_ntp->ntp_fractions >> 16);
		uint32_t dlsr = (yang_update_system_time() - last_send_systime) / 1000;
		rr_dlsr = ((dlsr / 1000) << 16) | ((dlsr % 1000) * 65536 / 1000);
	}

	yang_write_4bytes(&stream, ssrc);
	yang_write_1bytes(&stream, fraction_lost);
	yang_write_3bytes(&stream, cumulative_number_of_packets_lost);
	yang_write_4bytes(&stream, extended_highest_sequence);
	yang_write_4bytes(&stream, interarrival_jitter);
	yang_write_4bytes(&stream, rr_lsr);
	yang_write_4bytes(&stream, rr_dlsr);

	return yang_send_rtcppacket(context,stream.data,yang_buffer_pos(&stream));
}

int32_t yang_send_rtcp_xr_rrtr(YangRtcContext *context, uint32_t ssrc) {


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

	 @see: http://www.rfc-editor.org/rfc/rfc3611.html#section-4.4

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |     BT=4      |   reserved    |       block length = 2        |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |              NTP timestamp, most significant word             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |             NTP timestamp, least significant word             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	int64_t now = yang_update_system_time();
	YangNtp cur_ntp;
	yang_ntp_from_time_ms(&cur_ntp, now / 1000);

	char buf[kRtpPacketSize];

	YangBuffer stream;
	yang_init_buffer(&stream, buf, sizeof(buf));
	yang_write_1bytes(&stream, 0x80);
	yang_write_1bytes(&stream, kXR);
	yang_write_2bytes(&stream, 4);
	yang_write_4bytes(&stream, ssrc);
	yang_write_1bytes(&stream, 4);
	yang_write_1bytes(&stream, 0);
	yang_write_2bytes(&stream, 2);
	yang_write_4bytes(&stream, cur_ntp.ntp_second);
	yang_write_4bytes(&stream, cur_ntp.ntp_fractions);

	return yang_send_rtcppacket(context,stream.data,yang_buffer_pos(&stream));
}

int32_t yang_check_send_nacks(YangRtcContext *context, YangRtpRecvNack *nack,
		uint32_t ssrc, uint32_t *sent_nacks, uint32_t *timeout_nacks) {

	int32_t err=Yang_Ok;
    yang_recvnack_initvec(nack,ssrc);
	yang_rtcpNack_clear(nack->rtcp.nack);

	yang_recvnack_get_nack_seqs(nack, nack->rtcp.nack, timeout_nacks);
	if (nack->rtcp.nack->vsize == 0) 		return Yang_Ok;

	char buf[kRtcpPacketSize];
	YangBuffer stream;
	yang_init_buffer(&stream, buf, sizeof(buf));

	yang_rtcpNack_init(&nack->rtcp,ssrc);

	yang_encode_rtcpNack(&nack->rtcp, &stream);
	int32_t nb_protected_buf = yang_buffer_pos(&stream);

#if Yang_Enable_Dtls
	if((err=yang_enc_rtcp(&context->srtp, stream.data, &nb_protected_buf))!=Yang_Ok){
		yang_rtcpNack_clear(nack->rtcp.nack);
		return yang_error_wrap(err, "check send nacks");
	}
#endif

	context->sock->write(&context->sock->session, stream.data, nb_protected_buf);
	yang_rtcpNack_clear(nack->rtcp.nack);

	return Yang_Ok;
}


