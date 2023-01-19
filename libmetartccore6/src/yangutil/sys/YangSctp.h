//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGSCTP_H_
#define YANGSCTP_H_

#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>

#if Yang_Enable_Datachannel
#include <usrsctp.h>

#define YANG_SCTP_MTU                         1188
#define YANG_SCTP_ASSOCIATION_DEFAULT_PORT    5000
typedef struct{
	uint16_t streamId;
	uint16_t maxPacketLifeTime;
	uint16_t maxRetransmits;
	uint16_t ordered;
	struct sctp_sendv_spa spa;
	struct socket *local_sock;
	void* user;
	void (*send_dtls_msg)(void* user,char* data,int32_t nb);
	void (*receive_msg)(void* user,uint16_t streamId, uint16_t ssn, uint32_t ppid, int flags, uint8_t* data, size_t len);
}YangSctp;

void yang_create_sctp(YangSctp* sctp);
void yang_destroy_sctp(YangSctp* sctp);
void yang_sctp_sendData(YangSctp* sctp,YangFrame* msgFrame);
void yang_sctp_receiveData(YangSctp* sctp,char* data,int32_t nb);
#endif
#endif /* YANGSCTP_H_ */
