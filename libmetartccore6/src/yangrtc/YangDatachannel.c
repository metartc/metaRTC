//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangDatachannel.h>

#include <yangssl/YangRtcDtls.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSctp.h>

#if Yang_Enable_Datachannel
typedef struct{
	YangSctp sctp;
	YangRtcDtls* dtls;
}YangDatachannelContext;

#define Yang_Sctp_BufferLen 8192

void yang_datachannel_on_message(void *pcontext,char* data,int32_t nb){
	YangDatachannelContext* context=(YangDatachannelContext*)pcontext;
	yang_sctp_receiveData(&context->sctp,data,nb);
}

void yang_datachannel_send_message(void *pcontext,YangFrame* msgFrame){
	YangDatachannelContext* context=(YangDatachannelContext*)pcontext;
	yang_sctp_sendData(&context->sctp,msgFrame);
}


void yang_datachannel_receive_msg(void* user,uint16_t streamId, uint16_t ssn, uint32_t ppid, int flags,  uint8_t* data, size_t len){
	YangRtcContext* context=(YangRtcContext*)user;

	YangFrame frame;
	yang_memset(&frame,0,sizeof(YangFrame));
	frame.payload=data;
	frame.nb=len;
	frame.uid=context->streamConfig->uid;
    context->streamConfig->recvCallback.receiveMsg(context->streamConfig->recvCallback.context,&frame);
}

void yang_datachannelsend_dtls_msg(void* user,char* data,int32_t nb){
	if(user==NULL||data==NULL) return;

	YangRtcContext* context=(YangRtcContext*)user;
	if(context->dtls->session.state!=YangDtlsStateClientDone||context->dtls->session.isRecvAlert||context->state!=Yang_Conn_State_Connected)	return;

	context->dtls->sendSctpData(&context->dtls->session,(uint8_t*)data,nb);
}

void yang_create_datachannel(YangDatachannel *datachannel,YangRtcContext* ctx){
	if(datachannel==NULL) return;
	YangDatachannelContext* context=(YangDatachannelContext*)yang_calloc(sizeof(YangDatachannelContext),1);
	datachannel->context=context;
	context->dtls=ctx->dtls;
	yang_create_sctp(&context->sctp);
	context->sctp.user=ctx;
	context->sctp.send_dtls_msg=yang_datachannelsend_dtls_msg;
	context->sctp.receive_msg=yang_datachannel_receive_msg;


	datachannel->on_message=yang_datachannel_on_message;
	datachannel->send_message=yang_datachannel_send_message;
}

void yang_destroy_datachannel(YangDatachannel *datachannel){
	if(datachannel==NULL||datachannel->context==NULL) return;
	YangDatachannelContext* context=(YangDatachannelContext*)datachannel->context;
	yang_destroy_sctp(&context->sctp);
}
#endif
