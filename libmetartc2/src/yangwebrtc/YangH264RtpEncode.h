#ifndef SRC_YANGRTP_YANGRTPPACKETWRAP_H_
#define SRC_YANGRTP_YANGRTPPACKETWRAP_H_
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpBuffer.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangstream/YangStreamCapture.h>
#include <yangwebrtc/YangRtcSessionI.h>

class YangH264RtpEncode {
public:
	YangH264RtpEncode();
	virtual ~YangH264RtpEncode();
	void init(YangRtpBuffer* rtpBuffer,YangRtcSessionI* psession);
	int32_t on_h264_video(YangStreamCapture* videoFrame);
	int32_t on_audio(YangStreamCapture* audioFrame);
	int32_t package_single_nalu(char* p,int32_t plen, int64_t timestamp);
	int32_t package_single_nalu(YangStreamCapture* videoFrame);
	int32_t package_fu_a(YangStreamCapture* videoFrame,int32_t fu_payload_size);
	int32_t package_stap_a(YangStreamCapture* videoFrame);
public:
	uint32_t m_videoSsrc;
	uint32_t m_audioSsrc;
	uint16_t m_videoSeq;
	uint16_t m_audioSeq;
private:
	YangRtpBuffer* m_rtpBuffer;
	YangRtcSessionI* m_session;
	YangFua2H264Data m_videoFua2Data;
	YangRtpRawData m_videoRawData;
	YangRtpRawData m_audioRawData;
	YangRtpSTAPData m_stapData;
    YangBuffer m_buf;

	char* m_videoBuf;
	YangRtpPacket m_videoFuaPacket;
	YangRtpPacket m_videoRawPacket;
	YangRtpPacket m_videoStapPacket;
	YangRtpPacket m_audioRawPacket;
	int32_t encodeVideo(YangRtpPacket *pkt);
	int32_t encodeAudio(YangRtpPacket *pkt);
};
#endif /* SRC_YANGRTP_YANGRTPPACKETWRAP_H_ */
