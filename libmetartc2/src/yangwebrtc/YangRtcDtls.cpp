#include <yangwebrtc/YangRtcDtls.h>


#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangSsrc.h>

#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yangavinfotype.h>

#ifdef _MSC_VER
#include <winsock2.h>
#endif
#include <string.h>

#ifdef _WIN32
#include <openssl/err.h>

#define bzero(a, b) memset(a, 0, b)
#else
#include <openssl/err.h>
#endif
int32_t yang_verify_callback(int32_t preverify_ok, X509_STORE_CTX *ctx) {
	// Always OK, we don't check the certificate of client,
	// because we allow client self-sign certificate.

	return 1;
}
void ssl_on_info(const SSL* dtls, int32_t where, int32_t ret)
{
	YangRtcDtls* dtls_impl = (YangRtcDtls*)SSL_get_ex_data(dtls, 0);
    const char* method;
    int32_t w = where& ~SSL_ST_MASK;
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
        method = (where & SSL_CB_READ) ? "read":"write";

        // @see https://www.openssl.org/docs/man1.0.2/man3/SSL_alert_type_string_long.html
        string alert_type = SSL_alert_type_string_long(ret);
        string alert_desc = SSL_alert_desc_string(ret);
        // Notify the DTLS to handle the ALERT message, which maybe means media connection disconnect.
        dtls_impl->on_dtls_alert(alert_type, alert_desc);
    }
}
uint32_t  dtls_timer_cb(SSL *dtls, uint32_t  previous_us) {
	YangRtcDtls *dtls_impl = (YangRtcDtls*) SSL_get_ex_data(dtls, 0);
    if(!dtls_impl) return yang_error_wrap(ERROR_RTC_DTLS,"YangRtcDtls is null");

	// Double the timeout. Note that it can be 0.
	uint32_t  timeout_us = previous_us * 2;

	// If previous_us is 0, for example, the HelloVerifyRequest, we should response it ASAP.
	// When got ServerHello, we should reset the timer.
	if (previous_us == 0 || dtls_impl->should_reset_timer()) {
		timeout_us = 50 * 1000; // in us
	}
	// Never exceed the max timeout.
	timeout_us = yang_min(timeout_us, 30 * 1000 * 1000); // in us
	return timeout_us;
}

YangRtcDtls::YangRtcDtls(YangContext* pcontext) {
	m_context=pcontext;
	m_sslctx = NULL;
	m_ssl = NULL;
	m_bioIn = NULL;
	m_bioOut = NULL;
	m_version = YangDtlsVersionAuto;
	m_cer=NULL;
	reset_timer_=false;
	m_udp=NULL;
	m_handshake_done=false;
	m_isStart=0;
	m_loop=0;
	m_state=YangDtlsStateInit;
	 m_srtp = NULL;
}

YangRtcDtls::~YangRtcDtls() {

	if (m_sslctx) {
		SSL_CTX_free(m_sslctx);
		m_sslctx = NULL;
	}

	if (m_ssl) {
		// this function will free bio_in and bio_out
		SSL_free(m_ssl);
		m_ssl = NULL;
	}

	if(m_srtp ){
		delete m_srtp;
		m_srtp = NULL;
	}
	m_cer=NULL;
	m_udp=NULL;
	m_context=NULL;
}
YangSRtp* YangRtcDtls::getSrtp(){
	return m_srtp;
}
int32_t YangRtcDtls::init(YangSendUdpData* pudp) {

	m_udp=pudp;

	m_cer=dynamic_cast<YangCertificateImpl*>(m_context->getCertificate());

	string role = "actpass";
	m_sslctx = yang_build_dtls_ctx(m_version, role);

	if ((m_ssl = SSL_new(m_sslctx)) == NULL) {
		printf("\nERROR_OpenSslCreateSSL, SSL_new dtls");
		return 1;
	}

	SSL_set_ex_data(m_ssl, 0, this);
	SSL_set_info_callback(m_ssl, ssl_on_info);
	SSL_set_options(m_ssl, SSL_OP_NO_QUERY_MTU);
	SSL_set_mtu(m_ssl, kRtpPacketSize);
	SSL_set_connect_state(m_ssl);//SSL_set_accept_state(dtls);
	SSL_set_max_send_fragment(m_ssl, kRtpPacketSize);
#if OPENSSL_VERSION_NUMBER >= 0x1010102fL // 1.1.1b
	DTLS_set_timer_cb(m_ssl, dtls_timer_cb);
#endif

	if ((m_bioIn = BIO_new(BIO_s_mem())) == NULL) {
		printf("\nERROR_OpenSslBIONew, BIO_new in");
		return 1;
	}

	if ((m_bioOut = BIO_new(BIO_s_mem())) == NULL) {
		BIO_free(m_bioIn);
		printf("\nERROR_OpenSslBIONew, BIO_new out");
		return 1;
	}

	SSL_set_bio(m_ssl, m_bioIn, m_bioOut);
	m_srtp = new YangSRtp();

	return Yang_Ok;
}
bool YangRtcDtls::should_reset_timer() {
	bool v = reset_timer_;
	 reset_timer_ = false;
	return v;
}

