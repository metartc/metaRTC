#ifndef SRC_YANGWEBRTC_YangH264RecvTrack_H_
#define SRC_YANGWEBRTC_YangH264RecvTrack_H_
#include <yangwebrtc/YangRecvTrack.h>
#include <yangrtp/YangRtpSTAPPayload.h>




class YangH264RecvTrack{
public:
	YangH264RecvTrack(int32_t uid, YangRtcContext *conf,
			YangRtcSessionI *session, YangRtcTrack *stream_descs,
			YangRtpBuffer *rtpBuffer, YangMixQueue *pmixque);
	virtual ~YangH264RecvTrack();
	bool m_hasRequestKeyframe;
public:
	 int32_t on_rtp(YangRtpPacket *pkt);
	 void setReceiveCallback(YangReceiveCallback *cbk);
private:
	uint16_t m_header_sn;
	uint16_t m_lost_sn;
	int32_t m_hasReceiveStap;
	int64_t m_key_frame_ts;

    YangRtpSTAPData m_stapData;
    YangBuffer m_buf;
    YangFrame m_videoFrame;
    char* m_video_buffer;
    YangReceiveCallback *m_recvcb;
    int32_t m_uid;
    YangRtcSessionI *m_session;
    YangRtcTrack *m_track_desc;

private:
	int32_t put_frame_video(char *p, int64_t timestamp, int32_t nb);
	int32_t put_frame_mixvideo(char *p, int64_t timestamp, int32_t nb);

	const static uint16_t s_cache_size = 1024;
	struct YangRtcPacketCache {
		bool in_use;
		bool end;
		uint16_t sn;
		uint32_t ts;
		int32_t nalu_type;
		int32_t nb;
		Fua2H264DataCache fua2;
		char *payload;
	};
    YangRtcPacketCache m_cache_video_pkts[1024];
	void clear_cached_video();
	inline uint16_t cache_index(uint16_t current_sn) {
		//return current_sn % s_cache_size;
		return current_sn&1023;
	}

	bool check_frame_complete(const uint16_t start, const uint16_t end);
	int32_t find_next_lost_sn(uint16_t current_sn, uint16_t &end_sn);
    int32_t packet_video_Raw(YangRtpPacket* pkt);
	int32_t packet_video(const uint16_t start, const uint16_t end);
	int32_t packet_video_key_frame(YangRtpPacket *pkt);

	bool is_keyframe(YangRtpPacket* pkt);
	bool is_keyframe(YangRtcPacketCache* pkt);
	void copy(YangRtpPacket* src,YangRtcPacketCache* pkt);
};
#endif /* SRC_YANGWEBRTC_YANGRTCVIDEORECVTRACK_H_ */
