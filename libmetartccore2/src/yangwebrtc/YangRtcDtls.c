//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangwebrtc/YangRtcDtls.h>
#include <yangwebrtc/YangUdpHandle.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangSsrc.h>

#include <yangutil/sys/YangLog.h>
#include <time.h>

#define kRtpPacketSize 1500

#include <openssl/err.h>

SSL_CTX* yang_build_dtls_ctx(YangRtcDtls *dtls, char *role);
bool yang_should_reset_timer(YangRtcDtls *dtls) {
	bool v = dtls->reset_timer_;
	dtls->reset_timer_ = false;
	return v;
}
int32_t yang_verify_callback(int32_t preverify_ok, X509_STORE_CTX *ctx) {
	// Always OK, we don't check the certificate of client,
	// because we allow client self-sign certificate.

	return 1;
}
void ssl_on_info(const SSL *dtls, int32_t where, int32_t ret) {
	//YangRtcDtls* dtls_impl = (YangRtcDtls*)SSL_get_ex_data(dtls, 0);
	const char *method;
	int32_t w = where & ~SSL_ST_MASK;
	if (w & SSL_ST_CONNECT) {
		method = "SSL_connect";
	} else if (w & SSL_ST_ACCEPT) {
		method = "SSL_accept";
	} else {
		method = "undefined";
	}

	int32_t r1 = SSL_get_error(dtls, ret);
	if (where & SSL_CB_LOOP) {

	} else if (where & SSL_CB_ALERT) {
		method = (where & SSL_CB_READ) ? "read" : "write";

		// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_alert_type_string_long.html

		// Notify the DTLS to handle the ALERT message, which maybe means media connection disconnect.
		// dtls_impl->on_dtls_alert(alert_type, alert_desc);
		yang_info("dtls alert ..type==%s,desc==%s",
				SSL_alert_type_string_long(ret), SSL_alert_desc_string(ret));
	}
}
uint32_t dtls_timer_cb(SSL *dtls, uint32_t previous_us) {
	YangRtcDtls *dtls_impl = (YangRtcDtls*) SSL_get_ex_data(dtls, 0);
	if (!dtls_impl)
		return yang_error_wrap(ERROR_RTC_DTLS, "YangRtcDtls is null");

	// Double the timeout. Note that it can be 0.
	uint32_t timeout_us = previous_us * 2;

	// If previous_us is 0, for example, the HelloVerifyRequest, we should response it ASAP.
	// When got ServerHello, we should reset the timer.
	if (previous_us == 0 || yang_should_reset_timer(dtls_impl)) {
		timeout_us = 50 * 1000; // in us
	}
	// Never exceed the max timeout.
	timeout_us = yang_min(timeout_us, 30 * 1000 * 1000); // in us
	return timeout_us;
}

int32_t yang_init_rtcdtls(YangRtcDtls *dtls) {
	if (!dtls)
		return 1;
	dtls->sslctx = NULL;
	dtls->ssl = NULL;
	dtls->bioIn = NULL;
	dtls->bioOut = NULL;
	dtls->version = YangDtlsVersionAuto;
	dtls->reset_timer_ = false;
	dtls->handshake_done = false;
	dtls->isStart = 0;
	dtls->isLoop = 0;
	dtls->state = YangDtlsStateInit;

	dtls->sslctx = yang_build_dtls_ctx(dtls, "actpass");

	if ((dtls->ssl = SSL_new(dtls->sslctx)) == NULL) {
		printf("\nERROR_OpenSslCreateSSL, SSL_new dtls");
		return 1;
	}

	SSL_set_ex_data(dtls->ssl, 0, dtls);
	SSL_set_info_callback(dtls->ssl, ssl_on_info);
	SSL_set_options(dtls->ssl, SSL_OP_NO_QUERY_MTU);
	SSL_set_mtu(dtls->ssl, kRtpPacketSize);
	SSL_set_connect_state(dtls->ssl); //SSL_set_accept_state(dtls);
	SSL_set_max_send_fragment(dtls->ssl, kRtpPacketSize);
#if OPENSSL_VERSION_NUMBER >= 0x1010102fL // 1.1.1b
	DTLS_set_timer_cb(dtls->ssl, dtls_timer_cb);
#endif

	if ((dtls->bioIn = BIO_new(BIO_s_mem())) == NULL) {
		printf("\nERROR_OpenSslBIONew, BIO_new in");
		return 1;
	}

	if ((dtls->bioOut = BIO_new(BIO_s_mem())) == NULL) {
		BIO_free(dtls->bioIn);
		printf("\nERROR_OpenSslBIONew, BIO_new out");
		return 1;
	}

	SSL_set_bio(dtls->ssl, dtls->bioIn, dtls->bioOut);


	return Yang_Ok;
}
void yang_destroy_rtcdtls(YangRtcDtls *dtls) {
	if (!dtls)
		return;
	if (dtls->sslctx) {
		SSL_CTX_free(dtls->sslctx);
		dtls->sslctx = NULL;
	}

	if (dtls->ssl) {
		// this function will free bio_in and bio_out
		SSL_free(dtls->ssl);
		dtls->ssl = NULL;
	}
}

