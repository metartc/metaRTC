//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangwebrtc/YangVideoRecvTrack.h>
#include <yangwebrtc/YangH264RecvTrack.h>
#include <yangwebrtc/YangH265RecvTrack.h>
#include <yangwebrtc/YangRtcConnection.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangavutil/video/YangMeta.h>


void yang_init_recvvideoTrack(YangRtcContext *context,
		YangVideoRecvTrack *videorecv, YangRtpBuffer *rtpBuffer) {
	if (videorecv == NULL)
		return;

	yang_init_recvTrack(context, &videorecv->recv, rtpBuffer, 0);

	videorecv->h264Track = NULL;
	videorecv->h265Track = NULL;

	if (context->codec == Yang_VED_264) {
		videorecv->h264Track = (YangH264RecvTrack*) calloc(1,sizeof(YangH264RecvTrack));
		yang_init_h264recv(context, videorecv->h264Track, rtpBuffer);
	}
	if (context->codec == Yang_VED_265) {
		videorecv->h265Track = (YangH265RecvTrack*) calloc(1,
				sizeof(YangH265RecvTrack)); //new YangH265RecvTrack(conf, session, rtpBuffer);
		yang_init_h265recv(context, videorecv->h265Track, rtpBuffer);
	}

}
void yang_destroy_recvvideoTrack(YangVideoRecvTrack *videorecv) {
	if(videorecv==NULL) return ;
	yang_destroy_recvTrack(&videorecv->recv);
	yang_free(videorecv->h264Track);
	yang_free(videorecv->h265Track);
}

int32_t yang_recvvideoTrack_on_rtp(YangRtcContext *context,
		YangVideoRecvTrack *videorecv, YangRtpPacket *src) {
	if(context==NULL||videorecv==NULL) return 1;
	int32_t err = Yang_Ok;
	if (videorecv->h264Track)
		yang_h264recv_on_rtp(context, videorecv->h264Track, src);
	if (videorecv->h265Track)
		yang_h265recv_on_rtp(context, videorecv->h265Track, src);
	return err;
}

int32_t yang_recvvideoTrack_check_send_nacks(YangRtcContext *context,
		YangVideoRecvTrack *videorecv) {
	int32_t err = Yang_Ok;

	uint32_t timeout_nacks = 0;

	if ((err = yang_recvtrack_do_check_send_nacks(context, &videorecv->recv,
			&timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "video");
	}

	if (timeout_nacks == 0) {
		return err;
	}

	return err;
}



