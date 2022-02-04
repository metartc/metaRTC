//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangwebrtc/YangH265RtpEncode.h>
#include <yangwebrtc/YangRtcSession.h>
#include <yangwebrtc/YangRtcConnection.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavtype_h265.h>
#include <yangavutil/video/YangMeta.h>


#if	Yang_H265_Encoding
int32_t yang_h265_encodeAudio(YangRtcSession *session, YangH265RtpEncode *rtp,
		YangRtpPacket *pkt);
int32_t yang_h265_encodeVideo(YangRtcSession *session, YangH265RtpEncode *rtp,
		YangRtpPacket *pkt);
int32_t yang_h265_package_fu_a(YangRtcSession *session, YangH265RtpEncode *rtp,
		YangFrame *videoFrame, int32_t fu_payload_size);
int32_t yang_h265_package_single_nalu(YangRtcSession *session,
		YangH265RtpEncode *rtp, char *p, int32_t plen, int64_t timestamp);
int32_t yang_h265_package_single_nalu2(YangRtcSession *session,
		YangH265RtpEncode *rtp, YangFrame *videoFrame);
void yang_init_h265RtpEncode(YangH265RtpEncode *rtp, YangRtpBuffer *rtpBuffer) {
	if (rtp == NULL)
		return;
	rtp->videoSsrc = 0;
	rtp->videoSeq = 0;
	rtp->audioSeq = 0;
	rtp->audioSsrc = 0;
	rtp->rtpBuffer = NULL;

	rtp->videoBuf = (char*) calloc(kRtpPacketSize,1);
	memset(&rtp->stapData, 0, sizeof(YangRtpSTAPData));
	yang_init_stap(&rtp->stapData);
	rtp->rtpBuffer = rtpBuffer;
}
void yang_destroy_h265RtpEncode(YangH265RtpEncode *rtp) {
	if (rtp == NULL)
		return;
	yang_free(rtp->videoBuf);
	yang_reset_h2645_stap(&rtp->stapData);
	yang_destroy_stap(&rtp->stapData);
	yang_destroy_rtpPacket(&rtp->videoFuaPacket);
	yang_destroy_rtpPacket(&rtp->videoRawPacket);
	yang_destroy_rtpPacket(&rtp->videoStapPacket);
	yang_destroy_rtpPacket(&rtp->audioRawPacket);
}

