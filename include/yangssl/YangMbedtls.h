/*
 * YangSslMbedssl.h
 *
 *  Created on: 2022年7月25日
 *      Author: yang
 */

#ifndef INCLUDE_YANGSSL_YANGMBEDTLS_H_
#define INCLUDE_YANGSSL_YANGMBEDTLS_H_
#include <yangutil/yangtype.h>
#include <stdint.h>
#if !Yang_Enable_Openssl
#if Yang_Enable_Dtls
#include <mbedtls/version.h>
#include <mbedtls/ssl.h>

#define Yang_SHA256_Length 32
#if (MBEDTLS_VERSION_NUMBER>0x03000000)
#define Yang_Mbedtls_3 1 //0:mbedtls2.0 1:mbedtls3.0
#else
#define Yang_Mbedtls_3 0 //0:mbedtls2.0 1:mbedtls3.0
#endif

typedef struct{
    char fingerprint[128];
    yangbool ecdsa_mode;
    mbedtls_x509_crt *dtls_cert;
    mbedtls_pk_context *dtls_pkey;
}YangCertificate;

#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_hmac_encode(const char* algo, const char* key, const int32_t key_length,
       const char* input, const int32_t input_length, char* output, uint32_t* output_length);

int32_t yang_ssl_md5(const uint8_t* keystr,size_t keystrLen,uint8_t* str);

int32_t yang_create_certificate(YangCertificate* cer);
void yang_destroy_certificate(YangCertificate* cer);
void g_yang_create_srtp();
void g_yang_destroy_srtp();
#ifdef __cplusplus
}
#endif

#endif
#endif
#endif /* INCLUDE_YANGSSL_YANGMBEDTLS_H_ */
