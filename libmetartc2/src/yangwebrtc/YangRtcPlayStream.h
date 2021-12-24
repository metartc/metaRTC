
#ifndef YANGWEBRTC_YANGRTCPLAYSTREAM_H_
#define YANGWEBRTC_YANGRTCPLAYSTREAM_H_
#include <string>
#include <string.h>
#include <stdint.h>
#include <yangutil/yangavtype.h>

#include <yangrtp/YangRtcp.h>
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtcpTWCC.h>
#include <yangrtp/YangRtcpSR.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/buffer/YangVideoDecoderBuffer.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangstream/YangStreamType.h>
#include <yangrtp/YangRtpPacket.h>

#include <yangutil/sys/YangSsrc.h>
#include <yangwebrtc/YangRtcContext.h>
#include <yangwebrtc/YangRtcSessionI.h>
#include <yangwebrtc/YangVideoRecvTrack.h>
#include <yangwebrtc/YangRtcAudioRecvTrack.h>
using namespace std;
class YangRtcTrack;
class YangRtcSourceDescription;

class YangRtcPlayStream{
public:
	YangRtcPlayStream(YangRtcSessionI *psession);
	virtual ~YangRtcPlayStream();

	int32_t initialize(YangRtcContext *conf,
			YangRtcSourceDescription *stream_desc, YangReceiveCallback *cbk,
			YangRtpBuffer *rtpBuffer);
	int32_t send_rtcp_rr();
	int32_t send_rtcp_xr_rrtr();
	int32_t check_send_nacks();
	int32_t send_periodic_twcc();
	//void request_keyframe(uint32_t ssrc);


	int32_t on_rtp(char *data, int32_t nb_data);
	int32_t on_rtcp(YangRtcpCommon *rtcp);
private:
	int32_t on_rtcp_sr(YangRtcpSR *rtcp);
	int32_t on_rtcp_xr(YangRtcpXr *rtcp);
	int32_t on_twcc(uint16_t sn);
	void update_rtt(uint32_t ssrc, int32_t rtt);
	void update_send_report_time(uint32_t ssrc, const YangNtp &ntp,
			uint32_t rtp_time);
	void simulate_nack_drop(int32_t nn);
	void simulate_drop_packet(YangRtpHeader *h, int32_t nn_bytes);

private:
	YangRtcContext *m_conf;
	YangRtcSessionI *m_session;
	uint16_t m_pt_to_drop;
	YangRtpBuffer *m_rtpBuffer;
	YangMixQueue m_mixQueue;
	YangRtpPacket m_rtp;
    YangBuffer m_buf;

	bool m_twcc_enabled;
	int32_t m_twcc_id;
	int64_t m_last_time_send_twcc;
	uint8_t m_twcc_fb_count;
	YangRtcpTWCC m_rtcp_twcc;

	bool m_nack_enabled;

	YangRtpExtensionTypes m_extension_types;

	int32_t m_nn_simulate_nack_drop;
	bool m_request_keyframe;
	std::vector<YangRtcAudioRecvTrack*> m_audio_tracks;
	std::vector<YangVideoRecvTrack*> m_video_tracks;
	YangRtcAudioRecvTrack* get_audio_track(uint32_t ssrc);
	YangVideoRecvTrack* get_video_track(uint32_t ssrc);

};

#endif /* YANGWEBRTC_YANGRTCPLAYSTREAM_H_ */
