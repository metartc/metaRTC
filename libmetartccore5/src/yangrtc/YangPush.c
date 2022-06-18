//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangrtc/YangPush.h>
#include <yangrtc/YangUdpHandle.h>
#include <yangutil/sys/YangLog.h>
int32_t yang_send_avpacket(YangRtcSession *session, YangRtpPacket *pkt,	YangBuffer *pbuf) {
	int32_t err = Yang_Ok;
	int32_t nn_encrypt = yang_buffer_pos(pbuf);

	if (session->push)	session->push->cache_nack(session->push->pubStream, pkt, pbuf->data,	nn_encrypt);
#if Yang_HaveDtls
	if ((err = yang_enc_rtp(&session->context.srtp, pbuf->data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}
#endif
	return yang_rtc_sendData(session->context.udp, pbuf->data, nn_encrypt);
}

int32_t yang_send_nackpacket(YangRtcContext *context, char *data, int32_t nb) {
	int32_t err = Yang_Ok;
	int32_t nn_encrypt = nb;
#if Yang_HaveDtls
	if ((err = yang_enc_rtp(&context->srtp, data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}
#endif
	return yang_rtc_sendData(context->udp, data, nn_encrypt);
}
