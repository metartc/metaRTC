//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangice/YangRtcSocket.h>
#include <yangrtc/YangPush.h>
#include <yangutil/sys/YangLog.h>

int32_t yang_send_avpacket(YangRtcSession *session, YangRtpPacket *pkt,	YangBuffer *pbuf) {

	int32_t err = Yang_Ok;
	int32_t nn_encrypt = yang_buffer_pos(pbuf);

	if (session->push)	session->push->cache_nack(session->push->pubStream, pkt, pbuf->data,nn_encrypt);
#if Yang_Enable_Dtls
	if ((err = yang_enc_rtp(&session->context.srtp, pbuf->data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}
#endif

	return session->context.sock->write(&session->context.sock->session, pbuf->data, nn_encrypt);
}

int32_t yang_send_nackpacket(YangRtcContext *context, char *data, int32_t nb) {
	int32_t err = Yang_Ok;
	int32_t nn_encrypt = nb;
#if Yang_Enable_Dtls
	if ((err = yang_enc_rtp(&context->srtp, data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}
#endif
	return context->sock->write(&context->sock->session, data, nn_encrypt);
}
