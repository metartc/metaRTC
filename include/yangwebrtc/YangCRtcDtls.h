/*
 * YangCRtcDtls.h
 *
 *  Created on: 2021年12月28日
 *      Author: yang
 */

#ifndef SRC_YANGWEBRTC_YANGCRTCDTLS_H_
#define SRC_YANGWEBRTC_YANGCRTCDTLS_H_



#include <yangwebrtc/YangUdpHandle.h>

#include <yangutil/sys/YangSsl.h>
#include <yangutil/sys/YangSRtp.h>


#include <stdint.h>
#include <yangutil/yangtype.h>


enum YangDtlsRole {
    YangDtlsRoleClient,
    YangDtlsRoleServer
};
enum YangDtlsState {
    YangDtlsStateInit, // Start.
    YangDtlsStateClientHello, // Should start ARQ thread.
    YangDtlsStateServerHello, // We are in the first ARQ state.
    YangDtlsStateClientCertificate, // Should start ARQ thread again.
    YangDtlsStateServerDone, // We are in the second ARQ state.
    YangDtlsStateClientDone, // Done.
};

enum YangDtlsVersion {
    YangDtlsVersionAuto = -1,
    YangDtlsVersion1_0,
    YangDtlsVersion1_2
};
typedef struct {
	enum YangDtlsState state;
    enum YangDtlsVersion version;

    bool handshake_done;
    int isStart;
    int isLoop;
    bool reset_timer_;

    pthread_t threadId;
    SSL* ssl;
    BIO* bioIn;
    BIO* bioOut;
    YangCertificate *cer;
    YangUdpHandle* udp;
    YangSRtp* srtp;
    SSL_CTX* sslctx;
}YangRtcDtls;
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



#endif /* SRC_YANGWEBRTC_YANGCRTCDTLS_H_ */
