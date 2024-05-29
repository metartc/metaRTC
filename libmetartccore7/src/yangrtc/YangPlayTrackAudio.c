//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPlayTrackAudio.h>
#include <yangrtc/YangPlayTrack.h>

#include <yangutil/sys/YangLog.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpSTAPPayload.h>



#if Yang_Enable_RTC_Audio

int32_t yang_playtrackAudio_on_rtp(YangRtcContext *context, YangPlayTrack* track,YangRtpPacket *ppkt) {
	int32_t err = Yang_Ok;

	if (ppkt->payload) {
		track->audioFrame.uid = context->streamConfig->uid;
		track->audioFrame.payload = (uint8_t*) ppkt->payload;
		track->audioFrame.nb = ppkt->nb;
		track->audioFrame.pts = ppkt->header.timestamp;

		if(track&&track->sortBuffer){
			track->sortBuffer->putFrame(&track->sortBuffer->session,track->audioFrame.pts,&track->audioFrame);
			YangFrame* frame=track->sortBuffer->getCurFrameRef(&track->sortBuffer->session);
			if (frame&&context){
				if(context->streamConfig->recvCallback.receiveAudio)
					context->streamConfig->recvCallback.receiveAudio(context->streamConfig->recvCallback.context,frame);
				track->sortBuffer->removeFirst(&track->sortBuffer->session);
			}

		}else{
			if (context && context->streamConfig->recvCallback.receiveAudio)
				context->streamConfig->recvCallback.receiveAudio(context->streamConfig->recvCallback.context,&track->audioFrame);
		}
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

int32_t yang_playtrackAudio_send_rtcp_rr(YangRtcContext *context,YangPlayTrack *track){
	return yang_playtrack_send_rtcp_rr(context,yangtrue,track);
}

void yang_create_playTrackAudio(YangRtcContext *context, YangPlayTrackAudio *track) {
	yang_create_playTrack(context, &track->session,yangtrue);

	if(track->session.enableAudioBuffer&&track->session.sortBuffer==NULL){
		track->session.sortBuffer=(YangSortBuffer*)yang_calloc(sizeof(YangSortBuffer),1);
		yang_create_sortBuffer(track->session.sortBuffer,3);
		track->session.sortBuffer->initFrames(&track->session.sortBuffer->session,8,1500);
	}

	track->on_rtp=yang_playtrackAudio_on_rtp;
	track->check_nacks=yang_playtrackAudio_check_send_nacks;
	track->has_ssrc=yang_playtrack_has_ssrc;
	track->update_rtt=yang_playtrack_update_rtt;
	track->update_send_report_time=yang_playtrack_update_send_report_time;
	track->send_rtcp_rr=yang_playtrackAudio_send_rtcp_rr;
	track->send_rtcp_xr_rrtr=yang_playtrack_send_rtcp_xr_rrtr;
	track->on_nack=yang_playtrack_on_nack;
}
void yang_destroy_playTrackAudio(YangPlayTrackAudio *track) {
	yang_destroy_sortBuffer(track->session.sortBuffer);
	yang_free(track->session.sortBuffer);
	yang_destroy_playTrack(&track->session);
}
#endif
