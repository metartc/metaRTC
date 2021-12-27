#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangMeta.h>
#include <yangwebrtc/YangH264RtpEncode.h>
YangH264RtpEncode::YangH264RtpEncode() {
    m_videoSsrc = 0;
    m_videoSeq = 0;
    m_audioSeq = 0;
    m_audioSsrc = 0;
    m_rtpBuffer = NULL;
    m_session = NULL;
    m_videoBuf = new char[kRtpPacketSize];
    memset(&m_stapData,0,sizeof(YangRtpSTAPData));
    m_stapData.nalus=new std::vector<YangSample*>();
}

YangH264RtpEncode::~YangH264RtpEncode() {
    m_session = NULL;
    yang_deleteA(m_videoBuf);
    yang_reset_h2645_stap(&m_stapData);
    yang_delete(m_stapData.nalus);
}
void YangH264RtpEncode::init(YangRtpBuffer *rtpBuffer,
                             YangRtcSessionI *psession) {
    m_rtpBuffer = rtpBuffer;
    m_session = psession;
}
int32_t YangH264RtpEncode::on_audio(YangStreamCapture *audioFrame) {
    int err = 0;
    m_audioRawPacket.reset();
    m_audioRawPacket.m_header.payload_type=kAudioPayloadType;
    m_audioRawPacket.m_header.ssrc=m_audioSsrc;
    m_audioRawPacket.m_frame_type = YangFrameTypeAudio;
    m_audioRawPacket.m_header.marker=true;

    m_audioRawPacket.m_header.sequence=m_audioSeq++;
    m_audioRawPacket.m_header.timestamp=audioFrame->getAudioTimestamp();
    m_audioRawPacket.m_header.padding_length=0;
    m_audioRawPacket.m_payload_type = YangRtspPacketPayloadTypeRaw;

    m_audioRawData.payload = m_videoBuf;
    m_audioRawData.nb = audioFrame->getAudioLen();
    memcpy(m_audioRawData.payload, audioFrame->getAudioData(), m_audioRawData.nb);
    if ((err = encodeAudio(&m_audioRawPacket)) != Yang_Ok) {
        return yang_error_wrap(err, "encode packet");
    }

    return err;
}
int32_t YangH264RtpEncode::on_h264_video(YangStreamCapture *videoFrame) {
    int32_t err = Yang_Ok;

    if (videoFrame->getVideoLen() <= kRtpMaxPayloadSize) {
        if ((err = package_single_nalu(videoFrame)) != Yang_Ok) {
            return yang_error_wrap(err, "package single nalu");
        }
    } else {
        if ((err = package_fu_a(videoFrame, kRtpMaxPayloadSize)) != Yang_Ok) {
            return yang_error_wrap(err, "package fu-a");
        }
    }

    return err;
}

