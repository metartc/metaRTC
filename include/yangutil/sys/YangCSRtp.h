/*
 * YangCSrtp.h
 *
 *  Created on: 2021年12月27日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_SYS_YANGCSRTP_H_
#define INCLUDE_YANGUTIL_SYS_YANGCSRTP_H_
#include <stdint.h>
#include <srtp2/srtp.h>
typedef struct{
	 srtp_t recvCtx;
	 srtp_t sendCtx;
}YangSRtp;
    int32_t yang_init_srtp(YangSRtp* srtp,char* recv_key,int precvkeylen, char* send_key,int psendkeylen);
    int32_t yang_destroy_srtp(YangSRtp* srtp);
    int32_t yang_enc_rtp(YangSRtp* srtp,void* packet, int* nb_cipher);
    int32_t yang_enc_rtcp(YangSRtp* srtp,void* packet, int* nb_cipher);
    int32_t yang_dec_rtp(YangSRtp* srtp,void* packet, int* nb_plaintext);
    int32_t yang_dec_rtcp(YangSRtp* srtp,void* packet, int* nb_plaintext);



#endif /* INCLUDE_YANGUTIL_SYS_YANGCSRTP_H_ */
