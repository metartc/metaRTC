//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangPullTrackH.h>
#include <yangrtc/YangPullTrack.h>

#include <yangutil/sys/YangLog.h>
#include <yangvideo/YangNalu.h>

#include <math.h>

int32_t yang_pulltrack_has_ssrc(YangRtcContext *context, YangPullTrack *track,
		uint32_t ssrc) {
	int32_t i;
	if (track->isAudio)
		return yang_has_ssrc(&context->audioTrack, ssrc) ? 1 : 0;
	else{
		for(i=0;i<context->videoTracks.vsize;i++){
			if( yang_has_ssrc(&context->videoTracks.payload[i], ssrc)) return 1;
		}
	}
	return 0;
}

void yang_pulltrack_update_rtt(YangPullTrack *track, int32_t rtt) {
	yang_recvnack_update_rtt(track->nack_receiver, rtt);

}

void yang_pulltrack_update_send_report_time(YangPullTrack *track, YangNtp *ntp,
		uint32_t rtp_time) {

	yang_memcpy(&track->last_sender_report_ntp, ntp, sizeof(YangNtp));
	track->last_sender_report_rtp_time = rtp_time;

	track->last_sender_report_sys_time = yang_update_system_time();
}

int32_t yang_pulltrack_send_rtcp_rr(YangRtcContext *context,yangbool isAudio,YangPullTrack *track) {
	int32_t err = Yang_Ok;

	uint32_t ssrc = track->ssrc;
	const uint64_t last_time = track->last_sender_report_sys_time;


	if ((err = yang_send_rtcp_rr(context, isAudio,ssrc, track->rtp_queue, last_time,
			&track->last_sender_report_ntp)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u, last_time=%lld", ssrc, last_time);
	}

	return err;
}

int32_t yang_pulltrack_send_rtcp_xr_rrtr(YangRtcContext *context,
		YangPullTrack *track) {
	int32_t err = Yang_Ok;

	if ((err = yang_send_rtcp_xr_rrtr(context, track->ssrc)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u", track->ssrc);
	}

	return err;
}


int32_t yang_pulltrack_on_nack(YangRtcContext *context, YangPullTrack *track,YangRtpPacket *pkt) {

	uint16_t seq = pkt->header.sequence;
	// insert check nack
	uint16_t nack_first, nack_last;
	uint32_t timeout_nacks;
	int32_t err = Yang_Ok;

	if (track->nack_receiver->queue.vsize > 0) {
		YangRtpNackInfo *nack_info = yang_recvnack_find(track->nack_receiver,seq);
		if (nack_info) {
			//receive lost seq
			yang_trace("\nreceive nack seq=%hu", seq);
			yang_recvnack_remove(track->nack_receiver, seq);
			return err;
		}
	}
	nack_first = 0;
	nack_last = 0;

	yang_nackbuffer_update2(track->rtp_queue, seq, &nack_first, &nack_last);
	if (yang_rtp_seq_distance(nack_first, nack_last) > 0) {
		yang_trace("\nnack check: lost current seq=%u, nack range [%u, %u]", seq,nack_first, nack_last);
		yang_recvnack_insert(track->nack_receiver, nack_first, nack_last);
		yang_recvnack_check_queue_size(track->nack_receiver);

	}

//insert nack
	yang_nackbuffer_set(track->rtp_queue, seq, pkt->header.sequence);
	if (!yang_recvnack_isEmpty(track->nack_receiver)) {
		timeout_nacks = 0;
		yang_pulltrack_do_check_send_nacks(context, track, &timeout_nacks);
	}

	return err;
}

int32_t yang_pulltrack_do_check_send_nacks(YangRtcContext *context,YangPullTrack *track, uint32_t *timeout_nacks) {
	uint32_t sent_nacks = 0;
	return yang_check_send_nacks(context, track->nack_receiver, track->ssrc,&sent_nacks, timeout_nacks);

}

void yang_create_pullTrack(YangRtcContext *context, YangPullTrack *track, yangbool is_audio) {

	if (track == NULL)
		return;

	track->uid = context->peerInfo->uid;
	track->isAudio = is_audio;
	track->rtp_queue = (YangReceiveNackBuffer*) yang_calloc(1,sizeof(YangReceiveNackBuffer));
	track->nack_receiver = (YangRtpRecvNack*) yang_calloc(1,sizeof(YangRtpRecvNack));
	if (is_audio) {
		track->ssrc = context->audioTrack.ssrc;
		yang_create_nackBuffer(track->rtp_queue, 100);
		yang_create_recvnack(track->nack_receiver, track->rtp_queue, 100 * 2 / 3);
	} else {
        if(context->videoTracks.vsize>0) track->ssrc=context->videoTracks.payload[0].ssrc;
		yang_create_nackBuffer(track->rtp_queue, 1000);
		yang_create_recvnack(track->nack_receiver, track->rtp_queue, 1000 * 2 / 3);
	}
	track->last_sender_report_rtp_time = 0;
	track->last_sender_report_sys_time = 0;

	track->last_seq = 0;
	track->enableAudioBuffer=yangtrue;
	track->sortBuffer=NULL;
	yang_memset(&track->audioFrame, 0, sizeof(YangFrame));
}

void yang_destroy_pullTrack(YangPullTrack *track) {

	if (track == NULL)
		return;

	yang_destroy_nackBuffer(track->rtp_queue);
	yang_free(track->rtp_queue);
	yang_destroy_recvnack(track->nack_receiver);
	yang_free(track->nack_receiver);
}




