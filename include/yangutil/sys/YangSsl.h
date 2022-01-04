#ifndef INCLUDE_YANGUTIL_SYS_YANGSSL_H_
#define INCLUDE_YANGUTIL_SYS_YANGSSL_H_

#include <stdint.h>

#include <string.h>

#ifdef __cplusplus
extern "C"{
#include <yangutil/sys/YangCSsl.h>
}
#include <string>
std::string encode_hmac(char* hamc_buf, const int32_t hmac_buf_len);
std::string encode_fingerprint(uint32_t crc32);
#else
#include <yangutil/sys/YangCSsl.h>
#endif



/**
class YangCertificateImpl
{

public:
    YangCertificateImpl();
    virtual ~YangCertificateImpl();
private:
    std::string fingerprint;
    bool ecdsa_mode;
    X509* dtls_cert;
    EVP_PKEY* dtls_pkey;
    EC_KEY* eckey;

public:
    // Initialize DTLS certificate.
    int32_t init();

    // dtls_cert
    X509* get_cert();
    // public key
    EVP_PKEY* get_public_key();
    // ECDSA key
    EC_KEY* get_ecdsa_key();
    // certificate fingerprint
    std::string get_fingerprint();
    // whether is ecdsa
    bool is_ecdsa();
public:

};

/*
 * class YangCertificate{
public:
	YangCertificate(){};
	virtual ~YangCertificate(){};
	virtual int32_t init()=0;
	virtual    std::string get_fingerprint()=0;
	    // whether is ecdsa
	virtual   bool is_ecdsa()=0;
};
 * */
#endif /* INCLUDE_YANGUTIL_SYS_YANGSSL_H_ */
