#include <yangwebrtc/YangRecvTrack.h>
#include <yangutil/sys/YangLog.h>
#include <math.h>
YangRecvTrack::YangRecvTrack(int32_t uid, YangRtcContext *conf,
		YangRtcSessionI *session, YangRtcTrack *ptrack_desc,
		YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque, bool is_audio) {
	m_uid = uid;
	m_session = session;
	m_track_desc = ptrack_desc->copy();
	m_rtpBuffer = rtpBuffer;

	if (is_audio) {
		m_rtp_queue = new YangRtpPlayNackBuffer(100);
		m_nack_receiver = new YangRtpNackForReceiver(m_rtp_queue, 100 * 2 / 3);
	} else {
		m_rtp_queue = new YangRtpPlayNackBuffer(1000);
		m_nack_receiver = new YangRtpNackForReceiver(m_rtp_queue, 1000 * 2 / 3);
	}
	m_last_sender_report_rtp_time = 0;
	m_last_sender_report_rtp_time1 = 0;

	m_last_sender_report_sys_time = 0;
	//m_recvcb = NULL;
	//m_mixQueue = pmixque;
	m_conf = conf;
	//m_usingMixav = m_conf->context->rtc.mixAvqueue;
	m_last_seq=0;
	memset(&m_audioFrame,0,sizeof(YangFrame));
	//memset(&m_videoFrame,0,sizeof(YangFrame));
}

YangRecvTrack::~YangRecvTrack() {
	yang_delete(m_rtp_queue);
	yang_delete(m_nack_receiver);
	yang_delete(m_track_desc);
	//m_recvcb = NULL;
}

bool YangRecvTrack::has_ssrc(uint32_t ssrc) {
	return m_track_desc->has_ssrc(ssrc);
}

uint32_t YangRecvTrack::get_ssrc() {
	return m_track_desc->ssrc_;
}

void YangRecvTrack::update_rtt(int32_t rtt) {
	m_nack_receiver->update_rtt(rtt);
}

void YangRecvTrack::update_send_report_time(const YangNtp &ntp,
		uint32_t rtp_time) {
	m_last_sender_report_ntp1 = m_last_sender_report_ntp;
	m_last_sender_report_rtp_time1 = m_last_sender_report_rtp_time;

	m_last_sender_report_ntp = ntp;
	m_last_sender_report_rtp_time = rtp_time;

	m_last_sender_report_sys_time = yang_update_system_time();
}

int64_t YangRecvTrack::cal_avsync_time(uint32_t rtp_time) {
	// Have no recv at least 2 sender reports, can't calculate sync time.
	// TODO: FIXME: use the sample rate from sdp.
	return (int64_t) rtp_time;
	if (m_last_sender_report_rtp_time1 <= 0) {
		return -1;
	}

	// WebRTC using sender report to sync audio/video timestamp, because audio video have different timebase,
	// typical audio opus is 48000Hz, video is 90000Hz.
	// We using two sender report point to calculate avsync timestamp(clock time) with any given rtp timestamp.
	// For example, there are two history sender report of audio as below.
	//   sender_report1: rtp_time1 = 10000, ntp_time1 = 40000
	//   sender_report : rtp_time  = 10960, ntp_time  = 40020
	//   (rtp_time - rtp_time1) / (ntp_time - ntp_time1) = 960 / 20 = 48,
	// Now we can calcualte ntp time(ntp_x) of any given rtp timestamp(rtp_x),
	//   (rtp_x - rtp_time) / (ntp_x - ntp_time) = 48   =>   ntp_x = (rtp_x - rtp_time) / 48 + ntp_time;
	double sys_time_elapsed =
			static_cast<double>(m_last_sender_report_ntp.m_system_ms)
					- static_cast<double>(m_last_sender_report_ntp1.m_system_ms);

	// Check sys_time_elapsed is equal to zero.
	if (fpclassify(sys_time_elapsed) == FP_ZERO) {
		return -1;
	}

	double rtp_time_elpased = static_cast<double>(m_last_sender_report_rtp_time)
			- static_cast<double>(m_last_sender_report_rtp_time1);
	int rate = round(rtp_time_elpased / sys_time_elapsed);

	if (rate <= 0) {
		return -1;
	}

	double delta = round((rtp_time - m_last_sender_report_rtp_time) / rate);

	int64_t avsync_time = delta + m_last_sender_report_ntp.m_system_ms;

	return avsync_time;
}
int32_t YangRecvTrack::send_rtcp_rr() {
	int32_t err = Yang_Ok;

	uint32_t ssrc = m_track_desc->ssrc_;
	const uint64_t &last_time = m_last_sender_report_sys_time;
	//printf("last_%ld",m_last_sender_report_sys_time);
	if ((err = m_session->send_rtcp_rr(ssrc, m_rtp_queue, last_time,
			m_last_sender_report_ntp)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u, last_time=%lld", ssrc, last_time);
	}

	return err;
}

int32_t YangRecvTrack::send_rtcp_xr_rrtr() {
	int32_t err = Yang_Ok;

	if ((err = m_session->send_rtcp_xr_rrtr(m_track_desc->ssrc_)) != Yang_Ok) {
		return yang_error_wrap(err, "ssrc=%u", m_track_desc->ssrc_);
	}

	return err;
}
std::string YangRecvTrack::get_track_id() {
	return m_track_desc->id_;
}

int32_t YangRecvTrack::on_nack(YangRtpPacket *pkt) {
	int32_t err = Yang_Ok;

	uint16_t seq = pkt->m_header.sequence;
	if(seq==m_last_seq){
		m_rtp_queue->update(seq);
		m_last_seq=seq+1;

	}else{

		YangRtpNackInfo *nack_info = m_nack_receiver->find(seq);
		if (nack_info) {
			// seq had been received.
			yang_trace("\nreceive seq===%hu",seq);
			m_nack_receiver->remove(seq);
			return err;
		}

		// insert check nack list
		uint16_t nack_first = 0, nack_last = 0;
		m_rtp_queue->update(seq, nack_first, nack_last);
		if (yang_rtp_seq_distance(nack_first, nack_last) > 0) {
			yang_trace("\nNACK: update seq=%u, nack range [%u, %u]", seq,nack_first, nack_last);
			m_nack_receiver->insert(nack_first, nack_last);
			m_nack_receiver->check_queue_size();

		}
	}
	// insert into video_queue and audio_queue
	m_rtp_queue->set(seq, pkt->m_header.sequence);
	if(!m_nack_receiver->isEmpty()){
		uint32_t timeout_nacks = 0;
		do_check_send_nacks(timeout_nacks);
	}

	return err;
}

int32_t YangRecvTrack::do_check_send_nacks(uint32_t &timeout_nacks) {
	uint32_t sent_nacks = 0;
	return m_session->check_send_nacks(m_nack_receiver, m_track_desc->ssrc_,
			sent_nacks, timeout_nacks);

}
