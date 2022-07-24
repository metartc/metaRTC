//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangRtcAudioRecvTrack.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangutil/sys/YangLog.h>


void yang_create_recvaudioTrack(YangRtcContext *context, YangRecvTrack *audiorecv) {

	yang_create_recvTrack(context, audiorecv,1);

}
void yang_destroy_recvaudioTrack(YangRecvTrack *audiorecv) {
	yang_destroy_recvTrack(audiorecv);
}

int32_t yang_recvaudioTrack_on_rtp(YangRtcContext *context, YangRtpPacket *ppkt) {
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

int32_t yang_recvaudioTrack_check_send_nacks(YangRtcContext *context,
		YangRecvTrack *audiorecv) {
	int32_t err = Yang_Ok;
	uint32_t timeout_nacks = 0;
	if ((err = yang_recvtrack_do_check_send_nacks(context, audiorecv,
			&timeout_nacks)) != Yang_Ok) {
		return yang_error_wrap(err, "audio");
	}

	return err;
}

