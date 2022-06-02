//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGRTCDTLSH_H_
#define SRC_YANGRTC_YANGRTCDTLSH_H_
#include <yangutil/yangtype.h>

#include <yangutil/yangavctype.h>
#include <yangrtc/YangUdpHandleH.h>
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
    YangUdpHandle* udp;
    YangSRtp* srtp;
    SSL_CTX* sslctx;
    YangSslCallback* sslCallback;

}YangRtcDtls;
#endif
#endif /* SRC_YANGRTC_YANGRTCDTLSH_H_ */
