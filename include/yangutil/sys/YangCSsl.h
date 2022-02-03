/*
 * YangCSsl.h
 *
 *  Created on: 2021年12月27日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_SYS_YANGCSSL_H_
#define INCLUDE_YANGUTIL_SYS_YANGCSSL_H_
#include <stdint.h>

#include <openssl/ssl.h>
typedef struct{
    char fingerprint[128];
    int32_t ecdsa_mode;
    X509* dtls_cert;
    EVP_PKEY* dtls_pkey;
    EC_KEY* eckey;
}YangCertificate;

int32_t hmac_encode(const char* algo, const char* key, const int key_length,
       const char* input, const int32_t input_length, char* output, unsigned int* output_length);

int32_t yang_init_certificate(YangCertificate* cer);
void yang_destroy_certificate(YangCertificate* cer);
void g_yang_init_srtp();
void g_yang_destroy_srtp();
#endif /* INCLUDE_YANGUTIL_SYS_YANGCSSL_H_ */
