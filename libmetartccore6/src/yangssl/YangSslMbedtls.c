//
// Copyright (c) 2019-2022 yanggaofeng guoai
//
//

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangMath.h>

#include <yangssl/YangMbedtls.h>

#if !Yang_Enable_Openssl
#if Yang_Enable_Dtls
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/sha256.h>
#include <mbedtls/md5.h>
#include <srtp2/srtp.h>

int32_t yang_hmac_encode(const char *algo, const char *key, const int32_t key_length,
		const char *input, const int32_t input_length, char *output,
		uint32_t *output_length) {
	int32_t err = Yang_Ok;

	if ((err = mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1),
			(const uint8_t*) key, key_length, (const uint8_t*) input,
			input_length, (uint8_t*) output)) != Yang_Ok) {
		yang_error("mbedtls_md_hmac fail");
	}
	*(output_length) = mbedtls_md_get_size(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1));

	return err;
}


int32_t yang_ssl_md5(const uint8_t* keystr,size_t keystrLen,uint8_t* key){

#if Yang_Mbedtls_3
	return mbedtls_md5(keystr,keystrLen,key);
#else
	mbedtls_md5_ret(keystr,keystrLen,key);
	return Yang_Ok;
#endif


}


int32_t g_yang_srtp_inited = -1;

void g_yang_create_srtp() {
	if (g_yang_srtp_inited == -1) {
		// Initialize SRTP first.
		g_yang_srtp_inited = srtp_init();
		if (g_yang_srtp_inited)
			yang_error("srtp init fail");
		else
			yang_trace("\nsrtp init success!\n");
	}
}

void g_yang_destroy_srtp() {
	if (g_yang_srtp_inited != -1) {
		srtp_shutdown();
		g_yang_srtp_inited = -1;
	}
}

int32_t yang_dtls_randomBits(uint8_t *buf, uint32_t bufSize) {
	for (uint32_t i = 0; i < bufSize; i++) {
		*buf++ = (uint8_t) (rand() & 0xFF);
	}

	return Yang_Ok;
}

void yang_dtls_genDateStr(time_t ts, char *str) {
	struct tm *ntm = localtime(&ts);
	yang_sprintf(str, "%04d%02d%02d%02d%02d%02d", ntm->tm_year + 1900,
			ntm->tm_mon + 1, ntm->tm_mday, ntm->tm_hour, ntm->tm_min,
			ntm->tm_sec);
}

