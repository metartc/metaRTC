#include <yangutil/sys/YangLog.h>
#include <srtp2/srtp.h>
#include <assert.h>
#include <yangutil/sys/YangSsl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangString.h>
#include <yangutil/yangavinfotype.h>
#define yang_assert(expression) assert(expression)
int32_t hmac_encode(const std::string& algo, const char* key, const int& key_length,
        const char* input, const int32_t input_length, char* output, unsigned int& output_length)
{
    int32_t err = Yang_Ok;

    const EVP_MD* engine = NULL;
    if (algo == "sha512") {
        engine = EVP_sha512();
    } else if(algo == "sha256") {
        engine = EVP_sha256();
    } else if(algo == "sha1") {
        engine = EVP_sha1();
    } else if(algo == "md5") {
        engine = EVP_md5();
    } else if(algo == "sha224") {
        engine = EVP_sha224();
    } else if(algo == "sha384") {
        engine = EVP_sha384();
    } else {
    	yang_error( "ERROR_RTC_STUN unknown algo=%s", algo.c_str());
        return 1;
    }

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

    if (HMAC_Final(ctx, (uint8_t*)output, &output_length) < 0) {
        HMAC_CTX_free(ctx);
        return yang_error_wrap(ERROR_SSL, "hmac final faied");
    }

    HMAC_CTX_free(ctx);

    return err;
}



YangCertificateImpl::YangCertificateImpl() {
	ecdsa_mode = true;
	dtls_cert = NULL;
	dtls_pkey = NULL;
	eckey = NULL;
}

YangCertificateImpl::~YangCertificateImpl() {
	if (eckey) {
		EC_KEY_free(eckey);
	}

	if (dtls_pkey) {
		EVP_PKEY_free(dtls_pkey);
	}

	if (dtls_cert) {
		X509_free(dtls_cert);
	}
}

int32_t YangCertificateImpl::init() {
	int32_t err = Yang_Ok;

	// Initialize once.
	if (dtls_cert) {
		return err;
	}

#if OPENSSL_VERSION_NUMBER < 0x10100000L // v1.1.x
    OpenSSL_add_ssl_algorithms();
#else

#endif

	// Initialize SRTP first.
    if(srtp_init())
        yang_error("srtp init fail...................");
    else
        yang_trace("\nsrtp init success!...................\n");

	ecdsa_mode = 1;    //_srs_config->get_rtc_server_ecdsa();
	dtls_pkey = EVP_PKEY_new();
    //yang_assert(dtls_pkey);
	if (!ecdsa_mode) { // By RSA
		RSA *rsa = RSA_new();
    	if(rsa==NULL) 		return yang_error_wrap(ERROR_SSL,"Certificate rsa create fail");

		// Initialize the big-number for private key.
		BIGNUM *exponent = BN_new();
		if(exponent==NULL) 		return yang_error_wrap(ERROR_SSL,"Certificate BIGNUM create fail");
		BN_set_word(exponent, RSA_F4);
		int32_t key_bits = 1024;
		RSA_generate_key_ex(rsa, key_bits, exponent, NULL);
        if(EVP_PKEY_set1_RSA(dtls_pkey, rsa) != 1) yang_error("EVP_PKEY_set1_RSA error");

		RSA_free(rsa);
		BN_free(exponent);
	}
	if (ecdsa_mode) { // By ECDSA, https://stackoverflow.com/a/6006898
		eckey = EC_KEY_new();
		yang_assert(eckey);
		EC_GROUP *ecgroup = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
		yang_assert(ecgroup);
#if OPENSSL_VERSION_NUMBER < 0x10100000L // v1.1.x
        // For openssl 1.0, we must set the group parameters, so that cert is ok.
        // @see https://github.com/monero-project/monero/blob/master/contrib/epee/src/net_ssl.cpp#L225
        EC_GROUP_set_asn1_flag(ecgroup, OPENSSL_EC_NAMED_CURVE);
#endif

        if(EC_KEY_set_group(eckey, ecgroup) != 1) return yang_error_wrap(ERROR_SSL,"EC_KEY_set_group error");
        if(EC_KEY_generate_key(eckey) != 1) return yang_error_wrap(ERROR_SSL,"EC_KEY_generate_key error");
        if(EVP_PKEY_set1_EC_KEY(dtls_pkey, eckey) != 1) return yang_error_wrap(ERROR_SSL,"EVP_PKEY_set1_EC_KEY error");

		EC_GROUP_free(ecgroup);
	}

	// Create certificate, from previous generated pkey.
	// TODO: Support ECDSA certificate.
	dtls_cert = X509_new();
	yang_assert(dtls_cert);
	if (true) {
		X509_NAME *subject = X509_NAME_new();
		yang_assert(subject);

		int32_t serial = (int) yang_random();
		ASN1_INTEGER_set(X509_get_serialNumber(dtls_cert), serial);

		const std::string &aor = "yangrtc";//RTMP_SIG_SRS_DOMAIN;
		X509_NAME_add_entry_by_txt(subject, "CN", MBSTRING_ASC,
				(uint8_t*) aor.data(), aor.size(), -1, 0);

		X509_set_issuer_name(dtls_cert, subject);
		X509_set_subject_name(dtls_cert, subject);

		int32_t expire_day = 365;
		const long cert_duration = 60 * 60 * 24 * expire_day;

		X509_gmtime_adj(X509_get_notBefore(dtls_cert), 0);
		X509_gmtime_adj(X509_get_notAfter(dtls_cert), cert_duration);

		X509_set_version(dtls_cert, 2);
        if(X509_set_pubkey(dtls_cert, dtls_pkey) != 1) yang_error("EVP_PKEY_set1_EC_KEY error");
        if(X509_sign(dtls_cert, dtls_pkey, EVP_sha1()) == 0) yang_error("EVP_PKEY_set1_EC_KEY error");

		X509_NAME_free(subject);
	}

	// Show DTLS fingerprint
	if (true) {
		char fp[100] = { 0 };
		char *p = fp;
		uint8_t md[EVP_MAX_MD_SIZE];
		uint32_t  n = 0;

		// TODO: FIXME: Unused variable.
		/*int32_t r = */X509_digest(dtls_cert, EVP_sha256(), md, &n);

		for (uint32_t  i = 0; i < n; i++, ++p) {
			sprintf(p, "%02X", md[i]);
			p += 2;

			if (i < (n - 1)) {
				*p = ':';
			} else {
				*p = '\0';
			}
		}

		fingerprint.assign(fp, strlen(fp));
		//yang_trace("fingerprint=%s", fingerprint.c_str());
	}

	return err;
}

X509* YangCertificateImpl::get_cert() {
	return dtls_cert;
}

EVP_PKEY* YangCertificateImpl::get_public_key() {
	return dtls_pkey;
}

EC_KEY* YangCertificateImpl::get_ecdsa_key() {
	return eckey;
}

std::string YangCertificateImpl::get_fingerprint() {
	return fingerprint;
}

bool YangCertificateImpl::is_ecdsa() {
	return ecdsa_mode;
}
