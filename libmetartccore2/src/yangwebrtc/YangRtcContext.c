#include <yangwebrtc/YangRtcContext.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>

void yang_init_rtcContext(struct YangRtcContext *context) {
	if (context == NULL)
		return;
	context->codec = Yang_VED_264;

	context->audioSsrc = yang_generate_ssrc();
	context->videoSsrc = yang_generate_ssrc();
	context->context = (struct YangAVContex*) calloc(
			sizeof(struct YangAVContex), 1);
	yang_trace("\naudiossrc===%u,videossrc==%u..........", context->audioSsrc,
			context->videoSsrc);
	context->state = 0;
	memset(&context->srtp, 0, sizeof(context->srtp));
	context->cer = (struct YangCertificate*) malloc(
			sizeof(struct YangCertificate));
	memset(context->cer, 0, sizeof(struct YangCertificate));
	yang_init_certificate(context->cer);

	context->udp = (struct YangUdpHandle*) malloc(sizeof(struct YangUdpHandle));
	memset(context->udp, 0, sizeof(YangUdpHandle));

	context->dtls = (struct YangRtcDtls*) malloc(sizeof(struct YangRtcDtls));
	memset(context->dtls, 0, sizeof(YangRtcDtls));

	context->dtls->udp = context->udp;
	context->dtls->srtp = &context->srtp;
	context->dtls->cer = context->cer;

}
void yang_destroy_rtcContext(YangRtcContext *context) {
	if (context == NULL)
		return;
	context->streamConf = NULL;

	yang_free(context->context);
	yang_free(context->remote_audio);
	yang_free(context->remote_video);

	yang_destroy_certificate(context->cer);

	yang_free(context->stun.data);
	if (context->udp)
		yang_stop_rtcudp(context->udp);
	yang_destroy_rtcudp(context->udp);
	yang_free(context->udp);

	yang_destroy_rtcdtls(context->dtls);
	yang_free(context->dtls);


}
bool yang_has_ssrc(YangRtcTrack *track, uint32_t ssrc) {
	if (ssrc == track->ssrc_ || ssrc == track->rtx_ssrc_
			|| ssrc == track->fec_ssrc_) {
		return true;
	}

	return false;
}
