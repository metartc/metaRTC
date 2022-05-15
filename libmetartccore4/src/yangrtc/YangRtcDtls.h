
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGWEBRTC_YANGRTCDTLS_H_
#define YANGWEBRTC_YANGRTCDTLS_H_

#include <yangrtc/YangRtcDtlsH.h>
#if Yang_HaveDtls
#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_create_rtcdtls(YangRtcDtls* dtls,int32_t isServer);
void yang_destroy_rtcdtls(YangRtcDtls* dtls);
void yang_start_rtcdtls(YangRtcDtls* dtls);
void yang_stop_rtcdtls(YangRtcDtls* dtls);
int32_t yang_doHandshake(YangRtcDtls* dtls);
int32_t yang_startHandShake(YangRtcDtls* dtls);
int32_t yang_process_dtls_data(void* user,YangRtcDtls* dtls,char* p,int32_t len);
int32_t yang_sendDtlsAlert(YangRtcDtls* dtls);
int32_t yang_filter_data(YangRtcDtls* dtls,uint8_t* data, int32_t size);
int32_t yang_get_srtp_key(YangRtcDtls* dtls,char* recv_key, int *precvkeylen,char* send_key,int *psendkeylen);
int32_t yang_dtls_sendSctpData(YangRtcDtls* dtls,uint8_t* pdata, int32_t nb);
//int32_t yang_dtls_receiveSctpData(YangRtcDtls* dtls,uint8_t* src, int32_t src_nb,uint8_t* dst,int32_t* dst_nb);
#ifdef __cplusplus
}
#endif
#endif


#endif /* YANGWEBRTC_YANGRTCDTLS_H_ */