#define YANG_UTIME_MILLISECONDS 1000
int32_t yang_run_rtcdtls_app(YangRtcDtls *dtls) {
	dtls->isStart = 1;
	int32_t err = Yang_Ok;
	const int32_t max_loop = 512;
	int32_t arq_count = 0;
	int32_t arq_max_retry = 12 * 2;
	for (int32_t i = 0; arq_count < arq_max_retry && i < max_loop; i++) {
		if (dtls->handshake_done) {
			return err;
		}

		// For DTLS client ARQ, the state should be specified.
		if (dtls->state != YangDtlsStateClientHello
				&& dtls->state != YangDtlsStateClientCertificate) {
			return err;
		}

		// If there is a timeout in progress, it sets *out to the time remaining
		// and returns one. Otherwise, it returns zero.
		int32_t r0 = 0;
		struct timeval to = { 0 };
		if ((r0 = DTLSv1_get_timeout(dtls->ssl, &to)) == 0) {
			// No timeout, for example?, wait for a default 50ms.
			yang_usleep(50 * YANG_UTIME_MILLISECONDS);
			continue;
		}
		int64_t timeout = to.tv_sec + to.tv_usec;

		if (timeout > 0) {
			timeout = yang_min(100 * YANG_UTIME_MILLISECONDS, timeout);
			timeout = yang_max(50 * YANG_UTIME_MILLISECONDS, timeout);
			yang_usleep((unsigned int) timeout);
			continue;
		}

		r0 = BIO_reset(dtls->bioOut);
		int32_t r1 = SSL_get_error(dtls->ssl, r0);
		if (r0 != 1) {
			return yang_error_wrap(ERROR_OpenSslBIOReset,
					"OpenSslBIORese BIO_reset r0=%d, r1=%d", r0, r1);
		}

		r0 = DTLSv1_handle_timeout(dtls->ssl);
		r1 = SSL_get_error(dtls->ssl, r0);
		if (r0 == 0) {
			continue; // No timeout had expired.
		}
		if (r0 != 1) {
			return yang_error_wrap(ERROR_RTC_DTLS,
					"dtls error ARQ r0=%d, r1=%d", r0, r1);

		}

		uint8_t *data = NULL;
		int32_t size = BIO_get_mem_data(dtls->bioOut, (char** )&data);
		arq_count++;
		if (size
				> 0&& (err = yang_rtc_sendData(dtls->udp,(char*)data, size)) != Yang_Ok) {
			return yang_error_wrap(ERROR_RTC_DTLS, "error dtls send size=%u",
					size);
		}
	}
	dtls->isStart = 0;
	return Yang_Ok;
}
void* yang_run_rtcdtls_thread(void *obj) {
	YangRtcDtls *dtls = (YangRtcDtls*) obj;
	yang_run_rtcdtls_app(dtls);
	return NULL;
}
void yang_start_rtcdtls(YangRtcDtls *dtls) {
	if (!dtls)
		return;
	if (pthread_create(&dtls->threadId, 0, yang_run_rtcdtls_thread, dtls)) {
		yang_error("YangThread::start could not start thread");

	}
}
void yang_stop_rtcdtls(YangRtcDtls *dtls) {
	if (!dtls)
		return;
	dtls->isLoop = 0;
	while (dtls->isStart)
		yang_usleep(1000);
}

