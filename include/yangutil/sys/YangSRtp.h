
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGSRTP_H_
#define INCLUDE_YANGUTIL_SYS_YANGSRTP_H_
#include <yangutil/yangtype.h>
#include <stdint.h>
#include <yangutil/sys/YangThread.h>
#if Yang_Enable_Dtls
#include <srtp2/srtp.h>
typedef struct{
	 srtp_t recvCtx;
	 srtp_t sendCtx;
	 yang_thread_mutex_t rtpLock;
	 yang_thread_mutex_t rtcpLock;
}YangSRtp;
#ifdef __cplusplus
extern "C"{
#endif
    int32_t yang_create_srtp(YangSRtp* srtp,char* recv_key,int precvkeylen, char* send_key,int psendkeylen);
    int32_t yang_destroy_srtp(YangSRtp* srtp);
    int32_t yang_enc_rtp(YangSRtp* srtp,void* packet, int* nb_cipher);
    int32_t yang_enc_rtcp(YangSRtp* srtp,void* packet, int* nb_cipher);
    int32_t yang_dec_rtp(YangSRtp* srtp,void* packet, int* nb_plaintext);
    int32_t yang_dec_rtcp(YangSRtp* srtp,void* packet, int* nb_plaintext);
#ifdef __cplusplus
}
#endif
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGSRTP_H_ */
