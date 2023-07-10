//
// Copyright (c) 2019-2022 yanggaofeng guoai
//
//

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangMath.h>

#include <yangssl/YangSsl.h>

#if Yang_Enable_Openssl
#if Yang_Enable_Dtls

#include <srtp2/srtp.h>
#include <openssl/md5.h>

int32_t yang_hmac_encode(const char* algo, const char* key, const int32_t key_length,
        const char* input, const int32_t input_length, char* output, uint32_t* output_length)
{
    int32_t err = Yang_Ok;

    const EVP_MD* engine = NULL;
    if (yang_strcmp(algo , "sha512")==0) {
        engine = EVP_sha512();
    } else if(yang_strcmp(algo , "sha256")==0) {
        engine = EVP_sha256();
    } else if(yang_strcmp(algo ,  "sha1")==0) {
        engine = EVP_sha1();
    } else if(yang_strcmp(algo ,  "md5")==0) {
        engine = EVP_md5();
    } else if(yang_strcmp(algo ,  "sha224")==0) {
        engine = EVP_sha224();
    } else if(yang_strcmp(algo ,  "sha384")==0) {
        engine = EVP_sha384();
    } else {
    	yang_error( "ERROR_RTC_STUN unknown algo=%s", algo);
        return 1;
    }

#if !defined(OPENSSL_VERSION_NUMBER) || OPENSSL_VERSION_NUMBER < 0x10100000L
	HMAC_CTX hctx;
	HMAC_CTX_init(&hctx);
    if (&hctx == NULL) {
        return yang_error_wrap(ERROR_SSL, "hmac init faied");
    }

    if (HMAC_Init_ex(&hctx, key, key_length, engine, NULL) < 0) {
        HMAC_CTX_cleanup(&hctx);
        return yang_error_wrap(ERROR_SSL, "hmac init faied");
    }

    if (HMAC_Update(&hctx, (const uint8_t*)input, input_length) < 0) {
        HMAC_CTX_cleanup(&hctx);
        return yang_error_wrap(ERROR_SSL, "hmac update faied");
    }

    if (HMAC_Final(&hctx, (uint8_t*)output, &output_length) < 0) {
        HMAC_CTX_cleanup(&hctx);
        return yang_error_wrap(ERROR_SSL, "hmac final faied");
    }

    HMAC_CTX_cleanup(&hctx);

#else

    HMAC_CTX* ctx = HMAC_CTX_new();
    if (ctx == NULL) {
        return yang_error_wrap(ERROR_SSL, "hmac init faied");
    }

    if (HMAC_Init_ex(ctx, key, key_length, engine, NULL) < 0) {
        HMAC_CTX_free(ctx);
        return yang_error_wrap(ERROR_SSL, "hmac init faied");
    }

    if (HMAC_Update(ctx, (const uint8_t*)input, input_length) < 0) {
        HMAC_CTX_free(ctx);
        return yang_error_wrap(ERROR_SSL, "hmac update faied");
    }

    if (HMAC_Final(ctx, (uint8_t*)output, output_length) < 0) {
        HMAC_CTX_free(ctx);
        return yang_error_wrap(ERROR_SSL, "hmac final faied");
    }

    HMAC_CTX_free(ctx);
#endif

    return err;
}

int32_t yang_ssl_md5(const uint8_t* keystr,size_t keystrLen,uint8_t* key){
	MD5(keystr,keystrLen,key);
	return Yang_Ok;
}


void yang_destroy_certificate( YangCertificate* cer){
    if(cer==NULL) return;
	if (cer->eckey) {
		EC_KEY_free(cer->eckey);
	}

	if (cer->dtls_pkey) {
		EVP_PKEY_free(cer->dtls_pkey);
	}

	if (cer->dtls_cert) {
		X509_free(cer->dtls_cert);
	}
}
int32_t g_yang_srtp_inited=-1;

