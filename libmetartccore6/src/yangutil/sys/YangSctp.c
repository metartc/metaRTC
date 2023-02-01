//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangSctp.h"
#include <yangssl/YangRtcDtls.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

#if Yang_Enable_Datachannel

int32_t g_sctp_conn_output(void *addr, void *buf, size_t length, uint8_t tos, uint8_t set_df)
{
	YangSctp* sctp=(YangSctp*)addr;
	int32_t err=Yang_Ok;
	if(sctp->send_dtls_msg) sctp->send_dtls_msg(sctp->user,buf,length);

	return err;
}

int g_sctp_receive_cb(struct socket *sock, union sctp_sockstore addr, void *data,
          size_t datalen, struct sctp_rcvinfo rcv, int flags, void *ulp_info)
{
	YangSctp* sctp=(YangSctp*)ulp_info;
	uint16_t streamId = rcv.rcv_sid;
	uint32_t ppid     = yang_ntohl(rcv.rcv_ppid);
	uint16_t ssn      = rcv.rcv_ssn;
	int32_t eor = flags & MSG_EOR;
	(void)eor;
    //yang_trace("\nstreamId=%d,ssn=%d,ppid=%d,eor=%d,len=%d,data=%s",streamId,ssn,ppid,eor,datalen,ppid==YANG_DATA_CHANNEL_STRING?data:"[0/1]");

		if(ppid==YANG_DATA_CHANNEL_STRING||ppid==YANG_DATA_CHANNEL_BINARY){
			if(sctp->receive_msg) sctp->receive_msg(sctp->user,streamId, ssn, ppid, flags, (uint8_t*)data, datalen);
		}

	yang_free(data);
	return Yang_Ok;
}

// https://tools.ietf.org/html/draft-ietf-rtcweb-data-protocol-09#section-5.1
//      0                   1                   2                   3
//      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |  Message Type |  Channel Type |            Priority           |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |                    Reliability Parameter                      |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |         Label Length          |       Protocol Length         |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |                                                               |
//     |                             Label                             |
//     |                                                               |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//     |                                                               |
//     |                            Protocol                           |
//     |                                                               |
//     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void yang_sctp_sendData(YangSctp* sctp,YangFrame* msgFrame){

	yang_memset(&sctp->spa, 0, sizeof(struct sctp_sendv_spa));
	sctp->spa.sendv_flags |= SCTP_SEND_SNDINFO_VALID;
	sctp->spa.sendv_sndinfo.snd_sid = sctp->streamId;
	sctp->spa.sendv_sndinfo.snd_flags =SCTP_EOR;
	if(sctp->ordered){
		sctp->spa.sendv_prinfo.pr_policy = SCTP_PR_SCTP_NONE;
		sctp->spa.sendv_prinfo.pr_value  = 0;
	}else{
		sctp->spa.sendv_flags |= SCTP_SEND_PRINFO_VALID;
		sctp->spa.sendv_sndinfo.snd_flags |= SCTP_UNORDERED;

		if (sctp->maxPacketLifeTime != 0)
		{
			sctp->spa.sendv_prinfo.pr_policy = SCTP_PR_SCTP_TTL;

		}
		else if (sctp->maxRetransmits != 0)
		{
			sctp->spa.sendv_prinfo.pr_policy = SCTP_PR_SCTP_RTX;

		}
	}
	yang_put_be32((char*)&sctp->spa.sendv_sndinfo.snd_ppid,(uint32_t)msgFrame->mediaType);
	int32_t ret = usrsctp_sendv(sctp->local_sock, msgFrame->payload,msgFrame->nb, NULL, 0, &sctp->spa, sizeof(sctp->spa), SCTP_SENDV_SPA, 0);
	if(ret<0){
		yang_error("sctp send error(%d)",ret);
	}

}

void yang_sctp_receiveData(YangSctp* sctp,char* data,int32_t nb){
	usrsctp_conninput(sctp, data, nb, 0);
}


