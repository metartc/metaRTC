#include <yangwebrtc/YangRtcAudioRecvTrack.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangutil/sys/YangLog.h>


void yang_init_recvaudioTrack(YangRtcContext *context, YangRecvTrack *audiorecv,
		YangRtpBuffer *rtpBuffer) {

	yang_init_recvTrack(context, audiorecv, rtpBuffer, 1);

}
void yang_destroy_recvaudioTrack(YangRecvTrack *audiorecv) {
	yang_destroy_recvTrack(audiorecv);
}

int32_t yang_recvaudioTrack_on_rtp(YangRtcContext *context, YangRtpPacket *ppkt) {
	int32_t err = Yang_Ok;

	YangFrame audioFrame;
	if (ppkt->payload) {
		audioFrame.uid = context->streamConf->uid;
		audioFrame.payload = (uint8_t*) ppkt->payload;
		audioFrame.nb = ppkt->nb;
		audioFrame.pts = ppkt->header.timestamp;

		if (context && context->avcontext->recvcb.receiveAudio)
			context->avcontext->recvcb.receiveAudio(&audioFrame, context->user);
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

