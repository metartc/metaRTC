#ifndef SRC_YANGWEBRTC_YANGRTCSESSIONI_H_
#define SRC_YANGWEBRTC_YANGRTCSESSIONI_H_
#include <yangrtp/YangRtpNackForReceiver.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangSRtp.h>
class YangRtcSessionI {
public:
	YangRtcSessionI(){};
	virtual ~YangRtcSessionI(){};
	virtual int32_t check_send_nacks(YangRtpNackForReceiver* nack, uint32_t ssrc, uint32_t& sent_nacks, uint32_t& timeout_nacks)=0;
	virtual int32_t send_rtcp_rr(uint32_t ssrc, YangRtpPlayNackBuffer* rtp_queue, const uint64_t& last_send_systime, const YangNtp& last_send_ntp)=0;
	virtual int32_t send_rtcp_xr_rrtr(uint32_t ssrc)=0;
	virtual int32_t send_rtcp(char *data, int32_t nb_data)=0;
	//virtual int32_t send_packet(YangRtpPacket* pkt)=0;
	virtual int32_t send_nackpacket(char* data,int32_t nb)=0;
	virtual int32_t send_avpacket(YangRtpPacket* pkt,YangBuffer* pbuf)=0;
	virtual int32_t send_rtcp_fb_pli(uint32_t ssrc)=0;
	virtual void do_request_keyframe(uint32_t ssrc)=0;
	virtual YangSRtp* get_srtp()=0;
};



#endif /* SRC_YANGWEBRTC_YANGRTCSESSIONI_H_ */
