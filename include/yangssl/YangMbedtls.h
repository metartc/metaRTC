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
#if !Yang_Using_Openssl
#if Yang_HaveDtls

#include <mbedtls/ssl.h>
#define Yang_SHA256_Length 32
typedef struct{
    char fingerprint[128];
    yangbool ecdsa_mode;
    mbedtls_x509_crt *dtls_cert;
    mbedtls_pk_context *dtls_pkey;
}YangCertificate;

#ifdef __cplusplus
extern "C"{
#endif
int32_t hmac_encode(const char* algo, const char* key, const int key_length,
       const char* input, const int32_t input_length, char* output, unsigned int* output_length);

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
