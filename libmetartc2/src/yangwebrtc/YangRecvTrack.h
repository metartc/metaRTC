#ifndef SRC_YANGWEBRTC_YANGRECVTRACK_H_
#define SRC_YANGWEBRTC_YANGRECVTRACK_H_
#include <stdint.h>
#include <yangstream/YangStreamType.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRingBuffer.h>
#include <yangrtp/YangRtpNackForReceiver.h>
#include <yangrtp/YangRtpPlayNackBuffer.h>
#include <yangrtp/YangMixQueue.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangwebrtc/YangRtcContext.h>
#include <yangwebrtc/YangRtcSessionI.h>
#include <yangrtp/YangRtpFUAPayload2.h>

class YangRecvTrack {
protected:
	YangRtcTrack *m_track_desc;
public:
	YangRtcSessionI *m_session;
	//YangRtpRingBuffer *m_rtp_queue;
	YangRtpPlayNackBuffer* m_rtp_queue;
	YangRtpNackForReceiver *m_nack_receiver;

protected:
	// send report ntp and received time.
	int32_t m_uid;
	uint16_t m_last_seq;
	// Latest sender report ntp and rtp time.
	YangNtp m_last_sender_report_ntp;
	int64_t m_last_sender_report_rtp_time;

	// Prev sender report ntp and rtp time.
	YangNtp m_last_sender_report_ntp1;
	int64_t m_last_sender_report_rtp_time1;
	//YangNtp m_last_sender_report_ntp;
	uint64_t m_last_sender_report_sys_time;

	YangRtpBuffer *m_rtpBuffer;
	YangRtcContext *m_conf;
	YangFrame m_audioFrame;
	//YangFrame m_videoFrame;
	//bool m_usingMixav;
public:
	YangRecvTrack(int32_t uid, YangRtcContext *pcontext,
			YangRtcSessionI *session, YangRtcTrack *track_desc,
			YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque, bool is_audio);
	virtual ~YangRecvTrack();

	virtual void setReceiveCallback(YangReceiveCallback *cbk)=0;
public:

	bool has_ssrc(uint32_t ssrc);
	uint32_t get_ssrc();
	void update_rtt(int32_t rtt);
	void update_send_report_time(const YangNtp &ntp, uint32_t rtp_time);
	int64_t cal_avsync_time(uint32_t rtp_time);
	int32_t send_rtcp_rr();
	int32_t send_rtcp_xr_rrtr();
	std::string get_track_id();
public:
	// Note that we can set the pkt to NULL to avoid copy, for example, if the NACK cache the pkt and
	// set to NULL, nack nerver copy it but set the pkt to NULL.
	int32_t on_nack(YangRtpPacket *ppkt);
public:
	virtual int32_t on_rtp(YangRtpPacket *pkt) = 0;
	virtual int32_t check_send_nacks() = 0;
protected:
	virtual int32_t do_check_send_nacks(uint32_t &timeout_nacks);
	void on_audio_data(YangMessage *msg);
	void on_video_data(YangMessage *msg);
	//YangReceiveCallback *m_recvcb;
	//YangMixQueue *m_mixQueue;
};
#endif /* SRC_YANGWEBRTC_YANGRECVTRACK_H_ */
