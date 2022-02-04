//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangwebrtc/YangRtcConnection.h>
#include <yangwebrtc/YangRtcConnection.h>
#include <yangwebrtc/YangUdpHandle.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangstream/YangStreamType.h>

int32_t yang_send_nackpacket(YangRtcContext *context, char *data, int32_t nb) {
	int32_t err = Yang_Ok;
	int32_t nn_encrypt = nb;

	if ((err = yang_enc_rtp(&context->srtp, data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}

	return yang_rtc_sendData(context->udp, data, nb);
}
void yang_do_request_keyframe(YangRtcContext *context, uint32_t ssrc) {
	if (context && context->avcontext && context->avcontext->sendRequest)
		context->avcontext->sendRequest(context->streamConf->uid, ssrc,
				Yang_Req_Sendkeyframe,context->user);

}
int32_t yang_send_rtcp_fb_pli(YangRtcContext *context, uint32_t ssrc) {
	int32_t err = Yang_Ok;
	char buf[kRtpPacketSize];
	YangBuffer stream;
	yang_init_buffer(&stream, buf, sizeof(buf));
	yang_write_1bytes(&stream, 0x81);
	yang_write_1bytes(&stream, kPsFb);
	yang_write_2bytes(&stream, 2);
	yang_write_4bytes(&stream, ssrc);
	yang_write_4bytes(&stream, ssrc);

	int32_t nb_protected_buf = yang_buffer_pos(&stream);
	if ((err = yang_enc_rtcp(&context->srtp, stream.data, &nb_protected_buf))
			!= Yang_Ok) {
		return yang_error_wrap(err, "protect rtcp psfb pli");
	}

	return yang_rtc_sendData(context->udp, stream.data, nb_protected_buf);
}
int32_t yang_send_rtcp_rr(YangRtcContext *context, uint32_t ssrc,
		YangReceiveNackBuffer *rtp_queue, const uint64_t last_send_systime,
		YangNtp *last_send_ntp) {
	int32_t err = Yang_Ok;
	// @see https://tools.ietf.org/html/rfc3550#section-6.4.2
	char buf[kRtpPacketSize];

	YangBuffer stream;
	yang_init_buffer(&stream, buf, sizeof(buf));
	yang_write_1bytes(&stream, 0x81);
	yang_write_1bytes(&stream,  kRR);
	yang_write_2bytes(&stream, 7);
	yang_write_4bytes(&stream, ssrc); // TODO: FIXME: Should be 1?

	uint8_t fraction_lost = 0;
	uint32_t cumulative_number_of_packets_lost = 0 & 0x7FFFFF;
	uint32_t extended_highest_sequence =
	yang_nackbuffer_get_extended_highest_sequence(rtp_queue);

	uint32_t interarrival_jitter = 0;

	uint32_t rr_lsr = 0;
	uint32_t rr_dlsr = 0;

	if (last_send_systime > 0) {
		rr_lsr = (last_send_ntp->ntp_second << 16)
				| (last_send_ntp->ntp_fractions >> 16);
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

	int32_t nb_protected_buf = yang_buffer_pos(&stream);
	if ((err = yang_enc_rtcp(&context->srtp, stream.data, &nb_protected_buf))
			!= Yang_Ok) {
		return yang_error_wrap(err, "protect rtcp rr");
	}

	return yang_rtc_sendData(context->udp, stream.data, nb_protected_buf);
}

int32_t yang_send_rtcp_xr_rrtr(YangRtcContext *context, uint32_t ssrc) {
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
	yang_write_1bytes(&stream,  kXR);
	yang_write_2bytes(&stream, 4);
	yang_write_4bytes(&stream, ssrc);
	yang_write_1bytes(&stream, 4);
	yang_write_1bytes(&stream, 0);
	yang_write_2bytes(&stream, 2);
	yang_write_4bytes(&stream, cur_ntp.ntp_second);
	yang_write_4bytes(&stream, cur_ntp.ntp_fractions);

	int32_t nb_protected_buf = yang_buffer_pos(&stream);
	if ((err = yang_enc_rtcp(&context->srtp, stream.data, &nb_protected_buf))
			!= Yang_Ok) {
		return yang_error_wrap(err, "protect rtcp xr");
	}

	return yang_rtc_sendData(context->udp, stream.data, nb_protected_buf);
}

int32_t yang_check_send_nacks(YangRtcContext *context, YangRtpRecvNack *nack,
		uint32_t ssrc, uint32_t *sent_nacks, uint32_t *timeout_nacks) {


	YangRtcpCommon rtcp;
	memset(&rtcp, 0, sizeof(YangRtcpCommon));
	yang_init_rtcpNack(&rtcp, ssrc);

	yang_recvnack_get_nack_seqs(nack, rtcp.nack, timeout_nacks);
	if (rtcp.nack->vlen == 0) {
		return Yang_Ok;
	}

	char buf[kRtcpPacketSize];
	YangBuffer stream;
	yang_init_buffer(&stream, buf, sizeof(buf));

	yang_encode_rtcpNack(&rtcp, &stream);
	int32_t nb_protected_buf = yang_buffer_pos(&stream);

	yang_enc_rtcp(&context->srtp, stream.data, &nb_protected_buf);

	yang_rtc_sendData(context->udp, stream.data, nb_protected_buf);
	yang_destroy_rtcpNack(&rtcp);
	return Yang_Ok;
}