int32_t yang_pub_h265_audio(YangRtcSession *session, YangH265RtpEncode *rtp,
		YangFrame *audioFrame) {
	int err = 0;
	yang_reset_rtpPacket(&rtp->audioRawPacket);
	rtp->audioRawPacket.header.payload_type = kAudioPayloadType;
	rtp->audioRawPacket.header.ssrc = rtp->audioSsrc;
	rtp->audioRawPacket.frame_type = YangFrameTypeAudio;
	rtp->audioRawPacket.header.marker = true;

	rtp->audioRawPacket.header.sequence = rtp->audioSeq++;
	rtp->audioRawPacket.header.timestamp = audioFrame->pts;
	rtp->audioRawPacket.header.padding_length = 0;
	rtp->audioRawPacket.payload_type = YangRtspPacketPayloadTypeRaw;

	rtp->audioRawData.payload = rtp->videoBuf;
	rtp->audioRawData.nb = audioFrame->nb;
	memcpy(rtp->audioRawData.payload, audioFrame->payload,
			rtp->audioRawData.nb);
	if ((err = yang_h265_encodeAudio(session, rtp, &rtp->audioRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}

	return err;
}
int32_t yang_pub_h265_video(YangRtcSession *session, YangH265RtpEncode *rtp,
		YangFrame *videoFrame) {
	int32_t err = Yang_Ok;

	if (videoFrame->nb <= kRtpMaxPayloadSize) {
		if ((err = yang_h265_package_single_nalu2(session, rtp, videoFrame))
				!= Yang_Ok) {
			return yang_error_wrap(err, "package single nalu");
		}
	} else {
		if ((err = yang_h265_package_fu_a(session, rtp, videoFrame,
				kRtpMaxPayloadSize)) != Yang_Ok) {
			return yang_error_wrap(err, "package fu-a");
		}
	}

	return err;
}
int32_t yang_pub_h265_package_stap_a(YangRtcSession *session,
		YangH265RtpEncode *rtp, YangFrame *videoFrame) {
	int err = Yang_Ok;

	yang_reset_rtpPacket(&rtp->videoStapPacket);
	rtp->videoStapPacket.header.payload_type = kVideoPayloadType;
	rtp->videoStapPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoStapPacket.frame_type = YangFrameTypeVideo;
	rtp->videoStapPacket.nalu_type = (YangAvcNaluType) kStapA;
	rtp->videoStapPacket.header.marker = false;
	rtp->videoStapPacket.header.sequence = rtp->videoSeq++;
	rtp->videoStapPacket.header.timestamp = videoFrame->pts;

	rtp->videoStapPacket.payload_type = YangRtspPacketPayloadTypeSTAP;

	YangSample vps_sample; //= new YangSample();
	YangSample sps_sample; //= new YangSample();
	YangSample pps_sample; //= new YangSample();
	yang_decodeMetaH265(videoFrame->payload, videoFrame->nb, &vps_sample,
			&sps_sample, &pps_sample);
	yang_reset_h2645_stap(&rtp->stapData);
	uint8_t header = (uint8_t) sps_sample.bytes[0];
	rtp->stapData.nri = header;
	yang_insert_YangSampleVector(&rtp->stapData.nalus, &vps_sample);
	yang_insert_YangSampleVector(&rtp->stapData.nalus, &sps_sample);
	yang_insert_YangSampleVector(&rtp->stapData.nalus, &pps_sample);

	if ((err = yang_h265_encodeVideo(session, rtp, &rtp->videoStapPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	return err;
}

int32_t yang_h265_package_single_nalu2(YangRtcSession *session,
		YangH265RtpEncode *rtp, YangFrame *videoFrame) {
	int32_t err = Yang_Ok;


	yang_reset_rtpPacket(&rtp->videoRawPacket);
	rtp->videoRawPacket.header.payload_type = kVideoPayloadType;
	rtp->videoRawPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoRawPacket.frame_type = YangFrameTypeVideo;
	rtp->videoRawPacket.header.sequence = rtp->videoSeq++;
	rtp->videoRawPacket.header.timestamp = videoFrame->pts;
	rtp->videoRawPacket.header.marker = true;
	rtp->videoRawPacket.payload_type = YangRtspPacketPayloadTypeRaw;
	//memset(&rtp->raw_video,0,sizeof(YangRtpRawPayload));
	rtp->videoRawData.payload = rtp->videoBuf;
	rtp->videoRawData.nb = videoFrame->nb;
	memcpy(rtp->videoRawData.payload, videoFrame->payload,
			rtp->videoRawData.nb);
	if ((err = yang_h265_encodeVideo(session, rtp, &rtp->videoRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	return err;
}
int32_t yang_h265_package_single_nalu(YangRtcSession *session,
		YangH265RtpEncode *rtp, char *p, int32_t plen, int64_t timestamp) {

	int32_t err = Yang_Ok;

	yang_reset_rtpPacket(&rtp->videoRawPacket);
	rtp->videoRawPacket.header.payload_type = kVideoPayloadType;
	rtp->videoRawPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoRawPacket.frame_type = YangFrameTypeVideo;
	rtp->videoRawPacket.header.sequence = rtp->videoSeq++;
	rtp->videoRawPacket.header.timestamp = timestamp;

	rtp->videoRawPacket.payload_type = YangRtspPacketPayloadTypeRaw;
	rtp->videoRawData.payload = rtp->videoBuf;
	rtp->videoRawData.nb = plen;
	memcpy(rtp->videoRawData.payload, p, plen);
	if ((err = yang_h265_encodeVideo(session, rtp, &rtp->videoRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}

	return err;
}
int32_t yang_h265_package_fu_a(YangRtcSession *session, YangH265RtpEncode *rtp,
		YangFrame *videoFrame, int32_t fu_payload_size) {
	int32_t err = Yang_Ok;
	int32_t plen = videoFrame->nb;
	uint8_t *pdata = videoFrame->payload;
	char *p = (char*) pdata + 1;
	int32_t nb_left = plen - 1;
	uint8_t header = pdata[0];
	uint8_t nal_type = YANG_HEVC_NALU_TYPE(header);

	int32_t num_of_packet = ((plen - 1) % fu_payload_size==0)?0:1  + (plen - 1) / fu_payload_size;
	for (int32_t i = 0; i < num_of_packet; ++i) {
		int32_t packet_size = yang_min(nb_left, fu_payload_size);

		yang_reset_rtpPacket(&rtp->videoFuaPacket);
		rtp->videoFuaPacket.header.payload_type = kVideoPayloadType;
		rtp->videoFuaPacket.header.ssrc = rtp->videoSsrc;
		rtp->videoFuaPacket.frame_type = YangFrameTypeVideo;
		rtp->videoFuaPacket.header.sequence = rtp->videoSeq++;
		rtp->videoFuaPacket.header.timestamp = videoFrame->pts;
		rtp->videoFuaPacket.header.marker = (i == num_of_packet - 1) ? 1 : 0;

		rtp->videoFuaPacket.payload_type = YangRtspPacketPayloadTypeFUA2;

		memset(&rtp->videoFua2Data, 0, sizeof(YangFua2H265Data));
		rtp->videoFua2Data.nri = (YangHevcNaluType) header;
		rtp->videoFua2Data.nalu_type = (YangHevcNaluType) nal_type;
		rtp->videoFua2Data.start = (i == 0) ? 1 : 0;
		rtp->videoFua2Data.end = (i == (num_of_packet - 1)) ? 1 : 0;

		rtp->videoFua2Data.payload = rtp->videoBuf;
		rtp->videoFua2Data.nb = packet_size;
		memcpy(rtp->videoFua2Data.payload, p, packet_size);

		p += packet_size;
		nb_left -= packet_size;

		if ((err = yang_h265_encodeVideo(session, rtp, &rtp->videoFuaPacket))
				!= Yang_Ok) {
			return yang_error_wrap(err, "encode packet");
		}

	}

	return err;
}
int32_t yang_h265_encodeVideo(YangRtcSession *session, YangH265RtpEncode *rtp,
		YangRtpPacket *pkt) {
	int err = 0;

	yang_init_buffer(&rtp->buf, yang_get_rtpBuffer(rtp->rtpBuffer),
			kRtpPacketSize);
	if ((err = yang_encode_rtpHeader(&rtp->buf, &pkt->header)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp header(%d) encode packet fail",
				pkt->payload_type);
	}
	if (pkt->payload_type == YangRtspPacketPayloadTypeRaw) {
		err = yang_encode_h264_raw(&rtp->buf, &rtp->videoRawData);
	} else if (pkt->payload_type == YangRtspPacketPayloadTypeFUA2) {
		err = yang_encode_h265_fua2(&rtp->buf, &rtp->videoFua2Data);

	} else if (pkt->payload_type == YangRtspPacketPayloadTypeSTAP) {
		err = yang_encode_h265_stap(&rtp->buf, &rtp->stapData);
		yang_reset_h2645_stap(&rtp->stapData);
	}

	if (err != Yang_Ok) {
		return yang_error_wrap(err, "rtp payload(%d) encode packet fail",
				pkt->payload_type);
	}
	if (pkt->header.padding_length > 0) {
		uint8_t padding = pkt->header.padding_length;
		if (!yang_buffer_require(&rtp->buf, padding)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"padding requires %d bytes", padding);
		}
		memset(rtp->buf.head, padding, padding);
		yang_buffer_skip(&rtp->buf, padding);
	}


	return yang_send_avpacket(session, pkt, &rtp->buf);

}
int32_t yang_h265_encodeAudio(YangRtcSession *session, YangH265RtpEncode *rtp,
		YangRtpPacket *pkt) {
	int err = 0;

	yang_init_buffer(&rtp->buf, yang_get_rtpBuffer(rtp->rtpBuffer),
			kRtpPacketSize);

	if ((err = yang_encode_rtpHeader(&rtp->buf, &pkt->header)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp header(%d) encode packet fail",
				pkt->payload_type);
	}
	err = yang_encode_h264_raw(&rtp->buf, &rtp->audioRawData);
	if (err != Yang_Ok) {
		return yang_error_wrap(err, "rtp payload(%d) encode packet fail",
				pkt->payload_type);
	}
	if (pkt->header.padding_length > 0) {
		uint8_t padding = pkt->header.padding_length;
		if (!yang_buffer_require(&rtp->buf, padding)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"padding requires %d bytes", padding);
		}
		memset(rtp->buf.head, padding, padding);
		yang_buffer_skip(&rtp->buf, padding);
	}


	return yang_send_avpacket(session, pkt, &rtp->buf);

}

#endif