int32_t YangRtcDtls::sendDtlsAlert(){

	int32_t r0 = SSL_shutdown(m_ssl);
	int32_t r1 = SSL_get_error(m_ssl, r0);
		// Fatal SSL error, for example, no available suite when peer is DTLS 1.0 while we are DTLS 1.2.
		if (r0 < 0	&& (r1 != SSL_ERROR_NONE && r1 != SSL_ERROR_WANT_READ
						&& r1 != SSL_ERROR_WANT_WRITE)) {
			printf("\nERROR_RTC_DTLS, handshake r0=%d, r1=%d\n", r0, r1);
			return 1;
		}

	 	uint8_t *data = NULL;
		int32_t size = BIO_get_mem_data(m_bioOut, (char** )&data);
		if(size>0&&m_udp) m_udp->sendData((char*)data,size);

		return Yang_Ok;
}
int32_t YangRtcDtls::on_dtls_alert(std::string type, std::string desc){
    yang_info("dtls alert ..type==%s,desc==%s",type.c_str(),desc.c_str());
	return Yang_Ok;
}
int32_t YangRtcDtls::decodeHandshake(char *data, int32_t nb_data) {

	int32_t r0 = 0;

	if ((r0 = BIO_reset(m_bioIn)) != 1) {
		yang_error("ERROR_OpenSslBIOReset, BIO_in reset r0=%d", r0);
		return 1;
	}
	if ((r0 = BIO_reset(m_bioOut)) != 1) {
		yang_error("ERROR_OpenSslBIOReset, BIO_out reset r0=%d", r0);
		return 1;
	}

	// Trace the detail of DTLS packet.

	if ((r0 = BIO_write(m_bioIn, data, nb_data)) <= 0) {

		printf("\nERROR_OpenSslBIOWrite, BIO_write r0=%d", r0);
		return 1;
	}

	int32_t err = 0;
	if((err=this->doHandshake()!=Yang_Ok)){
		return 1;
	}

	for (int32_t i = 0; i < 1024 && BIO_ctrl_pending(m_bioIn) > 0; i++) {
		char buf[8092];
		int32_t r0 = SSL_read(m_ssl, buf, sizeof(buf));
		int32_t r1 = SSL_get_error(m_ssl, r0);
		   if (r0 <= 0) {

			if (r1 != SSL_ERROR_WANT_READ && r1 != SSL_ERROR_WANT_WRITE) {
				break;
			}

			uint8_t *data = NULL;
			int32_t size = BIO_get_mem_data(m_bioOut, (char** )&data);
			if(size>0&&m_udp) m_udp->sendData((char*)data,size);
			continue;
		}


	}

	return Yang_Ok;
}
int32_t YangRtcDtls::startHandShake(){
	if(doHandshake()) return 1;
	start();
	return Yang_Ok;
}

