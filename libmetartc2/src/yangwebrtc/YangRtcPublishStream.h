#ifndef YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_
#define YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_
#include <string>
#include <string.h>
#include <stdint.h>
#include <yangutil/buffer/YangBuffer.h>


#include <yangrtp/YangRtcpXr.h>
#include <yangrtp/YangRtcpNack.h>
#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtcpPsfbCommon.h>
#include <yangrtp/YangPublishNackBuffer.h>

#include <yangwebrtc/YangRtcSessionI.h>

class YangRtcPublishStream
{
private:
    YangRtcSessionI* m_session;
private:
    uint32_t m_audioSsrc;
    uint32_t m_videoSsrc;


private:
    // For merged-write messages.
    int32_t m_mw_msgs;
    bool m_realtime;
    // Whether enabled nack.
    bool m_nack_enabled;

    YangPublishNackBuffer* m_audio_queue;
    YangPublishNackBuffer* m_video_queue;


public:
    YangRtcPublishStream(YangRtcSessionI* s);
    virtual ~YangRtcPublishStream();
public:
    int32_t init(uint32_t audioSsrc,uint32_t videoSsrc);
public:
    int32_t on_rtcp(YangRtcpCommon* rtcp);
    int32_t on_recv_nack(YangPublishNackBuffer* que,const std::vector<uint16_t>& lost_seqs);
    int32_t cache_nack(YangRtpPacket* pkt,char* p,int plen);
private:
    YangPublishBuffer* fetch_rtp_packet(YangPublishNackBuffer* que,uint16_t seq);
    int32_t on_rtcp_xr(YangRtcpXr* rtcp);
    int32_t on_rtcp_nack(YangRtcpNack* rtcp);
    int32_t on_rtcp_ps_feedback(YangRtcpPsfbCommon* rtcp);
    int32_t on_rtcp_rr(YangRtcpRR* rtcp);

public:
    int32_t do_request_keyframe(uint32_t ssrc);
};
#endif /* YANGWEBRTC_YANGRTCPUBLISHSTREAM_H_ */
