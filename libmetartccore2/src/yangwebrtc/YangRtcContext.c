#include <yangstream/YangStreamType.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangwebrtc/YangCRtcDtls.h>
#include <yangwebrtc/YangCUdpHandle.h>
#include <yangwebrtc/YangRtcContext.h>
void yang_init_rtcContext(YangRtcContext *context) {
	if (context == NULL)
		return;
	context->codec = Yang_VED_264;

	context->audioSsrc = yang_generate_ssrc();
	context->videoSsrc = yang_generate_ssrc();
	context->avcontext = (YangAVContext*) calloc(sizeof(YangAVContext), 1);
	yang_trace("\naudiossrc===%u,videossrc==%u\n", context->audioSsrc,
			context->videoSsrc);
	context->state = 0;
	memset(&context->srtp, 0, sizeof(context->srtp));
	context->cer = (YangCertificate*) malloc(sizeof(YangCertificate));
	memset(context->cer, 0, sizeof(YangCertificate));
	yang_init_certificate(context->cer);

	context->udp = ( YangUdpHandle*) malloc(sizeof(YangUdpHandle));
	memset(context->udp, 0, sizeof(YangUdpHandle));

	context->dtls = ( YangRtcDtls*) malloc(sizeof( YangRtcDtls));
	memset(context->dtls, 0, sizeof(YangRtcDtls));

	context->dtls->udp = context->udp;
	context->dtls->srtp = &context->srtp;
	context->dtls->cer = context->cer;

}
void yang_destroy_rtcContext(YangRtcContext *context) {
	if (context == NULL)
		return;
	context->streamConf = NULL;

	yang_free(context->avcontext);


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