int32_t YangRtcDtls::doHandshake() {
	   if (m_handshake_done) {
	        return 0;
	    }

	int32_t r0 = SSL_do_handshake(m_ssl);
	int32_t r1 = SSL_get_error(m_ssl, r0);

	// Fatal SSL error, for example, no available suite when peer is DTLS 1.0 while we are DTLS 1.2.
	if (r0 < 0	&& (r1 != SSL_ERROR_NONE && r1 != SSL_ERROR_WANT_READ
					&& r1 != SSL_ERROR_WANT_WRITE)) {
		yang_trace("\nERROR_RTC_DTLS, handshake r0=%d, r1=%d\n", r0, r1);
		return 1;
	}

    if (r1 == SSL_ERROR_NONE) {
        m_handshake_done = true;
        yang_trace("\n******************dtls handshake is sucess....................\n");
    }
 	uint8_t *data = NULL;
	int32_t size = BIO_get_mem_data(m_bioOut, (char** )&data);

	filter_data(data, size);

	if(size>0&&m_udp) m_udp->sendData((char*)data,size);

	if(m_handshake_done){
		on_handshake_done();

	}

	return Yang_Ok;
}
int32_t YangRtcDtls::filter_data(uint8_t* data, int32_t size)
{
    int32_t err = Yang_Ok;

    if (m_state==YangDtlsStateInit&&size > 14 && data[0] == 22 && data[13] == 1) {
    	 m_state=YangDtlsStateClientHello;
        return err;
    }

    if (m_state==1&&size > 14 && data[0] == 22 && data[13] == 11) {
    	 m_state=YangDtlsStateClientCertificate;
        reset_timer_ = true;
        yang_trace("\nDTLS: Reset the timer for ServerHello");
        return err;
    }

    return err;
}
const int32_t SRTP_MASTER_KEY_KEY_LEN = 16;
const int32_t SRTP_MASTER_KEY_SALT_LEN = 14;
int32_t YangRtcDtls::get_srtp_key(char* recv_key, int *precvkeylen,char* send_key,int *psendkeylen){
	int32_t err = Yang_Ok;

	    uint8_t material[SRTP_MASTER_KEY_LEN * 2] = {0};  // client(SRTP_MASTER_KEY_KEY_LEN + SRTP_MASTER_KEY_SALT_LEN) + server
	    static const string dtls_srtp_lable = "EXTRACTOR-dtls_srtp";
	    if (!SSL_export_keying_material(m_ssl, material, sizeof(material), dtls_srtp_lable.c_str(), dtls_srtp_lable.size(), NULL, 0, 0)) {
	        return yang_error_wrap(ERROR_RTC_SRTP_INIT, "SSL export key r0=%lu", ERR_get_error());
	    }

	    size_t offset = 0;


        memcpy(send_key,material,SRTP_MASTER_KEY_KEY_LEN);
	    offset += SRTP_MASTER_KEY_KEY_LEN;

        memcpy(recv_key,material+offset,SRTP_MASTER_KEY_KEY_LEN);
	    offset += SRTP_MASTER_KEY_KEY_LEN;

        memcpy(send_key+SRTP_MASTER_KEY_KEY_LEN,material+offset,SRTP_MASTER_KEY_SALT_LEN);
	    offset += SRTP_MASTER_KEY_SALT_LEN;

        memcpy(recv_key+SRTP_MASTER_KEY_KEY_LEN,material+offset,SRTP_MASTER_KEY_SALT_LEN);


        *precvkeylen=SRTP_MASTER_KEY_KEY_LEN+SRTP_MASTER_KEY_SALT_LEN;
        *psendkeylen=SRTP_MASTER_KEY_KEY_LEN+SRTP_MASTER_KEY_SALT_LEN;
	    return err;

}

YangDtlsState YangRtcDtls::getDtlsState(){
	return m_state;
}

int32_t YangRtcDtls::on_handshake_done()
{
    int32_t err = Yang_Ok;

    // Ignore if done.
    if (m_state == YangDtlsStateClientDone) {
        return err;
    }

    // Change to done state.
    m_state = YangDtlsStateClientDone;
    // When handshake done, stop the ARQ.
    stop();
    char send_key[50];
     char recv_key[50];
     int sendKeyLen=30;
     int recvKeyLen=30;
     if ((err = get_srtp_key(recv_key, &recvKeyLen,send_key,&sendKeyLen)) != Yang_Ok) {
          return err;
      }

      if ((err = m_srtp->init(recv_key,recvKeyLen, send_key,sendKeyLen)) != Yang_Ok) {
          return yang_error_wrap(err, "srtp init");
      }

    return err;
}

