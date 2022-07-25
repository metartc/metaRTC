//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPushH264.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangMeta.h>

#include <yangrtc/YangPush.h>
#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangRtcConnection.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>



int32_t yang_push_h264_encodeVideo(YangRtcSession *session, YangPushH264Rtp *rtp,
		YangRtpPacket *pkt) {
	int err = 0;

	yang_init_buffer(&rtp->buf, yang_get_rtpBuffer(rtp->videoRtpBuffer),	kRtpPacketSize);
	if ((err = yang_encode_rtpHeader(&rtp->buf, &pkt->header)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp header(%d) encode packet fail",
				pkt->payload_type);
	}
	if (pkt->payload_type == YangRtspPacketPayloadTypeRaw) {
		err = yang_encode_h264_raw(&rtp->buf, &rtp->videoRawData);
	} else if (pkt->payload_type == YangRtspPacketPayloadTypeFUA2) {
		err = yang_encode_h264_fua2(&rtp->buf, &rtp->videoFua2Data);

	} else if (pkt->payload_type == YangRtspPacketPayloadTypeSTAP) {
		err = yang_encode_h264_stap(&rtp->buf, &rtp->stapData);
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

	session->context.stats.sendStats.videoRtpPacketCount++;
	return yang_send_avpacket(session, pkt, &rtp->buf);

}
int32_t yang_push_h264_encodeAudio(YangRtcSession *session, YangPushH264Rtp *rtp,
		YangRtpPacket *pkt) {
	int err = 0;

	yang_init_buffer(&rtp->buf, yang_get_rtpBuffer(rtp->audioRtpBuffer),	kRtpPacketSize);

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

int32_t yang_push_h264_package_stap_a(void *psession,
		YangPushH264Rtp *rtp, YangFrame *videoFrame) {
	int err = Yang_Ok;
	YangRtcSession *session=(YangRtcSession*)psession;
	yang_reset_rtpPacket(&rtp->videoStapPacket);
	rtp->videoStapPacket.header.payload_type = YangH264PayloadType;
	rtp->videoStapPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoStapPacket.frame_type = YangFrameTypeVideo;
	rtp->videoStapPacket.nalu_type = (YangAvcNaluType) kStapA;
	rtp->videoStapPacket.header.marker = false;
	rtp->videoStapPacket.header.sequence = rtp->videoSeq++;
	rtp->videoStapPacket.header.timestamp = videoFrame->pts;

	rtp->videoStapPacket.payload_type = YangRtspPacketPayloadTypeSTAP;

	yang_reset_h2645_stap(&rtp->stapData);

	YangSample sps_sample;
	YangSample pps_sample;
	yang_decodeMetaH264(videoFrame->payload, videoFrame->nb, &sps_sample,&pps_sample);


	uint8_t header = (uint8_t) sps_sample.bytes[0];
	rtp->stapData.nri = (YangAvcNaluType) header;

	yang_insert_YangSampleVector(&rtp->stapData.nalus, &sps_sample);
	yang_insert_YangSampleVector(&rtp->stapData.nalus, &pps_sample);

	if ((err = yang_push_h264_encodeVideo(session, rtp, &rtp->videoStapPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	return err;
}

int32_t yang_push_h264_package_single_nalu2(YangRtcSession *session,
		YangPushH264Rtp *rtp, YangFrame *videoFrame) {
	int32_t err = Yang_Ok;


	yang_reset_rtpPacket(&rtp->videoRawPacket);
	rtp->videoRawPacket.header.payload_type = YangH264PayloadType;
	rtp->videoRawPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoRawPacket.frame_type = YangFrameTypeVideo;
	rtp->videoRawPacket.header.sequence = rtp->videoSeq++;
	rtp->videoRawPacket.header.timestamp = videoFrame->pts;
	rtp->videoRawPacket.header.marker = true;
	rtp->videoRawPacket.payload_type = YangRtspPacketPayloadTypeRaw;

	rtp->videoRawData.payload = rtp->videoBuf;
	rtp->videoRawData.nb = videoFrame->nb;
	memcpy(rtp->videoRawData.payload, videoFrame->payload,
			rtp->videoRawData.nb);
	if ((err = yang_push_h264_encodeVideo(session, rtp, &rtp->videoRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	return err;
}
int32_t yang_push_h264_package_single_nalu(YangRtcSession *session,
		YangPushH264Rtp *rtp, char *p, int32_t plen, int64_t timestamp) {

	int32_t err = Yang_Ok;

	yang_reset_rtpPacket(&rtp->videoRawPacket);
	rtp->videoRawPacket.header.payload_type = YangH264PayloadType;
	rtp->videoRawPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoRawPacket.frame_type = YangFrameTypeVideo;
	rtp->videoRawPacket.header.sequence = rtp->videoSeq++;
	rtp->videoRawPacket.header.timestamp = timestamp;

	rtp->videoRawPacket.payload_type = YangRtspPacketPayloadTypeRaw;
	rtp->videoRawData.payload = rtp->videoBuf;
	rtp->videoRawData.nb = plen;
	memcpy(rtp->videoRawData.payload, p, plen);
	if ((err = yang_push_h264_encodeVideo(session, rtp, &rtp->videoRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}

	return err;
}
int32_t yang_push_h264_package_fu_a(YangRtcSession *session, YangPushH264Rtp *rtp,
		YangFrame *videoFrame, int32_t fu_payload_size) {
	int32_t err = Yang_Ok;
	int32_t plen = videoFrame->nb;
	uint8_t *pdata = videoFrame->payload;
	char *p = (char*) pdata + 1;
	int32_t nb_left = plen - 1;
	uint8_t header = pdata[0];
	uint8_t nal_type = header & kNalTypeMask;


	int32_t num_of_packet = ((plen - 1) % fu_payload_size==0)?0:1 + (plen - 1) / fu_payload_size;
	for (int32_t i = 0; i < num_of_packet; ++i) {
		int32_t packet_size = yang_min(nb_left, fu_payload_size);

		yang_reset_rtpPacket(&rtp->videoFuaPacket);
		rtp->videoFuaPacket.header.payload_type = YangH264PayloadType;
		rtp->videoFuaPacket.header.ssrc = rtp->videoSsrc;
		rtp->videoFuaPacket.frame_type = YangFrameTypeVideo;
		rtp->videoFuaPacket.header.sequence = rtp->videoSeq++;
		rtp->videoFuaPacket.header.timestamp = videoFrame->pts;
		rtp->videoFuaPacket.header.marker = (i == num_of_packet - 1) ? 1 : 0;

		rtp->videoFuaPacket.payload_type = YangRtspPacketPayloadTypeFUA2;

		memset(&rtp->videoFua2Data, 0, sizeof(YangFua2H264Data));
		rtp->videoFua2Data.nri = (YangAvcNaluType) header;
		rtp->videoFua2Data.nalu_type = (YangAvcNaluType) nal_type;
		rtp->videoFua2Data.start = (i == 0) ? 1 : 0;
		rtp->videoFua2Data.end = (i == (num_of_packet - 1)) ? 1 : 0;

		rtp->videoFua2Data.payload = rtp->videoBuf;
		rtp->videoFua2Data.nb = packet_size;
		memcpy(rtp->videoFua2Data.payload, p, packet_size);

		p += packet_size;
		nb_left -= packet_size;
#if Yang_Using_TWCC
		if(i==0){
			rtp->rtpExtension.twcc.sn=rtp->twccSeq++ ;
			rtp->videoFuaPacket.header.extensions=&rtp->rtpExtension;
			session->context.twcc.insertLocal(&session->context.twcc.session,rtp->rtpExtension.twcc.sn);
		}
#endif
		if ((err = yang_push_h264_encodeVideo(session, rtp, &rtp->videoFuaPacket))
				!= Yang_Ok) {
			return yang_error_wrap(err, "encode packet");
		}
		rtp->videoFuaPacket.header.extensions=NULL;

	}

	return err;
}


int32_t yang_push_h264_audio(void *psession, YangPushH264Rtp *rtp,
		YangFrame *audioFrame) {
	int err = 0;
	YangRtcSession *session=(YangRtcSession*)psession;
	yang_reset_rtpPacket(&rtp->audioRawPacket);
	rtp->audioRawPacket.header.payload_type = YangAudioPayloadType;
	rtp->audioRawPacket.header.ssrc = rtp->audioSsrc;
	rtp->audioRawPacket.frame_type = YangFrameTypeAudio;
	rtp->audioRawPacket.header.marker = true;

	rtp->audioRawPacket.header.sequence = rtp->audioSeq++;
	rtp->audioRawPacket.header.timestamp = audioFrame->pts;
	rtp->audioRawPacket.header.padding_length = 0;
	rtp->audioRawPacket.payload_type = YangRtspPacketPayloadTypeRaw;

	rtp->audioRawData.payload = rtp->videoBuf;
	rtp->audioRawData.nb = audioFrame->nb; //getAudioLen();
	memcpy(rtp->audioRawData.payload, audioFrame->payload,
			rtp->audioRawData.nb); //getAudioData(), rtp->audioRawData.nb);
	if ((err = yang_push_h264_encodeAudio(session, rtp, &rtp->audioRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	session->context.stats.sendStats.audioRtpPacketCount++;
	return err;
}

int32_t yang_push_h264_video(void *psession, YangPushH264Rtp *rtp,
		YangFrame *videoFrame) {
	int32_t err = Yang_Ok;
	YangRtcSession *session=(YangRtcSession*)psession;
	if (videoFrame->nb <= kRtpMaxPayloadSize) {
		if ((err = yang_push_h264_package_single_nalu2(session, rtp, videoFrame))
				!= Yang_Ok) {
			return yang_error_wrap(err, "package single nalu");
		}
		session->context.stats.sendStats.videoRtpPacketCount++;
	} else {
		if ((err = yang_push_h264_package_fu_a(session, rtp, videoFrame,
				kRtpMaxPayloadSize)) != Yang_Ok) {
			return yang_error_wrap(err, "package fu-a");
		}
	}
	session->context.stats.sendStats.frameCount++;
	return err;
}

void yang_create_pushH264(YangPushH264 *push, YangRtpBuffer* audioRtpBuffer,YangRtpBuffer* videoRtpBuffer) {
	if (push == NULL)		return;
	YangPushH264Rtp *rtp=(YangPushH264Rtp*)calloc(sizeof(YangPushH264Rtp),1);
	push->push=rtp;
	rtp->videoSsrc = 0;
	rtp->videoSeq = 0;
	rtp->audioSeq = 0;
	rtp->audioSsrc = 0;
	rtp->audioRtpBuffer = audioRtpBuffer;
	rtp->videoRtpBuffer = videoRtpBuffer;
	rtp->videoBuf = (char*) calloc(kRtpPacketSize,1);
	memset(&rtp->stapData, 0, sizeof(YangRtpSTAPData));
	yang_create_stap(&rtp->stapData);
 #if Yang_Using_TWCC
	rtp->rtpExtension.has_ext=1;
	rtp->rtpExtension.twcc.has_twcc=1;
	rtp->rtpExtension.twcc.id=Yang_TWCC_ID;
#endif
	push->on_video =yang_push_h264_video;
	push->on_audio =yang_push_h264_audio;
	push->on_spspps =yang_push_h264_package_stap_a;

}
void yang_destroy_pushH264(YangPushH264 *push) {
	if (push == NULL|| push->push==NULL)		return;
	YangPushH264Rtp *rtp=push->push;
	yang_free(rtp->videoBuf);
	yang_reset_h2645_stap(&rtp->stapData);

	yang_destroy_stap(&rtp->stapData);
	yang_destroy_rtpPacket(&rtp->videoFuaPacket);
	yang_destroy_rtpPacket(&rtp->videoRawPacket);
	yang_destroy_rtpPacket(&rtp->videoStapPacket);
	yang_destroy_rtpPacket(&rtp->audioRawPacket);
	yang_free(push->push);
}

