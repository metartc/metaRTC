//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPlayTrackAudio.h>
#include <yangrtc/YangPlayTrack.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangutil/sys/YangLog.h>



int32_t yang_playtrackAudio_on_rtp(YangRtcContext *context, YangRtpPacket *ppkt) {
	int32_t err = Yang_Ok;

	YangFrame audioFrame;
	if (ppkt->payload) {
		audioFrame.uid = context->streamConfig->uid;
		audioFrame.payload = (uint8_t*) ppkt->payload;
		audioFrame.nb = ppkt->nb;
		audioFrame.pts = ppkt->header.timestamp;

		if (context && context->streamConfig->recvCallback.receiveAudio)
            context->streamConfig->recvCallback.receiveAudio(context->streamConfig->recvCallback.context,&audioFrame);
	}

	return err;
}

int32_t yang_playtrackAudio_check_send_nacks(YangRtcContext *context,
		YangPlayTrack *audiorecv) {
	int32_t err = Yang_Ok;
	uint32_t timeout_nacks = 0;
	if ((err = yang_playtrack_do_check_send_nacks(context, audiorecv,
			&timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "audio");
	}

	return err;
}


void yang_create_playTrackAudio(YangRtcContext *context, YangPlayTrackAudio *track) {
	yang_create_playTrack(context, &track->session,1);
	track->on_rtp=yang_playtrackAudio_on_rtp;
	track->check_nacks=yang_playtrackAudio_check_send_nacks;
	track->has_ssrc=yang_playtrack_has_ssrc;
	track->update_rtt=yang_playtrack_update_rtt;
	track->update_send_report_time=yang_playtrack_update_send_report_time;
	track->send_rtcp_rr=yang_playtrack_send_rtcp_rr;
	track->send_rtcp_xr_rrtr=yang_playtrack_send_rtcp_xr_rrtr;
	track->on_nack=yang_playtrack_on_nack;
}
void yang_destroy_playTrackAudio(YangPlayTrackAudio *track) {
	yang_destroy_playTrack(&track->session);
}