void YangRtcDtls::run() {
	m_isStart = 1;
    startLoop();
	m_isStart = 0;
}
void YangRtcDtls::stop() {
	stopLoop();

}
void YangRtcDtls::stopLoop() {
	m_loop = 0;

}
#define YANG_UTIME_MILLISECONDS 1000
int32_t YangRtcDtls::startLoop()
{
    int32_t err = Yang_Ok;
    const int32_t max_loop = 512;
    int32_t arq_count = 0;
   int32_t arq_max_retry = 12 * 2;
    for (int32_t i = 0; arq_count < arq_max_retry && i < max_loop; i++) {
         if (m_handshake_done) {
            return err;
        }

        // For DTLS client ARQ, the state should be specified.
        if (m_state != YangDtlsStateClientHello && m_state != YangDtlsStateClientCertificate) {
            return err;
        }

        // If there is a timeout in progress, it sets *out to the time remaining
        // and returns one. Otherwise, it returns zero.
        int32_t r0 = 0; timeval to = {0};
        if ((r0 = DTLSv1_get_timeout(m_ssl, &to)) == 0) {
            // No timeout, for example?, wait for a default 50ms.
            yang_usleep(50 * YANG_UTIME_MILLISECONDS);
            continue;
        }
        int64_t timeout = to.tv_sec + to.tv_usec;


        if (timeout > 0) {
            timeout = yang_min(100 * YANG_UTIME_MILLISECONDS, timeout);
            timeout = yang_max(50 * YANG_UTIME_MILLISECONDS, timeout);
            yang_usleep((unsigned int)timeout);
            continue;
        }

        r0 = BIO_reset(m_bioOut); int32_t r1 = SSL_get_error(m_ssl, r0);
        if (r0 != 1) {
        	return yang_error_wrap(ERROR_OpenSslBIOReset,"OpenSslBIORese BIO_reset r0=%d, r1=%d", r0, r1);
        }

       r0 = DTLSv1_handle_timeout(m_ssl); r1 = SSL_get_error(m_ssl, r0);
        if (r0 == 0) {
            continue; // No timeout had expired.
        }
        if (r0 != 1) {
        	return yang_error_wrap(ERROR_RTC_DTLS,"dtls error ARQ r0=%d, r1=%d", r0, r1);

        }


        uint8_t* data = NULL;
        int32_t size = BIO_get_mem_data(m_bioOut, (char**)&data);
        arq_count++;
        if (size > 0 && (err = m_udp->sendData((char*)data, size)) != Yang_Ok) {
             return yang_error_wrap(ERROR_RTC_DTLS,"error dtls send size=%u", size);
           }
    }

    return err;
}


SSL_CTX* YangRtcDtls::yang_build_dtls_ctx(YangDtlsVersion version, std::string role) {
	SSL_CTX *dtls_ctx;
#if OPENSSL_VERSION_NUMBER < 0x10002000L // v1.0.2
    m_sslctx = SSL_CTX_new(DTLSv1_method());
#else
 if (version == YangDtlsVersion1_2) {
		if (role == "active") {
			dtls_ctx = SSL_CTX_new(DTLS_client_method());
		} else {
			dtls_ctx = SSL_CTX_new(DTLS_server_method());
		}
	} else {
		// YangDtlsVersionAuto, use version-flexible DTLS methods
		dtls_ctx = SSL_CTX_new(DTLS_method());
	}
#endif

	if (m_cer->is_ecdsa()) { // By ECDSA, https://stackoverflow.com/a/6006898
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
        if(SSL_CTX_set_cipher_list(dtls_ctx, "ALL") != 1) yang_error("SSL_CTX_set_cipher_list error");
        //yang_assert(SSL_CTX_use_certificate(dtls_ctx,m_cer->get_cert()) == 1);
        if(SSL_CTX_use_certificate(dtls_ctx,m_cer->get_cert()) != 1) yang_error("SSL_CTX_use_certificate error");
        //yang_assert(SSL_CTX_use_PrivateKey(dtls_ctx,	m_cer->get_public_key()) == 1);
        if(SSL_CTX_use_PrivateKey(dtls_ctx,	m_cer->get_public_key()) != 1)  yang_error("SSL_CTX_use_PrivateKey error");


		SSL_CTX_set_verify(dtls_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE,
				yang_verify_callback);
		SSL_CTX_set_verify_depth(dtls_ctx, 4);
		SSL_CTX_set_read_ahead(dtls_ctx, 1);
        //yang_assert(SSL_CTX_set_tlsext_use_srtp(dtls_ctx, "SRTP_AES128_CM_SHA1_80") == 0);
        if(SSL_CTX_set_tlsext_use_srtp(dtls_ctx, "SRTP_AES128_CM_SHA1_80") != 0)  yang_error("SSL_CTX_set_tlsext_use_srtp error");
	}

	return dtls_ctx;
}


