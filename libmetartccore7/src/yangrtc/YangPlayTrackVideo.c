//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPlayTrackVideo.h>
#include <yangrtc/YangPlayTrackH265.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpSTAPPayload.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangavutil/video/YangMeta.h>

#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangPlayTrack.h>
#include <yangrtc/YangPlayTrackH264.h>

#if Yang_Enable_RTC_Video

int32_t yang_playvideoTrack_on_rtp(YangRtcContext *context,
		YangPlayTrackSession *videotrack, YangRtpPacket *src) {
	if(context==NULL||videotrack==NULL) return 1;

	int32_t err = Yang_Ok;
	if (videotrack->h264Track)
		yang_playtrackH264_on_rtp(context, (YangPlayTrackH264*)videotrack->h264Track, src);
	if (videotrack->h265Track)
		yang_playtrackH265_on_rtp(context, (YangPlayTrackH265*)videotrack->h265Track, src);
	return err;
}

int32_t yang_playvideoTrack_check_send_nacks(YangRtcContext *context,
		YangPlayTrackSession *videotrack) {
	int32_t err = Yang_Ok;

	uint32_t timeout_nacks = 0;

	if ((err = yang_playtrack_do_check_send_nacks(context, &videotrack->track,
			&timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "video");
	}

	if (timeout_nacks == 0) {
		return err;
	}

	return err;
}
void yang_playvideoTrack_setRequestKeyframeState(YangPlayTrackSession* track,int32_t state){
	if (track->h264Track)		((YangPlayTrackH264*)track->h264Track)->hasRequestKeyframe = state;
	if (track->h265Track)       ((YangPlayTrackH265*)track->h265Track)->hasRequestKeyframe = state;

}

int32_t yang_playvideoTrack_send_rtcp_rr(YangRtcContext *context,YangPlayTrack *track){
	return yang_playtrack_send_rtcp_rr(context,yangfalse,track);
}

void yang_create_playvideoTrack(YangRtcContext *context,YangPlayTrackVideo *track) {
	if (track == NULL)		return;
	YangPlayTrackSession *videotrack=&track->session;
	yang_create_playTrack(context, &videotrack->track,yangfalse);

	videotrack->h264Track = NULL;
	videotrack->h265Track = NULL;

	if (context->videoCodec == Yang_VED_H264) {
		YangPlayTrackH264* h264Track=(YangPlayTrackH264*) yang_calloc(1,sizeof(YangPlayTrackH264));
		videotrack->h264Track = h264Track;
		yang_create_playTrackH264(context, h264Track);
	}
	if (context->videoCodec == Yang_VED_H265) {
		YangPlayTrackH265* h265Track= (YangPlayTrackH265*) yang_calloc(1,
				sizeof(YangPlayTrackH265));
		videotrack->h265Track =h265Track;
		yang_create_playTrackH265(context, h265Track);
	}

	track->on_rtp=yang_playvideoTrack_on_rtp;
	track->check_nacks=yang_playvideoTrack_check_send_nacks;
	track->setRequestKeyframeState=yang_playvideoTrack_setRequestKeyframeState;
	track->has_ssrc=yang_playtrack_has_ssrc;
	track->update_rtt=yang_playtrack_update_rtt;
	track->update_send_report_time=yang_playtrack_update_send_report_time;
	track->send_rtcp_rr=yang_playvideoTrack_send_rtcp_rr;
	track->send_rtcp_xr_rrtr=yang_playtrack_send_rtcp_xr_rrtr;
	track->on_nack=yang_playtrack_on_nack;

}

void yang_destroy_playvideoTrack(YangPlayTrackVideo *track) {
	if(track==NULL) return ;
	YangPlayTrackSession *videotrack=&track->session;
	yang_destroy_playTrack(&videotrack->track);
	yang_free(videotrack->h264Track);
	yang_free(videotrack->h265Track);
}
#endif