void g_yang_create_srtp(){
	   if(g_yang_srtp_inited==-1){
		// Initialize SRTP first.
	    	g_yang_srtp_inited=srtp_init();
			if(g_yang_srtp_inited)
				yang_error("srtp init fail");
			else
				yang_trace("\nsrtp init success!\n");
	    }
}
void g_yang_destroy_srtp(){
	if(g_yang_srtp_inited!=-1){
		srtp_shutdown();
		g_yang_srtp_inited=-1;
	}
}
int32_t yang_create_certificate( YangCertificate* cer) {
     if(cer==NULL) return 1;
	int32_t err = Yang_Ok;
    cer->ecdsa_mode = yangtrue;
	// Initialize once.
	if (cer->dtls_cert) {
		return err;
	}

#if OPENSSL_VERSION_NUMBER < 0x10100000L // v1.1.x
    OpenSSL_add_ssl_algorithms();
#else

#endif
    g_yang_create_srtp();
	cer->ecdsa_mode = 1;
	cer->dtls_pkey = EVP_PKEY_new();

	if (!cer->ecdsa_mode) { // By RSA
		RSA *rsa = RSA_new();
    	if(rsa==NULL) 		return yang_error_wrap(ERROR_SSL,"Certificate rsa create fail");

		// Initialize the big-number for private key.
		BIGNUM *exponent = BN_new();
		if(exponent==NULL) 		return yang_error_wrap(ERROR_SSL,"Certificate BIGNUM create fail");
		BN_set_word(exponent, RSA_F4);
		int32_t key_bits = 1024;
		RSA_generate_key_ex(rsa, key_bits, exponent, NULL);
        if(EVP_PKEY_set1_RSA(cer->dtls_pkey, rsa) != 1) yang_error("EVP_PKEY_set1_RSA error");

		RSA_free(rsa);
		BN_free(exponent);
	}
	if (cer->ecdsa_mode) { // By ECDSA, https://stackoverflow.com/a/6006898
		cer->eckey = EC_KEY_new();
		EC_GROUP *ecgroup = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);

#if OPENSSL_VERSION_NUMBER < 0x10100000L // v1.1.x
        // For openssl 1.0, we must set the group parameters, so that cert is ok.
        // @see https://github.com/monero-project/monero/blob/master/contrib/epee/src/net_ssl.cpp#L225
        EC_GROUP_set_asn1_flag(ecgroup, OPENSSL_EC_NAMED_CURVE);
#endif

        if(EC_KEY_set_group(cer->eckey, ecgroup) != 1) return yang_error_wrap(ERROR_SSL,"EC_KEY_set_group error");
        if(EC_KEY_generate_key(cer->eckey) != 1) return yang_error_wrap(ERROR_SSL,"EC_KEY_generate_key error");
        if(EVP_PKEY_set1_EC_KEY(cer->dtls_pkey, cer->eckey) != 1) return yang_error_wrap(ERROR_SSL,"EVP_PKEY_set1_EC_KEY error");

		EC_GROUP_free(ecgroup);
	}

	// Create certificate, from previous generated pkey.
	// TODO: Support ECDSA certificate.
	cer->dtls_cert = X509_new();


	X509_NAME *subject = X509_NAME_new();


	int32_t serial = (int) yang_random();
	ASN1_INTEGER_set(X509_get_serialNumber(cer->dtls_cert), serial);

	X509_NAME_add_entry_by_txt(subject, "CN", MBSTRING_ASC,
			(const uint8_t*)"metaRTC", (int)yang_strlen("metaRTC"), -1, 0);

	X509_set_issuer_name(cer->dtls_cert, subject);
	X509_set_subject_name(cer->dtls_cert, subject);

	int32_t expire_day = 365;
	const long cert_duration = 60 * 60 * 24 * expire_day;

	X509_gmtime_adj(X509_get_notBefore(cer->dtls_cert), 0);
	X509_gmtime_adj(X509_get_notAfter(cer->dtls_cert), cert_duration);

	X509_set_version(cer->dtls_cert, 2);
	if(X509_set_pubkey(cer->dtls_cert, cer->dtls_pkey) != 1) yang_error("EVP_PKEY_set1_EC_KEY error");
	if(X509_sign(cer->dtls_cert, cer->dtls_pkey, EVP_sha1()) == 0) yang_error("EVP_PKEY_set1_EC_KEY error");

	X509_NAME_free(subject);


	// DTLS fingerprint

	char fp[100] = { 0 };
	char *p = fp;
	uint8_t md[EVP_MAX_MD_SIZE];
	uint32_t  n = 0;


	X509_digest(cer->dtls_cert, EVP_sha256(), md, &n);

	for (uint32_t  i = 0; i < n; i++, ++p) {
		yang_sprintf(p, "%02X", md[i]);
		p += 2;

		if (i < (n - 1)) {
			*p = ':';
		} else {
			*p = '\0';
		}
	}
	yang_memset(cer->fingerprint,0,sizeof(cer->fingerprint));
	yang_memcpy(cer->fingerprint,fp, yang_strlen(fp));



	return err;
}
#endif
#endif
