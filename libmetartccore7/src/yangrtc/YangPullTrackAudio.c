//
// Copyright (c) 2019-2025 yanggaofeng
//
#include <yangrtc/YangPullTrackAudio.h>
#include <yangrtc/YangPullTrack.h>

#include <yangutil/sys/YangLog.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpSTAPPayload.h>



#if Yang_Enable_RTC_Audio

static int32_t yang_on_rtp(YangRtcContext *context, YangPullTrack* track,YangRtpPacket *ppkt) {
	int32_t err = Yang_Ok;
	YangFrame* frame;
	if (ppkt->payload) {
		track->audioFrame.uid = context->peerInfo->uid;
		track->audioFrame.payload = (uint8_t*) ppkt->payload;
		track->audioFrame.nb = ppkt->nb;
		track->audioFrame.pts = ppkt->header.timestamp;

		if(track&&track->sortBuffer){
			track->sortBuffer->putFrame(&track->sortBuffer->session,track->audioFrame.pts,&track->audioFrame);
			frame=track->sortBuffer->getCurFrameRef(&track->sortBuffer->session);
			if (frame&&context){
				if(context->peerCallback->recvCallback.receiveAudio)
					context->peerCallback->recvCallback.receiveAudio(context->peerCallback->recvCallback.context,frame);
				track->sortBuffer->removeFirst(&track->sortBuffer->session);
			}

		}else{
			if (context && context->peerCallback->recvCallback.receiveAudio)
				context->peerCallback->recvCallback.receiveAudio(context->peerCallback->recvCallback.context,&track->audioFrame);
		}
	}

	return err;
}

static int32_t yang_playtrackAudio_check_send_nacks(YangRtcContext *context,
		YangPullTrack *audiorecv) {
	int32_t err = Yang_Ok;
	uint32_t timeout_nacks = 0;
	if ((err = yang_pulltrack_do_check_send_nacks(context, audiorecv,
			&timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "audio");
	}

	return err;
}

static int32_t yang_playtrackAudio_send_rtcp_rr(YangRtcContext *context,YangPullTrack *track){
	return yang_pulltrack_send_rtcp_rr(context,yangtrue,track);
}

void yang_create_pullTrackAudio(YangRtcContext *context, YangPullTrackAudio *track) {
	yang_create_pullTrack(context, &track->session,yangtrue);

	if(track->session.enableAudioBuffer&&track->session.sortBuffer==NULL){
		track->session.sortBuffer=(YangSortBuffer*)yang_calloc(sizeof(YangSortBuffer),1);
		yang_create_sortBuffer(track->session.sortBuffer,3);
		track->session.sortBuffer->initFrames(&track->session.sortBuffer->session,8,1500);
	}

	track->on_rtp=yang_on_rtp;
	track->check_nacks=yang_playtrackAudio_check_send_nacks;
	track->has_ssrc=yang_pulltrack_has_ssrc;
	track->update_rtt=yang_pulltrack_update_rtt;
	track->update_send_report_time=yang_pulltrack_update_send_report_time;
	track->send_rtcp_rr=yang_playtrackAudio_send_rtcp_rr;
	track->send_rtcp_xr_rrtr=yang_pulltrack_send_rtcp_xr_rrtr;
	track->on_nack=yang_pulltrack_on_nack;
}
void yang_destroy_pullTrackAudio(YangPullTrackAudio *track) {
	yang_destroy_sortBuffer(track->session.sortBuffer);
	yang_free(track->session.sortBuffer);
	yang_destroy_pullTrack(&track->session);
}
#endif
