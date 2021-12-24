#ifndef INCLUDE_YANGUTIL_SYS_YANGSRTP_H_
#define INCLUDE_YANGUTIL_SYS_YANGSRTP_H_
#include <srtp2/srtp.h>
class YangSRtp
{
public:
    YangSRtp();
    virtual ~YangSRtp();
public:
    int32_t init(char* recv_key,int precvkeylen, char* send_key,int psendkeylen);
public:
    int32_t enc_rtp(void* packet, int* nb_cipher);
    int32_t enc_rtcp(void* packet, int* nb_cipher);
    int32_t dec_rtp(void* packet, int* nb_plaintext);
    int32_t dec_rtcp(void* packet, int* nb_plaintext);
private:
    srtp_t m_recvCtx;
    srtp_t m_sendCtx;
};
#endif /* INCLUDE_YANGUTIL_SYS_YANGSRTP_H_ */
