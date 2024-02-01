//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRTC_YANGRTCDTLS_H_
#define SRC_YANGRTC_YANGRTCDTLS_H_
#include <yangutil/yangavctype.h>
#include <yangice/YangRtcSocket.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/buffer/YangBuffer.h>

#if Yang_Enable_Dtls
#include <yangssl/YangSsl.h>
#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangThread.h>

#if !Yang_Enable_Openssl
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/sha256.h>
#include <mbedtls/md5.h>
typedef struct {
	uint64_t updatedTime;
	uint32_t int_ms, fin_ms;
}YangMbedTimer;
#endif



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
	yangbool handshake_done;
	yangbool isRecvAlert;
	yangbool isSendAlert;

	yangbool isStart;
	yangbool isLoop;
	yangbool reset_timer_;
	yangbool isControled;

	int32_t uid;
	yang_thread_t threadId;

	YangRtcSocket* sock;
	YangSRtp* srtp;
	YangSslCallback* sslCallback;
	YangCertificate *cer;

#if Yang_Enable_Openssl
	SSL_CTX* sslctx;
	SSL* ssl;

	BIO* bioIn;
	BIO* bioOut;
#else
	YangMbedTimer mbtimer;
	mbedtls_ssl_context* ssl;
	mbedtls_ssl_config* sslConfig;
	mbedtls_entropy_context* entropy;
	mbedtls_ctr_drbg_context* ctrDrbg;
	mbedtls_tls_prf_types tlsPrf;
	mbedtls_tls_prf_types srtpPrf;
	uint8_t masterSecret[48];
	void* datachannel;
	uint8_t randBytes[2 * 32];
	uint8_t buffer[kRtpPacketSize];
	int32_t bufferLen;
#endif



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

int32_t yang_create_rtcdtls(YangRtcDtls* dtls,yangbool isControled);
void yang_destroy_rtcdtls(YangRtcDtls* dtls);

#endif
#endif /* SRC_YANGRTC_YANGRTCDTLS_H_ */