void yang_create_sctp(YangSctp* sctp){
	//rel
	sctp->ordered=1;
	sctp->maxRetransmits=-1;
	struct sockaddr_conn localConn, remoteConn;
	struct sctp_paddrparams peerAddrParams;
	int32_t ret=-1;
	usrsctp_init(0, g_sctp_conn_output, NULL);
	usrsctp_sysctl_set_sctp_ecn_enable(0);


	yang_memset(&localConn, 0, sizeof(struct sockaddr_conn));
	yang_memset(&remoteConn, 0, sizeof(struct sockaddr_conn));

	localConn.sconn_family = AF_CONN;
	localConn.sconn_port=yang_htons(YANG_SCTP_ASSOCIATION_DEFAULT_PORT);
	localConn.sconn_addr = sctp;

	remoteConn.sconn_family = AF_CONN;
	remoteConn.sconn_port=yang_htons(YANG_SCTP_ASSOCIATION_DEFAULT_PORT);
	remoteConn.sconn_addr = sctp;

	usrsctp_register_address(sctp);
	sctp->local_sock = usrsctp_socket(AF_CONN, SOCK_STREAM, IPPROTO_SCTP, g_sctp_receive_cb, NULL, 0, sctp);
	usrsctp_set_ulpinfo(sctp->local_sock,sctp);


	ret=usrsctp_set_non_blocking(sctp->local_sock, 1);
	if(ret<0) yang_error("usrsctp_set_non_blocking() failed: %s", strerror(errno));
	struct linger linger_opt;
	linger_opt.l_onoff = 1;
	linger_opt.l_linger = 0;
	ret=usrsctp_setsockopt(sctp->local_sock, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
	if(ret<0) yang_error("usrsctp_setsockopt(SO_LINGER) failed: %s", strerror(errno));
	// Set SCTP_ENABLE_STREAM_RESET.
	struct sctp_assoc_value av; // NOLINT(cppcoreguidelines-pro-type-member-init)

	av.assoc_value =
			SCTP_ENABLE_RESET_STREAM_REQ | SCTP_ENABLE_RESET_ASSOC_REQ | SCTP_ENABLE_CHANGE_ASSOC_REQ;

	ret=usrsctp_setsockopt(sctp->local_sock, IPPROTO_SCTP, SCTP_ENABLE_STREAM_RESET, &av, sizeof(av));
	if(ret<0) yang_error("usrsctp_setsockopt(SCTP_ENABLE_STREAM_RESET) failed: %s", strerror(errno));

	uint32_t on = 1;
	ret=usrsctp_setsockopt(sctp->local_sock, IPPROTO_SCTP, SCTP_NODELAY, &on, sizeof(on));
	if(ret<0) yang_error("usrsctp_setsockopt(SCTP_NODELAY) failed: %s", strerror(errno));

	struct sctp_event event;
	uint16_t event_types[] = {SCTP_ADAPTATION_INDICATION,
			SCTP_ASSOC_CHANGE,
			SCTP_ASSOC_RESET_EVENT,
			SCTP_REMOTE_ERROR,
			SCTP_SHUTDOWN_EVENT,
			SCTP_SEND_FAILED_EVENT,
			SCTP_STREAM_RESET_EVENT,
			SCTP_STREAM_CHANGE_EVENT};
	yang_memset(&event, 0, sizeof(event));
	event.se_assoc_id = SCTP_FUTURE_ASSOC;
	event.se_on = 1;
	for (uint32_t i = 0; i < sizeof(event_types) / sizeof(uint16_t); i++) {
		event.se_type = event_types[i];
		ret=usrsctp_setsockopt(sctp->local_sock, IPPROTO_SCTP, SCTP_EVENT, &event, sizeof(struct sctp_event));
		if(ret<0) yang_error("usrsctp_setsockopt(SCTP_EVENT) failed: %s", strerror(errno));
	}

	struct sctp_initmsg initmsg;
	yang_memset(&initmsg, 0, sizeof(struct sctp_initmsg));
	initmsg.sinit_num_ostreams = 300;
	initmsg.sinit_max_instreams = 300;
	ret=usrsctp_setsockopt(sctp->local_sock, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(struct sctp_initmsg));
	if(ret<0) yang_error("usrsctp_setsockopt(SCTP_INITMSG) failed: %s", strerror(errno));

	ret=usrsctp_bind(sctp->local_sock, (struct sockaddr*) &localConn, sizeof(localConn));
	if(ret<0) yang_error("usrsctp_bind() failed: %s", strerror(errno));

	int32_t status = 0;
	status = usrsctp_connect(sctp->local_sock, (struct sockaddr*) &remoteConn, sizeof(remoteConn));
	if(status >= 0 || errno == EINPROGRESS){

	}
	yang_memset(&peerAddrParams, 0, sizeof(struct sctp_paddrparams));
	yang_memcpy(&peerAddrParams.spp_address, &remoteConn, sizeof(remoteConn));
	peerAddrParams.spp_flags = SPP_PMTUD_DISABLE;
	peerAddrParams.spp_pathmtu = YANG_SCTP_MTU;
	ret=usrsctp_setsockopt(sctp->local_sock, IPPROTO_SCTP, SCTP_PEER_ADDR_PARAMS, &peerAddrParams, sizeof(peerAddrParams));
	if(ret<0) yang_error("usrsctp_setsockopt(SCTP_PEER_ADDR_PARAMS) failed: %s", strerror(errno));

}
void yang_destroy_sctp(YangSctp* sctp){

	usrsctp_deregister_address(sctp);
	usrsctp_set_ulpinfo(sctp->local_sock, NULL);
	usrsctp_shutdown(sctp->local_sock, SHUT_RDWR);
	usrsctp_close(sctp->local_sock);


	//while (usrsctp_finish() != 0) {
	//	yang_usleep(1000);
	//}
}
#endif