int32_t yang_create_certificate(YangCertificate *cer) {

	int32_t err = Yang_Ok;
	yangbool ca_inited = yangfalse;
	cer->ecdsa_mode = yangtrue;
	g_yang_create_srtp();
	int32_t caLen;
	mbedtls_entropy_context *entropy = NULL;
	mbedtls_ctr_drbg_context *ctrDrbg = NULL;
	mbedtls_mpi serial;
	mbedtls_x509write_cert *tmpCert = NULL;
	uint8_t caSn[20];
	uint8_t *certBuf = (uint8_t*) yang_calloc(4096, 1);
	if (cer->dtls_cert == NULL)
		cer->dtls_cert = (mbedtls_x509_crt*) yang_calloc(sizeof(mbedtls_x509_crt),
				1);
	if (cer->dtls_pkey == NULL)
		cer->dtls_pkey = (mbedtls_pk_context*) yang_calloc(
				sizeof(mbedtls_pk_context), 1);
	entropy = (mbedtls_entropy_context*) yang_calloc(sizeof(mbedtls_entropy_context),
			1);
	ctrDrbg = (mbedtls_ctr_drbg_context*) yang_calloc(
			sizeof(mbedtls_ctr_drbg_context), 1);
	tmpCert = (mbedtls_x509write_cert*) yang_calloc(sizeof(mbedtls_x509write_cert),
			1);
	char before[16] = { 0 }, after[16] = { 0 };
	yang_dtls_randomBits(caSn, sizeof(caSn));

	mbedtls_entropy_init(entropy);
	mbedtls_ctr_drbg_init(ctrDrbg);
	mbedtls_mpi_init(&serial);
	mbedtls_x509write_crt_init(tmpCert);
	mbedtls_x509_crt_init(cer->dtls_cert);
	mbedtls_pk_init(cer->dtls_pkey);
	ca_inited = yangtrue;
	if ((err = mbedtls_ctr_drbg_seed(ctrDrbg, mbedtls_entropy_func, entropy,
			NULL, 0)) != Yang_Ok) {
		yang_error("mbedtls_ctr_drbg_seed fail");
		goto ret;
	}

	// generate a key
	if (cer->ecdsa_mode) {
		if (mbedtls_pk_setup(cer->dtls_pkey,
				mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) != Yang_Ok
				|| mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1,
						mbedtls_pk_ec(*cer->dtls_pkey), mbedtls_ctr_drbg_random,
						ctrDrbg) != Yang_Ok) {
			yang_error("certifate create eckey fail");
			err = 1;
			goto ret;
		}

	} else {
		//RSA_F4 0x10001
		if (mbedtls_pk_setup(cer->dtls_pkey,
				mbedtls_pk_info_from_type(MBEDTLS_PK_RSA)) != Yang_Ok
				|| mbedtls_rsa_gen_key(mbedtls_pk_rsa(*cer->dtls_pkey),
						mbedtls_ctr_drbg_random, ctrDrbg, 1024, 0x10001L)
						!= Yang_Ok) {
			yang_error("certifate create pk_rsa fail");
			err = 1;
			goto ret;
		}
	}

	// create a new certificate
	if ((err = mbedtls_mpi_read_binary(&serial, caSn, sizeof(caSn))) != Yang_Ok) {
		yang_error("mbedtls_mpi_read_binary	 fail");
		goto ret;
	}

	time_t t_now = time(NULL)-2 * 24 * 60 * 60;
	yang_dtls_genDateStr(t_now, before);
	yang_dtls_genDateStr(t_now + 365 * 24 * 60 * 60, after);

	if (mbedtls_x509write_crt_set_serial(tmpCert, &serial) != Yang_Ok
			|| mbedtls_x509write_crt_set_validity(tmpCert, before,after) !=Yang_Ok ||
					mbedtls_x509write_crt_set_subject_name(tmpCert, "O=metaRTC,CN=metaRTC" ) != Yang_Ok ||
					mbedtls_x509write_crt_set_issuer_name(tmpCert, "O=metaRTC,CN=metaRTC" ) != Yang_Ok) {
		yang_error("crt generate fail(mbedtls_x509write_crt_set_serial)");
		err = 1;
		goto ret;
	}

	mbedtls_x509write_crt_set_version(tmpCert, MBEDTLS_X509_CRT_VERSION_3);
	mbedtls_x509write_crt_set_subject_key(tmpCert, cer->dtls_pkey);
	mbedtls_x509write_crt_set_issuer_key(tmpCert, cer->dtls_pkey);
	mbedtls_x509write_crt_set_md_alg(tmpCert, MBEDTLS_MD_SHA1);

	yang_memset(certBuf, 0, 4096);
	caLen = mbedtls_x509write_crt_der(tmpCert, certBuf, 4096,
			mbedtls_ctr_drbg_random, ctrDrbg);
	if (caLen < 0) {
		yang_error(
				"crt generate fail(mbedtls_x509write_crt_der),ca length=%04x",
				caLen);
		goto ret;
	}

	if ((err = mbedtls_x509_crt_parse_der(cer->dtls_cert,
			certBuf + 4096 - caLen, caLen)) != Yang_Ok) {

		yang_error(
				"crt generate fail(mbedtls_x509_crt_parse_der),err=%d,err=%04x",
				err, -err);
		goto ret;
	}

	char fp[128] = { 0 };
	char *p = fp;
	uint8_t md[128] = { 0 };
#if Yang_Mbedtls_3
	if ((err = mbedtls_sha256(cer->dtls_cert->raw.p, cer->dtls_cert->raw.len,md, 0)) != Yang_Ok) {
#else
	if ((err = mbedtls_sha256_ret(cer->dtls_cert->raw.p, cer->dtls_cert->raw.len,md, 0)) != Yang_Ok) {
#endif

		yang_error("mbedtls_sha256 fail");
	}

	size_t n = Yang_SHA256_Length;

	for (size_t i = 0; i < n; i++, ++p) {
		yang_sprintf(p, "%02X", md[i]);
		p += 2;

		if (i < (n - 1)) {
			*p = ':';
		} else {
			*p = '\0';
		}
	}
	yang_memset(cer->fingerprint, 0, sizeof(cer->fingerprint));
	yang_memcpy(cer->fingerprint, fp, yang_strlen(fp));

	ret: if (ca_inited) {
		mbedtls_x509write_crt_free(tmpCert);
		mbedtls_mpi_free(&serial);
		mbedtls_ctr_drbg_free(ctrDrbg);
		mbedtls_entropy_free(entropy);

		if (err) {
			mbedtls_x509_crt_free(cer->dtls_cert);
			mbedtls_pk_free(cer->dtls_pkey);
		}
	}

	yang_free(certBuf);
	yang_free(entropy);
	yang_free(ctrDrbg);
	yang_free(tmpCert);
	return err;
}

void yang_destroy_certificate(YangCertificate *cer) {
	if (cer == NULL)
		return;
	if (cer->dtls_cert)
		mbedtls_x509_crt_free(cer->dtls_cert);
	if (cer->dtls_pkey)
		mbedtls_pk_free(cer->dtls_pkey);
	yang_free(cer->dtls_cert);
	yang_free(cer->dtls_pkey);

}
#endif
#endif
