//
// Copyright (c) 2019-2025 yanggaofeng
//
#include <yangrtc/YangPullTrackVideo.h>
#include <yangrtc/YangPullTrackH265.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpSTAPPayload.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangavutil/video/YangMeta.h>

#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangPullTrack.h>
#include <yangrtc/YangPullTrackH264.h>

#if Yang_Enable_RTC_Video

int32_t yang_playvideoTrack_on_rtp(YangRtcContext *context,
		YangPullTrackSession *videotrack, YangRtpPacket *src) {
	int32_t err = Yang_Ok;

	if(context==NULL||videotrack==NULL)
		return 1;

	if (videotrack->h264Track)
		yang_pulltrackH264_on_rtp(context, (YangPullTrackH264*)videotrack->h264Track, src);

	if (videotrack->h265Track)
		yang_pulltrackH265_on_rtp(context, (YangPullTrackH265*)videotrack->h265Track, src);

	return err;
}

int32_t yang_playvideoTrack_check_send_nacks(YangRtcContext *context,
		YangPullTrackSession *videotrack) {
	int32_t err = Yang_Ok;

	uint32_t timeout_nacks = 0;

	if ((err = yang_pulltrack_do_check_send_nacks(context, &videotrack->track,
			&timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "video");
	}

	if (timeout_nacks == 0) {
		return err;
	}

	return err;
}
void yang_playvideoTrack_setRequestKeyframeState(YangPullTrackSession* track,int32_t state){

	if (track->h264Track)
		((YangPullTrackH264*)track->h264Track)->hasRequestKeyframe = state;

	if (track->h265Track)
		((YangPullTrackH265*)track->h265Track)->hasRequestKeyframe = state;

}

int32_t yang_playvideoTrack_send_rtcp_rr(YangRtcContext *context,YangPullTrack *track){
	return yang_pulltrack_send_rtcp_rr(context,yangfalse,track);
}

void yang_create_pullVideoTrack(YangRtcContext *context,YangPullTrackVideo *track) {
	YangPullTrackSession *videotrack;
	YangPullTrackH264* h264Track;
	YangPullTrackH265* h265Track;
	if (track == NULL)
		return;

	videotrack=&track->session;
	yang_create_pullTrack(context, &videotrack->track,yangfalse);

	videotrack->h264Track = NULL;
	videotrack->h265Track = NULL;

	if (context->videoCodec == Yang_VED_H264) {
		h264Track=(YangPullTrackH264*) yang_calloc(1,sizeof(YangPullTrackH264));
		videotrack->h264Track = h264Track;
		yang_create_pullTrackH264(context, h264Track);
	}

	if (context->videoCodec == Yang_VED_H265) {
		h265Track= (YangPullTrackH265*) yang_calloc(1,
				sizeof(YangPullTrackH265));
		videotrack->h265Track =h265Track;
		yang_create_pullTrackH265(context, h265Track);
	}

	track->on_rtp=yang_playvideoTrack_on_rtp;
	track->check_nacks=yang_playvideoTrack_check_send_nacks;
	track->setRequestKeyframeState=yang_playvideoTrack_setRequestKeyframeState;
	track->has_ssrc=yang_pulltrack_has_ssrc;
	track->update_rtt=yang_pulltrack_update_rtt;
	track->update_send_report_time=yang_pulltrack_update_send_report_time;
	track->send_rtcp_rr=yang_playvideoTrack_send_rtcp_rr;
	track->send_rtcp_xr_rrtr=yang_pulltrack_send_rtcp_xr_rrtr;
	track->on_nack=yang_pulltrack_on_nack;

}

void yang_destroy_playVideoTrack(YangPullTrackVideo *track) {
	YangPullTrackSession *videotrack;

	if(track==NULL)
		return ;

	videotrack=&track->session;
	yang_destroy_pullTrack(&videotrack->track);
	yang_free(videotrack->h264Track);
	yang_free(videotrack->h265Track);
}
#endif