SSL_CTX* yang_build_dtls_ctx(YangRtcDtls *dtls, char *role) {
	SSL_CTX *dtls_ctx;
#if OPENSSL_VERSION_NUMBER < 0x10002000L // v1.0.2
    m_sslctx = SSL_CTX_new(DTLSv1_method());
#else
	if (dtls->version == YangDtlsVersion1_2) {
		if (strcmp(role, "active") == 0) {
			dtls_ctx = SSL_CTX_new(DTLS_client_method());
		} else {
			dtls_ctx = SSL_CTX_new(DTLS_server_method());
		}
	} else {
		// YangDtlsVersionAuto, use version-flexible DTLS methods
		dtls_ctx = SSL_CTX_new(DTLS_method());
	}
#endif

	if (dtls->cer->ecdsa_mode) { // By ECDSA, https://stackoverflow.com/a/6006898
#if OPENSSL_VERSION_NUMBER >= 0x10002000L // v1.0.2
		// For ECDSA, we could set the curves list.
		// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set1_curves_list.html
		SSL_CTX_set1_curves_list(dtls_ctx, "P-521:P-384:P-256");
#endif

		// For openssl <1.1, we must set the ECDH manually.
		// @see https://stackoverrun.com/cn/q/10791887
#if OPENSSL_VERSION_NUMBER < 0x10100000L // v1.1.x
    #if OPENSSL_VERSION_NUMBER < 0x10002000L // v1.0.2
        SSL_CTX_set_tmp_ecdh(m_sslctx, _srs_rtc_dtls_certificate->get_ecdsa_key());
    #else
        SSL_CTX_set_ecdh_auto(m_sslctx, 1);
    #endif
#endif
	}

	// Setup DTLS context.
	if (true) {
		//yang_assert(SSL_CTX_set_cipher_list(dtls_ctx, "ALL") == 1);
		if (SSL_CTX_set_cipher_list(dtls_ctx, "ALL") != 1)
			yang_error("SSL_CTX_set_cipher_list error");
		//yang_assert(SSL_CTX_use_certificate(dtls_ctx,m_cer->get_cert()) == 1);
		if (SSL_CTX_use_certificate(dtls_ctx, dtls->cer->dtls_cert) != 1)
			yang_error("SSL_CTX_use_certificate error");
		//yang_assert(SSL_CTX_use_PrivateKey(dtls_ctx,	m_cer->get_public_key()) == 1);
		if (SSL_CTX_use_PrivateKey(dtls_ctx, dtls->cer->dtls_pkey) != 1)
			yang_error("SSL_CTX_use_PrivateKey error");

		SSL_CTX_set_verify(dtls_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE,
				yang_verify_callback);
		SSL_CTX_set_verify_depth(dtls_ctx, 4);
		SSL_CTX_set_read_ahead(dtls_ctx, 1);
		//yang_assert(SSL_CTX_set_tlsext_use_srtp(dtls_ctx, "SRTP_AES128_CM_SHA1_80") == 0);
		if (SSL_CTX_set_tlsext_use_srtp(dtls_ctx, "SRTP_AES128_CM_SHA1_80")
				!= 0)
			yang_error("SSL_CTX_set_tlsext_use_srtp error");
	}

	return dtls_ctx;
}
int32_t yang_on_handshake_done(YangRtcDtls *dtls) {
	int32_t err = Yang_Ok;

	// Ignore if done.
	if (dtls->state == YangDtlsStateClientDone) {
		return err;
	}

	// Change to done state.
	dtls->state = YangDtlsStateClientDone;
	// When handshake done, stop the ARQ.
	// stop();
	dtls->isLoop = 0;
	char send_key[50];
	char recv_key[50];
	int sendKeyLen = 30;
	int recvKeyLen = 30;
	if ((err = yang_get_srtp_key(dtls, recv_key, &recvKeyLen, send_key,
			&sendKeyLen)) != Yang_Ok) {
		return err;
	}

	if ((err = yang_init_srtp(dtls->srtp, recv_key, recvKeyLen, send_key,
			sendKeyLen)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp init");
	}

	return err;
}
int32_t yang_doHandshake(YangRtcDtls *dtls) {
	if (dtls->handshake_done) {
		return 0;
	}

	int32_t r0 = SSL_do_handshake(dtls->ssl);
	int32_t r1 = SSL_get_error(dtls->ssl, r0);

	// Fatal SSL error, for example, no available suite when peer is DTLS 1.0 while we are DTLS 1.2.
	if (r0 < 0
			&& (r1 != SSL_ERROR_NONE && r1 != SSL_ERROR_WANT_READ
					&& r1 != SSL_ERROR_WANT_WRITE)) {
		yang_trace("\nERROR_RTC_DTLS, handshake r0=%d, r1=%d\n", r0, r1);
		return 1;
	}

	if (r1 == SSL_ERROR_NONE) {
		dtls->handshake_done = true;
		yang_trace("\ndtls handshake is sucess\n");
	}
	uint8_t *data = NULL;
	int32_t size = BIO_get_mem_data(dtls->bioOut, (char** )&data);

	yang_filter_data(dtls, data, size);

	if (size > 0)
		yang_rtc_sendData(dtls->udp, (char*) data, size);



	if (dtls->handshake_done) {
		yang_on_handshake_done(dtls);

	}

	return Yang_Ok;
}
int32_t yang_startHandShake(YangRtcDtls *dtls) {
	if (yang_doHandshake(dtls))
		return 1;
	yang_start_rtcdtls(dtls);
	return Yang_Ok;
}
int32_t yang_decodeHandshake(YangRtcDtls *dtls, char *data, int32_t nb_data) {
	int32_t r0 = 0;

	if ((r0 = BIO_reset(dtls->bioIn)) != 1) {
		yang_error("ERROR_OpenSslBIOReset, BIO_in reset r0=%d", r0);
		return 1;
	}


	if ((r0 = BIO_reset(dtls->bioOut)) != 1) {
		yang_error("ERROR_OpenSslBIOReset, BIO_out reset r0=%d", r0);
		return 1;
	}

	// Trace the detail of DTLS packet.

	if ((r0 = BIO_write(dtls->bioIn, data, nb_data)) <= 0) {

		printf("\nERROR_OpenSslBIOWrite, BIO_write r0=%d", r0);
		return 1;
	}

	int32_t err = 0;
	if ((err = yang_doHandshake(dtls) != Yang_Ok)) {
		return 1;
	}

	for (int32_t i = 0; i < 1024 && BIO_ctrl_pending(dtls->bioIn) > 0; i++) {
		char buf[8092];
		int32_t r0 = SSL_read(dtls->ssl, buf, sizeof(buf));
		int32_t r1 = SSL_get_error(dtls->ssl, r0);
		if (r0 <= 0) {

			if (r1 != SSL_ERROR_WANT_READ && r1 != SSL_ERROR_WANT_WRITE) {
				break;
			}

			uint8_t *data = NULL;
			int32_t size = BIO_get_mem_data(dtls->bioOut, (char** )&data);
			if (size > 0 && dtls->udp)
				yang_rtc_sendData(dtls->udp, (char*) data, size);
			continue;
		}

	}

	return Yang_Ok;
}
int32_t yang_sendDtlsAlert(YangRtcDtls *dtls) {
	if (!dtls)
		return 1;
	int32_t r0 = SSL_shutdown(dtls->ssl);
	int32_t r1 = SSL_get_error(dtls->ssl, r0);
	// Fatal SSL error, for example, no available suite when peer is DTLS 1.0 while we are DTLS 1.2.
	if (r0 < 0
			&& (r1 != SSL_ERROR_NONE && r1 != SSL_ERROR_WANT_READ
					&& r1 != SSL_ERROR_WANT_WRITE)) {
		printf("\nERROR_RTC_DTLS, handshake r0=%d, r1=%d\n", r0, r1);
		return 1;
	}

	uint8_t *data = NULL;
	int32_t size = BIO_get_mem_data(dtls->bioOut, (char** )&data);
	if (size > 0 && dtls->udp)
		yang_rtc_sendData(dtls->udp, (char*) data, size);

	return Yang_Ok;
}
int32_t yang_filter_data(YangRtcDtls *dtls, uint8_t *data, int32_t size) {
	int32_t err = Yang_Ok;

	if (dtls->state == YangDtlsStateInit && size > 14 && data[0] == 22
			&& data[13] == 1) {
		dtls->state = YangDtlsStateClientHello;
		return err;
	}

	if (dtls->state == 1 && size > 14 && data[0] == 22 && data[13] == 11) {
		dtls->state = YangDtlsStateClientCertificate;
		dtls->reset_timer_ = true;
		yang_trace("\nDTLS: Reset the timer for ServerHello");
		return err;
	}

	return err;
}

#define SRTP_MASTER_KEY_KEY_LEN  16
#define SRTP_MASTER_KEY_SALT_LEN  14
int32_t yang_get_srtp_key(YangRtcDtls *dtls, char *recv_key, int *precvkeylen,
		char *send_key, int *psendkeylen) {
	int32_t err = Yang_Ok;

	uint8_t material[SRTP_MASTER_KEY_LEN * 2] = { 0 }; // client(SRTP_MASTER_KEY_KEY_LEN + SRTP_MASTER_KEY_SALT_LEN) + server
	static const char *dtls_srtp_lable = "EXTRACTOR-dtls_srtp";
	if (!SSL_export_keying_material(dtls->ssl, material, sizeof(material),
			dtls_srtp_lable, strlen(dtls_srtp_lable), NULL, 0, 0)) {
		return yang_error_wrap(ERROR_RTC_SRTP_INIT, "SSL export key r0=%lu",
				ERR_get_error());
	}

	size_t offset = 0;

	memcpy(send_key, material, SRTP_MASTER_KEY_KEY_LEN);
	offset += SRTP_MASTER_KEY_KEY_LEN;

	memcpy(recv_key, material + offset, SRTP_MASTER_KEY_KEY_LEN);
	offset += SRTP_MASTER_KEY_KEY_LEN;

	memcpy(send_key + SRTP_MASTER_KEY_KEY_LEN, material + offset,
			SRTP_MASTER_KEY_SALT_LEN);
	offset += SRTP_MASTER_KEY_SALT_LEN;

	memcpy(recv_key + SRTP_MASTER_KEY_KEY_LEN, material + offset,
			SRTP_MASTER_KEY_SALT_LEN);

	*precvkeylen = SRTP_MASTER_KEY_KEY_LEN + SRTP_MASTER_KEY_SALT_LEN;
	*psendkeylen = SRTP_MASTER_KEY_KEY_LEN + SRTP_MASTER_KEY_SALT_LEN;
	return err;
}

