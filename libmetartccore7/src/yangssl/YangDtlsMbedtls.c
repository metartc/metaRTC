//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangssl/YangRtcDtls.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangLog.h>
#include <time.h>

#include <yangice/YangRtcSocket.h>
#include <yangrtc/YangDatachannel.h>


#if Yang_Enable_Dtls
#if !Yang_Enable_Openssl
#include <mbedtls/debug.h>

int32_t yang_on_handshake_done(YangDtlsSession *dtls);
int32_t yang_filter_data(YangDtlsSession *dtls, uint8_t *data, int32_t size) {
	int32_t err = Yang_Ok;

	if (dtls->state == YangDtlsStateInit && size > 14 && data[0] == 22
			&& data[13] == 1) {
		dtls->state = YangDtlsStateClientHello;
		return err;
	}

	if (dtls->state == YangDtlsStateClientHello && size > 14 && data[0] == 22 && data[13] == 11) {
		dtls->state = YangDtlsStateClientCertificate;
		dtls->reset_timer_ = yangtrue;
		yang_trace("\nDTLS: Reset the timer for ServerHello");
		return err;
	}

	return err;
}
#if Yang_Mbedtls_3
void yang_dtls_keyCallback( void *user,
		mbedtls_ssl_key_export_type type,
		const unsigned char *secret,
		size_t secret_len,
		const unsigned char client_random[32],
		const unsigned char server_random[32],
		mbedtls_tls_prf_types tls_prf_type ){
#else
int yang_dtls_keyCallback( void *user,
	                                           const unsigned char *secret,
	                                           const unsigned char *kb,
	                                           size_t maclen,
	                                           size_t keylen,
	                                           size_t ivlen,
	                                           const unsigned char client_random[32],
	                                           const unsigned char server_random[32],
	                                           mbedtls_tls_prf_types tls_prf_type ){
#endif
	YangDtlsSession* session = (YangDtlsSession*) user;

	yang_memcpy(session->masterSecret, secret, sizeof(session->masterSecret));
	yang_memcpy(session->randBytes, client_random, 32);
	yang_memcpy(session->randBytes + 32, server_random, 32);
	session->tlsPrf=tls_prf_type;
#if !Yang_Mbedtls_3
	return Yang_Ok;
#endif

}

void yang_dtls_set_timer_t( void * user,uint32_t int_ms,uint32_t fin_ms ){
	YangDtlsSession* session = (YangDtlsSession*) user;
	YangMbedTimer* mbtimer = &session->mbtimer;

	mbtimer->int_ms = int_ms * YANG_UTIME_MILLISECONDS;
	mbtimer->fin_ms = fin_ms * YANG_UTIME_MILLISECONDS;

	if (fin_ms != 0) {
		mbtimer->updatedTime = yang_get_system_time();
	}
}

int yang_dtls_get_timer_t( void * user ){
	YangDtlsSession* session = (YangDtlsSession*) user;
	YangMbedTimer* mbtimer = &session->mbtimer;
	uint64_t elapsedTime = yang_get_system_time() - mbtimer->updatedTime;

	if (mbtimer->fin_ms == 0) {
		return -1;
	} else if (elapsedTime >= mbtimer->fin_ms) {
		return 2;
	} else if (elapsedTime >= mbtimer->int_ms) {
		return 1;
	} else {
		return 0;
	}
}



int32_t yang_doHandshake(YangDtlsSession *dtls) {
	if (dtls->state == YangDtlsStateClientDone||dtls->handshake_done) {
		return Yang_Ok;
	}

	int32_t ret=mbedtls_ssl_handshake(dtls->ssl);

	if(ret==MBEDTLS_ERR_SSL_WANT_READ||ret==MBEDTLS_ERR_SSL_WANT_WRITE) return Yang_Ok;

	return Yang_Ok;
}


#define SRTP_MASTER_KEY_KEY_LEN  16
#define SRTP_MASTER_KEY_SALT_LEN  14
int32_t yang_get_srtp_key(YangDtlsSession *session, char *precv_key, int *precvkeylen,
		char *psend_key, int *psendkeylen){

	char* send_key=psend_key;
	char* recv_key=precv_key;
	if(session->isControled){
		send_key=precv_key;
		recv_key=psend_key;
	}
	size_t offset = 0;

	uint8_t keyingMaterialBuffer[SRTP_MASTER_KEY_KEY_LEN * 2 + SRTP_MASTER_KEY_SALT_LEN * 2];
	mbedtls_dtls_srtp_info negotiatedSRTPProfile;

	if(mbedtls_ssl_tls_prf(session->tlsPrf, session->masterSecret, sizeof(session->masterSecret), "EXTRACTOR-dtls_srtp", session->randBytes,
			sizeof(session->randBytes), keyingMaterialBuffer, sizeof(keyingMaterialBuffer)) != Yang_Ok){
		yang_error("\nmbedtls_ssl_tls_prf fail");

	}



	yang_memcpy(send_key, &keyingMaterialBuffer[offset], SRTP_MASTER_KEY_KEY_LEN);
	offset += SRTP_MASTER_KEY_KEY_LEN;

	yang_memcpy(recv_key, &keyingMaterialBuffer[offset], SRTP_MASTER_KEY_KEY_LEN);
	offset += SRTP_MASTER_KEY_KEY_LEN;

	yang_memcpy(send_key + SRTP_MASTER_KEY_KEY_LEN, &keyingMaterialBuffer[offset], SRTP_MASTER_KEY_SALT_LEN);
	offset += SRTP_MASTER_KEY_SALT_LEN;

	yang_memcpy(recv_key + SRTP_MASTER_KEY_KEY_LEN, &keyingMaterialBuffer[offset], SRTP_MASTER_KEY_SALT_LEN);

	mbedtls_ssl_get_dtls_srtp_negotiation_result(session->ssl, &negotiatedSRTPProfile);
#if Yang_Mbedtls_3
	switch (negotiatedSRTPProfile.private_chosen_dtls_srtp_profile) {
#else
	switch(negotiatedSRTPProfile.chosen_dtls_srtp_profile) {
#endif
	case MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80:
		session->srtpPrf = MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80;
		break;
	case MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32:
		session->srtpPrf = MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32;
		break;
	}

	*precvkeylen = SRTP_MASTER_KEY_KEY_LEN + SRTP_MASTER_KEY_SALT_LEN;
	*psendkeylen = SRTP_MASTER_KEY_KEY_LEN + SRTP_MASTER_KEY_SALT_LEN;
	return Yang_Ok;
}


int32_t yang_run_rtcdtls_app(YangDtlsSession *dtls) {
	dtls->isStart = 1;
	const int32_t max_loop = 512;
	int32_t arq_count = 0;
	int32_t arq_max_retry = 12 * 2;
	for (int32_t i = 0; arq_count < arq_max_retry && i < max_loop; i++) {
		if (dtls->handshake_done)  goto ret;
		// For DTLS client ARQ, the state should be specified.
#if Yang_Mbedtls_3
		if (dtls->ssl->private_state == MBEDTLS_SSL_HANDSHAKE_OVER)  goto ret;
#else
		if (dtls->ssl->state == MBEDTLS_SSL_HANDSHAKE_OVER)  goto ret;
#endif

		yang_usleep(50*YANG_UTIME_MILLISECONDS);
		yang_doHandshake(dtls);

		arq_count++;

	}

	ret:
	dtls->isStart = 0;
	return Yang_Ok;
}

void* yang_run_rtcdtls_thread(void *obj) {
	YangDtlsSession *dtls = (YangDtlsSession*) obj;
	yang_run_rtcdtls_app(dtls);
	return NULL;
}

void yang_start_rtcdtls(YangDtlsSession *dtls) {
	if (!dtls)		return;
	dtls->isStart=1;
	yang_doHandshake(dtls);
	if (yang_thread_create(&dtls->threadId, 0, yang_run_rtcdtls_thread, dtls)) {
		yang_error("YangThread::start could not start thread");
	}
}
void yang_stop_rtcdtls(YangDtlsSession *dtls) {
	if (!dtls)
		return;
	dtls->isLoop = 0;
	while (dtls->isStart)
		yang_usleep(1000);
}


int32_t yang_on_handshake_done(YangDtlsSession *dtls) {
	int32_t err = Yang_Ok;

	// Ignore if done.
	if (dtls->state == YangDtlsStateClientDone) 		return err;

	dtls->isLoop = 0;
	char send_key[50];
	char recv_key[50];
	int sendKeyLen = 30;
	int recvKeyLen = 30;
	if ((err = yang_get_srtp_key(dtls, recv_key, &recvKeyLen, send_key,	&sendKeyLen)) != Yang_Ok) {
		return err;
	}

	if ((err = yang_create_srtp(dtls->srtp, recv_key, recvKeyLen, send_key,	sendKeyLen)) != Yang_Ok) {
		return yang_error_wrap(err, "srtp init");
	}
	// Change to done state.
	dtls->state = YangDtlsStateClientDone;
	yang_trace("\ndtls handshake is sucess\n");

	return err;
}

int32_t yang_startHandShake(YangDtlsSession *dtls) {
	if (yang_doHandshake(dtls))		return ERROR_RTC_DTLS;
	yang_start_rtcdtls(dtls);
	return Yang_Ok;
}

int32_t yang_process_dtls_data(void* user,YangDtlsSession *dtls, char *data, int32_t nb_data) {

	if(dtls->datachannel==NULL) dtls->datachannel=user;

	int32_t ret=0, readBytes = 0;
	int32_t err=Yang_Ok;
	yangbool flag = yangtrue;

	if(dtls->isControled&&dtls->state != YangDtlsStateClientDone){
		if(dtls->isStart==0) yang_start_rtcdtls(dtls);
	}

	yang_memcpy(dtls->buffer+dtls->bufferLen,data,nb_data);
	dtls->bufferLen+=nb_data;
	// read application data
	while (flag && dtls->bufferLen>0) {
		ret = mbedtls_ssl_read(dtls->ssl, (uint8_t*)data + readBytes, (size_t)dtls->bufferLen);

		if (ret > 0) {
			readBytes += ret;
		} else if (ret == 0 || ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
			if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY){
				dtls->isRecvAlert = yangtrue;
				if(!dtls->isSendAlert)
				dtls->sslCallback->sslAlert(dtls->sslCallback->context,dtls->uid,"warning","CN");
			}
			flag = yangfalse;
		} else if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			flag = yangfalse;
		} else {
			yang_error("mbedtls_ssl_read(%d)", ret);
			readBytes = 0;
			err = ERROR_RTC_DTLS;
			flag = yangfalse;
		}
	}


#if Yang_Mbedtls_3
	if (dtls->ssl->private_state == MBEDTLS_SSL_HANDSHAKE_OVER) {
#else
	if (dtls->ssl->state == MBEDTLS_SSL_HANDSHAKE_OVER) {
#endif
		yang_on_handshake_done(dtls);
	}

	if(dtls->state == YangDtlsStateClientDone){
#if Yang_Enable_Datachannel
		YangDatachannel* datachannel=(YangDatachannel*)dtls->datachannel;
		if(datachannel&&datachannel->on_message&&readBytes>0)
			datachannel->on_message(datachannel->context,data,readBytes);
#endif
	}

	return err;
}

int32_t yang_sendDtlsAlert(YangDtlsSession *session) {
	if (!session)		return ERROR_RTC_DTLS;
	while (mbedtls_ssl_close_notify(session->ssl) == MBEDTLS_ERR_SSL_WANT_WRITE) {

	}

	return Yang_Ok;
}


int32_t yang_dtls_sendSctpData(YangDtlsSession* dtls,uint8_t* pdata, int32_t dataLen){
	int32_t err=Yang_Ok;
	int32_t writtenBytes=0,	 len = 0;
	int32_t ret=0;
	yangbool flag=yangtrue;
	while ( flag&&writtenBytes < dataLen) {
		len = yang_min(dataLen - writtenBytes, mbedtls_ssl_get_max_out_record_payload(dtls->ssl));
		ret = mbedtls_ssl_write(dtls->ssl, pdata + writtenBytes, len);
		if (ret > 0) {
			writtenBytes += ret;
		} else if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			flag = yangfalse;
		} else {
			yang_error("mbedtls_ssl_write(%d)", ret);
			writtenBytes = 0;
			err = ERROR_RTC_DTLS;
			flag = yangfalse;
		}
	}



	return err;

}

