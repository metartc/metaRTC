#include <yangwebrtc/YangCRecvTrack.h>
#include <yangutil/sys/YangLog.h>
#include <yangwebrtc/YangRtcConnection.h>
#include <math.h>

void yang_init_recvTrack(YangRtcContext *context, YangRecvTrack *recv,
		YangRtpBuffer *rtpBuffer, int is_audio) {
	if (recv == NULL)
		return;
	recv->uid = context->streamConf->uid;

	//recv->track_desc = ptrack_desc;
	recv->rtpBuffer = rtpBuffer;
	recv->isAudio = is_audio;
	recv->rtp_queue = (YangReceiveNackBuffer*) malloc(
			sizeof(YangReceiveNackBuffer)); //new YangRtpPlayNackBuffer(100);
	recv->nack_receiver = (YangRtpRecvNack*) malloc(sizeof(YangRtpRecvNack)); // new YangRtpNackForReceiver(recv->rtp_queue, 100 * 2 / 3);
	if (is_audio) {
		recv->ssrc_ = context->audioTrack.ssrc_;
		yang_init_nackBuffer(recv->rtp_queue, 100);
		yang_init_recvnack(recv->nack_receiver, recv->rtp_queue, 100 * 2 / 3);
	} else {
		recv->ssrc_ = context->videoTrack.ssrc_;
		yang_init_nackBuffer(recv->rtp_queue, 1000);
		yang_init_recvnack(recv->nack_receiver, recv->rtp_queue, 1000 * 2 / 3);
	}
	recv->last_sender_report_rtp_time = 0;
	recv->last_sender_report_rtp_time1 = 0;

	recv->last_sender_report_sys_time = 0;

	//recv->usingMixav = recv->conf->context->rtc.mixAvqueue;
	recv->last_seq = 0;
	memset(&recv->audioFrame, 0, sizeof(YangFrame));
}
void yang_destroy_recvTrack(YangRecvTrack *recv) {
	if (recv == NULL)
		return;

	yang_destroy_nackBuffer(recv->rtp_queue);
	yang_free(recv->rtp_queue);
	yang_destroy_recvnack(recv->nack_receiver);
	yang_free(recv->nack_receiver);
}

int32_t yang_recvtrack_has_ssrc(YangRtcContext *context, YangRecvTrack *recv,
		uint32_t ssrc) {
	//return recv->track_desc->has_ssrc(ssrc);
	if (recv->isAudio)
		return yang_has_ssrc(&context->audioTrack, ssrc) ? 1 : 0;
	else
		return yang_has_ssrc(&context->videoTrack, ssrc) ? 1 : 0;
}

void yang_recvtrack_update_rtt(YangRecvTrack *recv, int32_t rtt) {
	yang_recvnack_update_rtt(recv->nack_receiver, rtt);
	//recv->nack_receiver->update_rtt(rtt);
}

void yang_recvtrack_update_send_report_time(YangRecvTrack *recv, YangNtp *ntp,
		uint32_t rtp_time) {
	recv->last_sender_report_ntp1 = recv->last_sender_report_ntp;
	recv->last_sender_report_rtp_time1 = recv->last_sender_report_rtp_time;

	//recv->last_sender_report_ntp = ntp;
	memcpy(&recv->last_sender_report_ntp, ntp, sizeof(YangNtp));
	recv->last_sender_report_rtp_time = rtp_time;

	recv->last_sender_report_sys_time = yang_update_system_time();
}

int32_t yang_recvtrack_send_rtcp_rr(YangRtcContext *context,
		YangRecvTrack *recv) {
	int32_t err = Yang_Ok;

	uint32_t ssrc = recv->ssrc_;
	const uint64_t last_time = recv->last_sender_report_sys_time;
	//printf("last_%ld",recv->last_sender_report_sys_time);
	//if ((err = recv->session->send_rtcp_rr(ssrc, recv->rtp_queue, last_time,
	if ((err = yang_send_rtcp_rr(context, ssrc, recv->rtp_queue, last_time,
			&recv->last_sender_report_ntp)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u, last_time=%lld", ssrc, last_time);
	}

	return err;
}

int32_t yang_recvtrack_send_rtcp_xr_rrtr(YangRtcContext *context,
		YangRecvTrack *recv) {
	int32_t err = Yang_Ok;

	if ((err = yang_send_rtcp_xr_rrtr(context, recv->ssrc_)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u", recv->ssrc_);
	}

	return err;
}
//std::string yang_recvtrack_get_track_id() {
//return recv->track_desc->id_;
//}

int32_t yang_recvtrack_on_nack(YangRtcContext *context, YangRecvTrack *recv,
		YangRtpPacket *pkt) {
	int32_t err = Yang_Ok;

	uint16_t seq = pkt->header.sequence;
	if (seq == recv->last_seq) {
		//recv->rtp_queue->update(seq);
		yang_nackbuffer_update(recv->rtp_queue, seq);
		recv->last_seq = seq + 1;

	} else {

		YangRtpNackInfo *nack_info = yang_recvnack_find(recv->nack_receiver,
				seq);
		if (nack_info) {
			// seq had been received.
			yang_trace("\nreceive seq===%hu", seq);
			yang_recvnack_remove(recv->nack_receiver, seq);
			return err;
		}

		// insert check nack list
		uint16_t nack_first = 0, nack_last = 0;
		//recv->rtp_queue->update(seq, nack_first, nack_last);
		yang_nackbuffer_update2(recv->rtp_queue, seq, &nack_first, &nack_last);
		if (yang_rtp_seq_distance(nack_first, nack_last) > 0) {
			yang_trace("\nNACK: update seq=%u, nack range [%u, %u]", seq,
					nack_first, nack_last);
			yang_recvnack_insert(recv->nack_receiver, nack_first, nack_last);
			yang_recvnack_check_queue_size(recv->nack_receiver);
			//recv->nack_receiver->insert(nack_first, nack_last);
			//recv->nack_receiver->check_queue_size();

		}
	}
	// insert into video_queue and audio_queue
	//recv->rtp_queue->set(seq, pkt->header.sequence);
	yang_nackbuffer_set(recv->rtp_queue, seq, pkt->header.sequence);
	if (!yang_recvnack_isEmpty(recv->nack_receiver)) {
		uint32_t timeout_nacks = 0;
		yang_recvtrack_do_check_send_nacks(context, recv, &timeout_nacks);
	}

	return err;
}

int32_t yang_recvtrack_do_check_send_nacks(YangRtcContext *context,
		YangRecvTrack *recv, uint32_t *timeout_nacks) {
	uint32_t sent_nacks = 0;
	return yang_check_send_nacks(context, recv->nack_receiver, recv->ssrc_,
			&sent_nacks, timeout_nacks);

}
