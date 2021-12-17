#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>
#include <yangutil/sys/YangSRtp.h>
#ifdef _WIN32
#include <openssl/err.h>

#define bzero(a, b) memset(a, 0, b)
#else
#include <openssl/err.h>
#endif
YangSRtp::YangSRtp()
{
    m_recvCtx = NULL;
    m_sendCtx = NULL;
}

YangSRtp::~YangSRtp()
{
    if (m_recvCtx) {
        srtp_dealloc(m_recvCtx);
    }

    if (m_sendCtx) {
        srtp_dealloc(m_sendCtx);
    }
}

int32_t YangSRtp::init(char* recv_key,int precvkeylen, char* send_key,int psendkeylen)
{
    int32_t err = Yang_Ok;
     srtp_policy_t policy;

    bzero(&policy, sizeof(policy));

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
    uint8_t *skey = new uint8_t[psendkeylen];
    YangAutoFreeA(uint8_t, skey);
    memcpy(skey, send_key, psendkeylen);
    policy.key = skey;

    if ((r0 = srtp_create(&m_sendCtx, &policy)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_INIT, "srtp create send r0=%u", r0);
    }

    // init recv context
    policy.ssrc.type = ssrc_any_inbound;
    uint8_t *rkey = new uint8_t[precvkeylen];
    YangAutoFreeA(uint8_t, rkey);
    memcpy(rkey, recv_key, precvkeylen);
    policy.key = rkey;


    if ((r0 = srtp_create(&m_recvCtx, &policy)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_INIT, "srtp create recv r0=%u", r0);
    }


    return err;
}

int32_t YangSRtp::enc_rtp(void* packet, int* nb_cipher)
{
    int32_t err = Yang_Ok;

    // If DTLS/SRTP is not ready, fail.
    if (!m_sendCtx) {
        return yang_error_wrap(ERROR_RTC_SRTP_PROTECT, "not ready");
    }

    srtp_err_status_t r0 = srtp_err_status_ok;
    if ((r0 = srtp_protect(m_sendCtx, packet, nb_cipher)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_PROTECT, "rtp protect r0=%u", r0);
    }

    return err;
}

int32_t YangSRtp::enc_rtcp(void* packet, int* nb_cipher)
{
    int32_t err = Yang_Ok;

    // If DTLS/SRTP is not ready, fail.
    if (!m_sendCtx) {
        return yang_error_wrap(ERROR_RTC_SRTP_PROTECT, "not ready");
    }

    srtp_err_status_t r0 = srtp_err_status_ok;
    if ((r0 = srtp_protect_rtcp(m_sendCtx, packet, nb_cipher)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_PROTECT, "rtcp protect r0=%u", r0);
    }

    return err;
}

int32_t YangSRtp::dec_rtp(void* packet, int* nb_plaintext)
{
    int32_t err = Yang_Ok;

    // If DTLS/SRTP is not ready, fail.
    if (!m_recvCtx) {
        return yang_error_wrap(ERROR_RTC_SRTP_UNPROTECT, "not ready");
    }

    srtp_err_status_t r0 = srtp_err_status_ok;
    if ((r0 = srtp_unprotect(m_recvCtx, packet, nb_plaintext)) != srtp_err_status_ok) {
        if(r0==srtp_err_status_replay_fail) return r0;
        return yang_error_wrap(ERROR_RTC_SRTP_UNPROTECT, "rtp unprotect r0=%u", r0);
    }

    return err;
}

int32_t YangSRtp::dec_rtcp(void* packet, int* nb_plaintext)
{
    int32_t err = Yang_Ok;

    // If DTLS/SRTP is not ready, fail.
    if (!m_recvCtx) {
        return yang_error_wrap(ERROR_RTC_SRTP_UNPROTECT, "not ready");
    }

    srtp_err_status_t r0 = srtp_err_status_ok;
    if ((r0 = srtp_unprotect_rtcp(m_recvCtx, packet, nb_plaintext)) != srtp_err_status_ok) {
        return yang_error_wrap(ERROR_RTC_SRTP_UNPROTECT, "rtcp unprotect r0=%u", r0);
    }

    return err;
}