int32_t yang_mbed_sendCallback(void* user, const unsigned char* data, size_t len)
{
	YangDtlsSession* dtls = (YangDtlsSession*) user;

	if (len > 0&& dtls->sock->write(&dtls->sock->session,(char*)data, len)!= Yang_Ok) {
		return yang_error_wrap(-1, "error dtls send size=%u",len);
	}
	yang_filter_data(dtls,(uint8_t*)data,len);
	return len;
}

int32_t yang_mbed_receiveCallback(void* user, unsigned char* buf, size_t len)
{
	if(user==NULL) return MBEDTLS_ERR_SSL_WANT_READ;

	YangDtlsSession* dtls = (YangDtlsSession*) user;
	uint32_t dataLen = MBEDTLS_ERR_SSL_WANT_READ;

	if(dtls->bufferLen==0) return dataLen;

	dataLen=yang_min(dtls->bufferLen,len);
	yang_memcpy(buf,dtls->buffer,dataLen);

	dtls->bufferLen-=dataLen;
	if(dtls->bufferLen<0) dtls->bufferLen=0;
	if(dtls->bufferLen>0) yang_memmove(dtls->buffer,dtls->buffer+dataLen,dtls->bufferLen);
	return dataLen;
}

mbedtls_ssl_srtp_profile YANG_SRTP_SUPPORTED_PROFILES[] = {
		MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80,
		MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32,
		MBEDTLS_TLS_SRTP_UNSET,
};
void yang_dtls_debug(void *user, int level, const char *file, int line, const char *str){
	yang_trace("\n%s,%d:,%s",file,line,str);
}
void yang_mbed_initDtls(YangDtlsSession* session){
	yang_trace("\n dtls is mbedtls");
	mbedtls_entropy_init(session->entropy);
	mbedtls_ctr_drbg_init(session->ctrDrbg);
	mbedtls_ssl_config_init(session->sslConfig);
	mbedtls_ssl_init(session->ssl);

	mbedtls_ctr_drbg_set_prediction_resistance(session->ctrDrbg, MBEDTLS_CTR_DRBG_PR_ON);
	if(mbedtls_ctr_drbg_seed(session->ctrDrbg, mbedtls_entropy_func, session->entropy, NULL, 0) !=Yang_Ok){
		yang_error("create ssl(mbedtls_ctr_drbg_seed) fail");
	}

	// init ssl config
	if(mbedtls_ssl_config_defaults(session->sslConfig, session->isControled ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
			MBEDTLS_SSL_TRANSPORT_DATAGRAM, MBEDTLS_SSL_PRESET_DEFAULT)!=Yang_Ok){
		yang_error("create ssl(mbedtls_ssl_config_defaults) fail");
	}

	mbedtls_ssl_conf_authmode(session->sslConfig, MBEDTLS_SSL_VERIFY_OPTIONAL);
	mbedtls_ssl_conf_rng(session->sslConfig, mbedtls_ctr_drbg_random, session->ctrDrbg);

	if(mbedtls_ssl_conf_own_cert(session->sslConfig, session->cer->dtls_cert, session->cer->dtls_pkey)!=Yang_Ok){
		yang_error("mbedtls set certificate fail");
	}

	//mbedtls_ssl_conf_dbg(session->sslConfig,yang_dtls_debug,session);
	//mbedtls_debug_set_threshold(5);

	mbedtls_ssl_conf_dtls_cookies(session->sslConfig, NULL, NULL, NULL);


	if(mbedtls_ssl_conf_dtls_srtp_protection_profiles(session->sslConfig, YANG_SRTP_SUPPORTED_PROFILES)!=Yang_Ok){
		yang_error("mbedtls_ssl_conf_dtls_srtp_protection_profiles  fail");
	}
#if Yang_Mbedtls_3
	mbedtls_ssl_set_export_keys_cb(session->ssl, yang_dtls_keyCallback, session);
#else

	mbedtls_ssl_conf_export_keys_ext_cb(session->sslConfig, yang_dtls_keyCallback, session);
#endif

	if(mbedtls_ssl_setup(session->ssl, session->sslConfig) !=Yang_Ok){
		yang_error("create ssl ctx(mbedtls_ssl_setup) fail");
	}
	mbedtls_ssl_set_mtu(session->ssl, kRtpPacketSize);
	mbedtls_ssl_set_bio(session->ssl, session, yang_mbed_sendCallback, yang_mbed_receiveCallback,NULL);
	mbedtls_ssl_set_timer_cb(session->ssl, session, yang_dtls_set_timer_t, yang_dtls_get_timer_t);


}


