//
// Copyright (c) 2019-2026 yanggaofeng
//

#include <yangice/YangRtcSocket.h>
#include <yangrtc/YangPush.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

void yang_push_audioPacket(YangRtcContext *context, uint32_t audioSSrc,uint8_t* data){
	uint8_t marked=*(data+1);;
	uint8_t payloadtype=context->audioPayloadType;

	*(data+1)=payloadtype|marked;
	yang_put_be32((char*)data+8,audioSSrc);
}

void yang_push_videoPacket(YangRtcContext *context, uint32_t videoSSrc,uint8_t* data,uint32_t length){
	uint8_t marked=*(data+1);;
	uint8_t payloadtype=context->h264PayloadType;
#if Yang_Enable_TWCC
	uint16_t twccSeq;
#endif
	if (context->videoEncodeCodec == Yang_VED_H265)
		payloadtype=context->h265PayloadType;

	*(data+1)=payloadtype|marked;
	yang_put_be32((char*)data+8,videoSSrc);

#if Yang_Enable_TWCC
	if(context->twcc){
		twccSeq=context->twccSeq++;//yang_get_be16(tmp+17);
		yang_put_be16((char*)data+17,twccSeq);
		yang_insert_twcc_local(context->twcc,twccSeq,length);
	}
#endif
}

int32_t yang_send_avpacket(YangRtcSession *session, YangRtpPacket *pkt,	YangBuffer *pbuf) {

	int32_t err = Yang_Ok;
	int32_t nn_encrypt = yang_buffer_pos(pbuf);

#if Yang_Enable_Dtls
	if ((err = yang_enc_rtp(&session->context.srtp, pbuf->data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}
#endif

	return session->context.sock->write(&session->context.sock->session, pbuf->data, nn_encrypt);
}

int32_t yang_send_avpacket2(YangRtcContext *context, uint8_t* data,uint32_t length) {

	int32_t err = Yang_Ok;

	int32_t nn_encrypt = length;

	//if (session->push)	session->push->cache_nack(session->push->pubStream, pkt, pbuf->data,nn_encrypt);
#if Yang_Enable_Dtls
	if ((err = yang_enc_rtp(&context->srtp, data, &nn_encrypt)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp protect");
	}
#endif

	return context->sock->write(&context->sock->session,(char*) data, nn_encrypt);
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
