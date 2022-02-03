
#ifndef YANGWEBRTC_YANGRTCDTLS_H_
#define YANGWEBRTC_YANGRTCDTLS_H_

#include <yangwebrtc/YangRtcDtlsH.h>
#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_init_rtcdtls(YangRtcDtls* dtls);
void yang_destroy_rtcdtls(YangRtcDtls* dtls);
void yang_start_rtcdtls(YangRtcDtls* dtls);
void yang_stop_rtcdtls(YangRtcDtls* dtls);
int32_t yang_doHandshake(YangRtcDtls* dtls);
int32_t yang_startHandShake(YangRtcDtls* dtls);
int32_t yang_decodeHandshake(YangRtcDtls* dtls,char* p,int32_t len);
int32_t yang_sendDtlsAlert(YangRtcDtls* dtls);
int32_t yang_filter_data(YangRtcDtls* dtls,uint8_t* data, int32_t size);


int32_t yang_get_srtp_key(YangRtcDtls* dtls,char* recv_key, int *precvkeylen,char* send_key,int *psendkeylen);
#ifdef __cplusplus
}
#endif



#endif /* YANGWEBRTC_YANGRTCDTLS_H_ */
