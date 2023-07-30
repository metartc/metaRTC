//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGSSL_YANGOPENSSL_H_
#define INCLUDE_YANGSSL_YANGOPENSSL_H_


#include <yangutil/yangtype.h>


#if Yang_Enable_Openssl
#if Yang_Enable_Dtls

#include <openssl/ssl.h>
typedef struct{
    int32_t ecdsa_mode;
    X509* dtls_cert;
    EVP_PKEY* dtls_pkey;
    EC_KEY* eckey;
    char fingerprint[128];
}YangCertificate;



#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_hmac_encode(const char* algo, const char* key, const int32_t key_length,
       const char* input, const int32_t input_length, char* output, uint32_t* output_length);
int32_t yang_ssl_md5(const uint8_t* keystr,size_t keystrLen,uint8_t* key);
int32_t yang_create_certificate(YangCertificate* cer);
void yang_destroy_certificate(YangCertificate* cer);
void g_yang_create_srtp();
void g_yang_destroy_srtp();
#ifdef __cplusplus
}
#endif

#endif
#endif


#endif /* INCLUDE_YANGSSL_YANGOPENSSL_H_ */
