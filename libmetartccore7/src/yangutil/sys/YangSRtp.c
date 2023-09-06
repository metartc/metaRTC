//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangSRtp.h>
#include <yangutil/sys/YangLog.h>


#if Yang_Enable_Dtls
#if Yang_OS_WIN
#define bzero(a, b) yang_memset(a, 0, b)
#endif

int32_t yang_destroy_srtp(YangSRtp* srtp){
    if (srtp->recvCtx) {
        srtp_dealloc(srtp->recvCtx);
    }

    if (srtp->sendCtx) {
        srtp_dealloc(srtp->sendCtx);
    }
    yang_thread_mutex_destroy(&srtp->rtpLock);
    yang_thread_mutex_destroy(&srtp->rtcpLock);
	return Yang_Ok;
}
int32_t yang_create_srtp(YangSRtp* srtp,char* recv_key,int precvkeylen, char* send_key,int psendkeylen)
{
    int32_t err = Yang_Ok;
     srtp_policy_t policy;


    yang_memset(&policy, 0,sizeof(policy));
    // TODO: Maybe we can use SRTP-GCM in future.
    // @see https://bugs.chromium.org/p/chromium/issues/detail?id=713701
    // @see https://groups.google.com/forum/#!topic/discuss-webrtc/PvCbWSetVAQ
    srtp_crypto_policy_set_aes_cm_128_hmac_sha1_80(&policy.rtp);
    srtp_crypto_policy_set_aes_cm_128_hmac_sha1_80(&policy.rtcp);

    policy.ssrc.value = 0;
    // TODO: adjust window_size
    policy.window_size = 8192;
    policy.allow_repeat_tx = 1;
    policy.next = NULL;
     srtp_err_status_t r0 = srtp_err_status_ok;

    //init send context
    policy.ssrc.type = ssrc_any_outbound;
    uint8_t *skey = (uint8_t *)yang_calloc(psendkeylen,1);
    yang_memcpy(skey, send_key, psendkeylen);
    policy.key = skey;

    if ((r0 = srtp_create(&srtp->sendCtx, &policy)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_INIT, "srtp create send r0=%u", r0);
    }

    // init recv context
    policy.ssrc.type = ssrc_any_inbound;
    uint8_t *rkey = (uint8_t *)yang_calloc(precvkeylen,1);
    yang_memcpy(rkey, recv_key, precvkeylen);
    policy.key = rkey;


    if ((r0 = srtp_create(&srtp->recvCtx, &policy)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_INIT, "srtp create recv r0=%u", r0);
    }

    yang_free(skey);
    yang_free(rkey);
    yang_thread_mutex_init(&srtp->rtpLock,NULL);
    yang_thread_mutex_init(&srtp->rtcpLock,NULL);
    return err;
}

int32_t yang_enc_rtp(YangSRtp* srtp,void* packet, int* nb_cipher)
{
    int32_t err = Yang_Ok;

    // If DTLS/SRTP is not ready, fail.
    if (!srtp->sendCtx) {
        return yang_error_wrap(ERROR_RTC_SRTP_PROTECT, "srtp not init");
    }

    srtp_err_status_t r0 = srtp_err_status_ok;
    yang_thread_mutex_lock(&srtp->rtpLock);
    if ((r0 = srtp_protect(srtp->sendCtx, packet, nb_cipher)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_PROTECT, "rtp protect r0=%u", r0);
    }
    yang_thread_mutex_unlock(&srtp->rtpLock);
    return err;
}

int32_t yang_enc_rtcp(YangSRtp* srtp,void* packet, int* nb_cipher)
{
    int32_t err = Yang_Ok;

    // If DTLS/SRTP is not ready, fail.
    if (!srtp->sendCtx) {
        return yang_error_wrap(ERROR_RTC_SRTP_PROTECT, "not ready");
    }

    srtp_err_status_t r0 = srtp_err_status_ok;
    yang_thread_mutex_lock(&srtp->rtcpLock);
    if ((r0 = srtp_protect_rtcp(srtp->sendCtx, packet, nb_cipher)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_PROTECT, "rtcp protect r0=%u", r0);
    }
    yang_thread_mutex_unlock(&srtp->rtcpLock);
    return err;
}

int32_t yang_dec_rtp(YangSRtp* srtp,void* packet, int* nb_plaintext)
{
    // If DTLS/SRTP is not ready, fail.
    if (!srtp->recvCtx) {
        return yang_error_wrap(ERROR_RTC_SRTP_UNPROTECT, "not ready");
    }

    srtp_err_status_t r0 = srtp_err_status_ok;
    if ((r0 = srtp_unprotect(srtp->recvCtx, packet, nb_plaintext)) != srtp_err_status_ok) {
        if(r0==srtp_err_status_replay_fail) return r0;
        return yang_error_wrap(ERROR_RTC_SRTP_UNPROTECT, "rtp unprotect r0=%u", r0);
    }

    return Yang_Ok;
}

int32_t yang_dec_rtcp(YangSRtp* srtp,void* packet, int* nb_plaintext)
{

    // If DTLS/SRTP is not ready, fail.
    if (!srtp->recvCtx) {
        return yang_error_wrap(ERROR_RTC_SRTP_UNPROTECT, "not ready");
    }

    srtp_err_status_t r0 = srtp_err_status_ok;
    if ((r0 = srtp_unprotect_rtcp(srtp->recvCtx, packet, nb_plaintext)) != srtp_err_status_ok) {
    	if (r0 == srtp_err_status_replay_fail)	return r0;
    	return yang_error_wrap(ERROR_RTC_SRTP_UNPROTECT, "rtcp unprotect r0=%u", r0);
    }

    return Yang_Ok;
}
#endif