int32_t YangH264RtpEncode::package_single_nalu(YangStreamCapture *videoFrame) {
    int32_t err = Yang_Ok;

    m_videoRawPacket.reset();
    m_videoRawPacket.m_header.payload_type=kVideoPayloadType;
    m_videoRawPacket.m_header.ssrc=m_videoSsrc;
    m_videoRawPacket.m_frame_type = YangFrameTypeVideo;
    m_videoRawPacket.m_header.sequence=m_videoSeq++;
    m_videoRawPacket.m_header.timestamp=videoFrame->getVideoTimestamp();
    m_videoRawPacket.m_header.marker=true;
    m_videoRawPacket.m_payload_type = YangRtspPacketPayloadTypeRaw;
    //memset(&m_raw_video,0,sizeof(YangRtpRawPayload));
    m_videoRawData.payload = m_videoBuf;
    m_videoRawData.nb = videoFrame->getVideoLen();
    memcpy(m_videoRawData.payload, videoFrame->getVideoData(),  m_videoRawData.nb);
    if ((err = encodeVideo(&m_videoRawPacket)) != Yang_Ok) {
        return yang_error_wrap(err, "encode packet");
    }
    return err;
}
int32_t YangH264RtpEncode::package_single_nalu(char *p, int32_t plen,
                                               int64_t timestamp) {

    int32_t err = Yang_Ok;
    m_videoRawPacket.reset();
    m_videoRawPacket.m_header.payload_type=kVideoPayloadType;
    m_videoRawPacket.m_header.ssrc=m_videoSsrc;
    m_videoRawPacket.m_frame_type = YangFrameTypeVideo;
    m_videoRawPacket.m_header.sequence=m_videoSeq++;
    m_videoRawPacket.m_header.timestamp=timestamp;


    m_videoRawPacket.m_payload_type = YangRtspPacketPayloadTypeRaw;
    m_videoRawData.payload = m_videoBuf;
    m_videoRawData.nb = plen;
    memcpy(m_videoRawData.payload, p, plen);
    if ((err = encodeVideo(&m_videoRawPacket)) != Yang_Ok) {
        return yang_error_wrap(err, "encode packet");
    }

    return err;
}
int32_t YangH264RtpEncode::package_fu_a(YangStreamCapture *videoFrame,
                                        int32_t fu_payload_size) {
    int32_t err = Yang_Ok;
    int32_t plen = videoFrame->getVideoLen();
    uint8_t *pdata = videoFrame->getVideoData();
    char *p = (char*) pdata + 1;
    int32_t nb_left = plen - 1;
    uint8_t header = pdata[0];
    uint8_t nal_type = header & kNalTypeMask;

    int32_t num_of_packet = 1 + (plen - 1) / fu_payload_size;
    for (int32_t i = 0; i < num_of_packet; ++i) {
        int32_t packet_size = yang_min(nb_left, fu_payload_size);
        m_videoFuaPacket.reset();
        m_videoFuaPacket.m_header.payload_type=kVideoPayloadType;
        m_videoFuaPacket.m_header.ssrc=m_videoSsrc;
        m_videoFuaPacket.m_frame_type = YangFrameTypeVideo;
        m_videoFuaPacket.m_header.sequence=m_videoSeq++;
        m_videoFuaPacket.m_header.timestamp=videoFrame->getVideoTimestamp();
        m_videoFuaPacket.m_header.marker=bool(i == num_of_packet - 1);


        m_videoFuaPacket.m_payload_type = YangRtspPacketPayloadTypeFUA2;

        memset(&m_videoFua2Data,0,sizeof(YangFua2H264Data));
        m_videoFua2Data.nri = (YangAvcNaluType) header;
        m_videoFua2Data.nalu_type = (YangAvcNaluType) nal_type;
        m_videoFua2Data.start = bool(i == 0);
        m_videoFua2Data.end = bool(i == (num_of_packet - 1));

        m_videoFua2Data.payload = m_videoBuf;
        m_videoFua2Data.nb = packet_size;
        memcpy(m_videoFua2Data.payload, p, packet_size);

        p += packet_size;
        nb_left -= packet_size;

        if ((err = encodeVideo(&m_videoFuaPacket)) != Yang_Ok) {
            return yang_error_wrap(err, "encode packet");
        }

    }

    return err;
}
int32_t YangH264RtpEncode::encodeVideo(YangRtpPacket *pkt) {
    int err = 0;
    m_buf.init(m_rtpBuffer->getBuffer(), kRtpPacketSize);

    if ((err = yang_encode_rtpHeader(&m_buf,&pkt->m_header)) != Yang_Ok) {
        return yang_error_wrap(err, "rtp header(%d) encode packet fail",
                               pkt->m_payload_type);
    }
    if (pkt->m_payload_type == YangRtspPacketPayloadTypeRaw) {
    	err=yang_encode_h264_raw(&m_buf,&m_videoRawData);
    } else if (pkt->m_payload_type == YangRtspPacketPayloadTypeFUA2) {
    	 err = yang_encode_h264_fua2(&m_buf,&m_videoFua2Data);

    } else if (pkt->m_payload_type == YangRtspPacketPayloadTypeSTAP) {
    	 err = yang_encode_h264_stap(&m_buf,&m_stapData);
    	 yang_reset_h2645_stap(&m_stapData);
    }

    if (err != Yang_Ok) {
        return yang_error_wrap(err, "rtp payload(%d) encode packet fail",  pkt->m_payload_type);
    }
    if (pkt->m_header.padding_length > 0) {
        uint8_t padding = pkt->m_header.padding_length;
        if (!m_buf.require(padding)) {
            return yang_error_wrap(ERROR_RTC_RTP_MUXER,
                                   "padding requires %d bytes", padding);
        }
        memset(m_buf.head(), padding, padding);
        m_buf.skip(padding);
    }

    if (m_session)
        return m_session->send_avpacket(pkt, &m_buf);
    return err;
}
int32_t YangH264RtpEncode::encodeAudio(YangRtpPacket *pkt) {
    int err = 0;
    m_buf.init(m_rtpBuffer->getBuffer(), kRtpPacketSize);


   if ((err = yang_encode_rtpHeader(&m_buf,&pkt->m_header)) != Yang_Ok) {
        return yang_error_wrap(err, "rtp header(%d) encode packet fail",
                               pkt->m_payload_type);
    }
   err=yang_encode_h264_raw(&m_buf,&m_audioRawData);
    if (err != Yang_Ok) {
        return yang_error_wrap(err, "rtp payload(%d) encode packet fail",
                               pkt->m_payload_type);
    }
    if (pkt->m_header.padding_length > 0) {
        uint8_t padding = pkt->m_header.padding_length;
        if (!m_buf.require(padding)) {
            return yang_error_wrap(ERROR_RTC_RTP_MUXER,
                                   "padding requires %d bytes", padding);
        }
        memset(m_buf.head(), padding, padding);
        m_buf.skip(padding);
    }

    if (m_session)
        return m_session->send_avpacket(pkt, &m_buf);
    return err;
}
int32_t YangH264RtpEncode::package_stap_a(YangStreamCapture *videoFrame) {
    int err = Yang_Ok;
//    uint8_t *buf = videoFrame->getVideoData();
//    int32_t spsLen = *(buf + 12) + 1;
//    uint8_t *sps = buf + 13;
//    int32_t ppsLen = *(sps + spsLen + 1) + 1;
//    uint8_t *pps = buf + 13 + spsLen + 2;
    m_videoStapPacket.reset();
    m_videoStapPacket.m_header.payload_type=kVideoPayloadType;
    m_videoStapPacket.m_header.ssrc=m_videoSsrc;
    m_videoStapPacket.m_frame_type = YangFrameTypeVideo;
    m_videoStapPacket.m_nalu_type = (YangAvcNaluType) kStapA;
    m_videoStapPacket.m_header.marker=false;
    m_videoStapPacket.m_header.sequence=m_videoSeq++;
    m_videoStapPacket.m_header.timestamp=0;


    m_videoStapPacket.m_payload_type = YangRtspPacketPayloadTypeSTAP;

    yang_reset_h2645_stap(&m_stapData);

    YangSample *sps_sample = new YangSample();
    YangSample *pps_sample = new YangSample();
    yang_decodeMetaH264(videoFrame->getVideoData(),videoFrame->getVideoLen(),sps_sample,pps_sample);
    uint8_t header = (uint8_t)sps_sample->bytes[0];

    m_stapData.nri = (YangAvcNaluType) header;

   // YangSample *vps_sample = new YangSample();
  //  vps_sample->m_bytes = (char*) vps;
   // vps_sample->m_size = vpsLen;
   // m_stap.nalus.push_back(vps_sample);

    if(m_stapData.nalus) m_stapData.nalus->push_back(sps_sample);
    if(m_stapData.nalus) m_stapData.nalus->push_back(pps_sample);


    if ((err = encodeVideo(&m_videoStapPacket)) != Yang_Ok) {
        return yang_error_wrap(err, "encode packet");
    }
    return err;

}