int32_t yang_create_rtcdtls(YangRtcDtls *dtls,yangbool isControled) {
	if (!dtls)	return ERROR_RTC_DTLS;
	YangDtlsSession* session=&dtls->session;

	session->version = YangDtlsVersionAuto;
	session->reset_timer_ = yangfalse;
	session->handshake_done = yangfalse;

	session->isRecvAlert = yangfalse;
	session->isSendAlert = yangfalse;

	session->isStart = yangfalse;
	session->isLoop = yangfalse;

	session->isControled=isControled;

	session->state = YangDtlsStateInit;

	session->ssl=(mbedtls_ssl_context*)yang_calloc(sizeof(mbedtls_ssl_context),1);
	session->ctrDrbg=(mbedtls_ctr_drbg_context*)yang_calloc(sizeof(mbedtls_ctr_drbg_context),1);
	session->entropy=(mbedtls_entropy_context*)yang_calloc(sizeof(mbedtls_entropy_context),1);
	session->sslConfig=(mbedtls_ssl_config*)yang_calloc(sizeof(mbedtls_ssl_config),1);

	yang_memset(session->buffer,0,sizeof(session->buffer));

	yang_mbed_initDtls(session);

	dtls->start=yang_start_rtcdtls;
	dtls->stop=yang_stop_rtcdtls;
	dtls->doHandshake=yang_doHandshake;
	dtls->startHandShake=yang_startHandShake;

	dtls->processData=yang_process_dtls_data;

	dtls->sendDtlsAlert=yang_sendDtlsAlert;
	dtls->filterData=yang_filter_data;
	dtls->getSrtpKey=yang_get_srtp_key;
	dtls->sendSctpData=yang_dtls_sendSctpData;

	return Yang_Ok;
}

void yang_destroy_rtcdtls(YangRtcDtls *dtls) {
	if (!dtls)	return;
	YangDtlsSession* session=&dtls->session;
	if(session->entropy) mbedtls_entropy_free(session->entropy);
	if(session->ctrDrbg) mbedtls_ctr_drbg_free(session->ctrDrbg);
	if(session->sslConfig) mbedtls_ssl_config_free(session->sslConfig);
	if(session->ssl) mbedtls_ssl_free(session->ssl);
	yang_free(session->entropy);
	yang_free(session->ctrDrbg);
	yang_free(session->sslConfig);
	yang_free(session->ssl);
}

#endif
#endif
