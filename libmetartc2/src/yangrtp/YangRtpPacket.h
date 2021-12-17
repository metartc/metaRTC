#ifndef SRC_YANGRTP_YANGRTPPACKET_H_
#define SRC_YANGRTP_YANGRTPPACKET_H_
#include <yangutil/yangavtype.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangrtp/YangRtcp.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpBuffer.h>


    // 8 = audio
#define    YangFrameTypeAudio 8
    // 9 = video
#define YangFrameTypeVideo  9
    // 18 = script data


class YangRtpPacket
{
public:
    YangRtpPacket();
    virtual ~YangRtpPacket();
public:
    YangRtpHeader m_header;

public:
    // The first byte as nalu type, for video decoder only.
    int32_t m_actual_buffer_size;
    int32_t m_nalu_type;
    // The frame type, for RTMP bridger or SFU source.
    int32_t m_frame_type;
// Fast cache for performance.
    YangRtspPacketPayloadType m_payload_type;

    char* m_payload;
    int32_t m_nb;
    //bool m_is_keyframe;
private:
    // The cached payload size for packet.
    int32_t m_cached_payload_size;


public:
    // virtual YangRtpPacket* copy();
     char* wrap(YangRtpBuffer* rtpb,char* data,int32_t nb);
public:
    // Parse the TWCC extension, ignore by default.
    void enable_twcc_decode() { if(m_header.extensions) m_header.extensions->enable_twcc_decode(); } // SrsRtpPacket::enable_twcc_decode

    // Set the padding of RTP packet.
    void set_padding(int32_t size);
    // Increase the padding of RTP packet.
    void add_padding(int32_t size);

    // Whether the packet is Audio packet.
    bool is_audio();
    // Set RTP header extensions for encoding or decoding header extension
    void set_extension_types(YangRtpExtensionTypes* v);
   int32_t decode(YangBuffer* buf);
    void reset();

};
int32_t yang_encode_rtppayload(YangRtpPacket* pkt,YangBuffer *buf);
#endif /* SRC_YANGRTP_YANGRTPPACKET_H_ */
