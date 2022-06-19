//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGRTCDTLS_H_
#define SRC_YANGRTC_YANGRTCDTLS_H_
#include <yangrtc/YangRtcUdp.h>
#include <yangutil/yangtype.h>

#include <yangutil/yangavctype.h>
#if Yang_HaveDtls
#include <yangutil/sys/YangSsl.h>
#include <yangutil/sys/YangSRtp.h>

#include <stdint.h>
#include <pthread.h>

typedef enum{
    YangDtlsRoleClient,
    YangDtlsRoleServer
}YangDtlsRole;

typedef enum{
    YangDtlsStateInit, // Start.
    YangDtlsStateClientHello, // Should start ARQ thread.
    YangDtlsStateServerHello, // We are in the first ARQ state.
    YangDtlsStateClientCertificate, // Should start ARQ thread again.
    YangDtlsStateServerDone, // We are in the second ARQ state.
    YangDtlsStateClientDone, // Done.
}YangDtlsState;

typedef enum{
    YangDtlsVersionAuto = -1,
    YangDtlsVersion1_0,
    YangDtlsVersion1_2
}YangDtlsVersion;


typedef struct {
	YangDtlsState state;
    YangDtlsVersion version;
    int32_t handshake_done;
    int32_t isStart;
    int32_t isStop;
    int32_t isLoop;
    int32_t reset_timer_;
    int32_t uid;
    int32_t isServer;
    pthread_t threadId;
    SSL* ssl;
    BIO* bioIn;
    BIO* bioOut;
    YangCertificate *cer;
    YangRtcUdp* udp;
    YangSRtp* srtp;
    SSL_CTX* sslctx;
    YangSslCallback* sslCallback;
}YangDtlsSession;

typedef struct{
	YangDtlsSession session;
	void (*start)(YangDtlsSession* session);
	void (*stop)(YangDtlsSession* session);
	int32_t (*doHandshake)(YangDtlsSession* session);
	int32_t (*startHandShake)(YangDtlsSession* session);

	int32_t (*processData)(void* user,YangDtlsSession* session,char* p,int32_t len);

	int32_t (*sendDtlsAlert)(YangDtlsSession* session);
	int32_t (*filterData)(YangDtlsSession* session,uint8_t* data, int32_t size);
	int32_t (*getSrtpKey)(YangDtlsSession* session,char* recv_key, int *precvkeylen,char* send_key,int *psendkeylen);
	int32_t (*sendSctpData)(YangDtlsSession* session,uint8_t* pdata, int32_t nb);
}YangRtcDtls;

int32_t yang_create_rtcdtls(YangRtcDtls* dtls,int32_t isServer);
void yang_destroy_rtcdtls(YangRtcDtls* dtls);

#endif
#endif /* SRC_YANGRTC_YANGRTCDTLS_H_ */
