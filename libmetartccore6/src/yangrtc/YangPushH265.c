//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPushH265.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>

#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangMeta.h>

#include <yangrtc/YangPush.h>
#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangutil/yangavh265.h>

#if	Yang_Enable_H265_Encoding

int32_t yang_h265_encodeVideo(YangRtcSession *session, YangPushH265Rtp *rtp,
		YangRtpPacket *pkt) {
	int err = 0;

	yang_init_buffer(&rtp->buf, yang_get_rtpBuffer(rtp->videoRtpBuffer),
			kRtpPacketSize);
	if ((err = yang_encode_rtpHeader(&rtp->buf, &pkt->header)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp header(%d) encode packet fail",
				pkt->payload_type);
	}
	if (pkt->payload_type == YangRtpPacketPayloadTypeRaw) {
		err = yang_encode_h264_raw(&rtp->buf, &rtp->videoRawData);
	} else if (pkt->payload_type == YangRtpPacketPayloadTypeFUA2) {
		err = yang_encode_h265_fua2(&rtp->buf, &rtp->videoFua2Data);

	} else if (pkt->payload_type == YangRtpPacketPayloadTypeSTAP) {
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
		yang_memset(rtp->buf.head, padding, padding);
		yang_buffer_skip(&rtp->buf, padding);
	}

	session->context.stats.on_pub_videoRtp(&session->context.stats.sendStats,pkt,&rtp->buf);
	return yang_send_avpacket(session, pkt, &rtp->buf);

}

int32_t yang_push_h265_package_stap_a(void *psession,
		YangPushH265Rtp *rtp, YangFrame *videoFrame) {
	int err = Yang_Ok;
	YangRtcSession *session=(YangRtcSession*)psession;
	yang_reset_rtpPacket(&rtp->videoStapPacket);
	rtp->videoStapPacket.header.payload_type = session->h265PayloadType;
	rtp->videoStapPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoStapPacket.frame_type = YangFrameTypeVideo;
	rtp->videoStapPacket.nalu_type = (YangAvcNaluType) kStapA;
	rtp->videoStapPacket.header.marker = yangfalse;
	rtp->videoStapPacket.header.sequence = rtp->videoSeq++;
	rtp->videoStapPacket.header.timestamp = videoFrame->pts;

	rtp->videoStapPacket.payload_type = YangRtpPacketPayloadTypeSTAP;

	YangSample vps_sample;
	YangSample sps_sample;
	YangSample pps_sample;
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
		YangPushH265Rtp *rtp, YangFrame *videoFrame) {
	int32_t err = Yang_Ok;


	yang_reset_rtpPacket(&rtp->videoRawPacket);
	rtp->videoRawPacket.header.payload_type = session->h265PayloadType;
	rtp->videoRawPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoRawPacket.frame_type = YangFrameTypeVideo;
	rtp->videoRawPacket.header.sequence = rtp->videoSeq++;
	rtp->videoRawPacket.header.timestamp = videoFrame->pts;
	rtp->videoRawPacket.header.marker = yangtrue;
	rtp->videoRawPacket.payload_type = YangRtpPacketPayloadTypeRaw;

	rtp->videoRawData.payload = rtp->videoBuf;
	rtp->videoRawData.nb = videoFrame->nb;
	yang_memcpy(rtp->videoRawData.payload, videoFrame->payload,
			rtp->videoRawData.nb);
	if ((err = yang_h265_encodeVideo(session, rtp, &rtp->videoRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	return err;
}
int32_t yang_h265_package_single_nalu(YangRtcSession *session,
		YangPushH265Rtp *rtp, char *p, int32_t plen, int64_t timestamp) {

	int32_t err = Yang_Ok;

	yang_reset_rtpPacket(&rtp->videoRawPacket);
	rtp->videoRawPacket.header.payload_type = session->h265PayloadType;
	rtp->videoRawPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoRawPacket.frame_type = YangFrameTypeVideo;
	rtp->videoRawPacket.header.sequence = rtp->videoSeq++;
	rtp->videoRawPacket.header.timestamp = timestamp;

	rtp->videoRawPacket.payload_type = YangRtpPacketPayloadTypeRaw;
	rtp->videoRawData.payload = rtp->videoBuf;
	rtp->videoRawData.nb = plen;
	yang_memcpy(rtp->videoRawData.payload, p, plen);
	if ((err = yang_h265_encodeVideo(session, rtp, &rtp->videoRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}

	return err;
}
int32_t yang_h265_package_fu_a(YangRtcSession *session, YangPushH265Rtp *rtp,
		YangFrame *videoFrame, int32_t fu_payload_size) {
	int32_t err = Yang_Ok;
	int32_t plen = videoFrame->nb;
	uint8_t *pdata = videoFrame->payload;
	char *p = (char*) pdata + 2;
	int32_t nb_left = plen - 2;
	uint8_t header = pdata[0];
	uint8_t nal_type = YANG_HEVC_NALU_TYPE(header);

	int32_t num_of_packet = ((plen - 1) % fu_payload_size==0)?0:1  + (plen - 1) / fu_payload_size;
	for (int32_t i = 0; i < num_of_packet; ++i) {
		int32_t packet_size = yang_min(nb_left, fu_payload_size);

		yang_reset_rtpPacket(&rtp->videoFuaPacket);
		rtp->videoFuaPacket.header.payload_type = session->h265PayloadType;
		rtp->videoFuaPacket.header.ssrc = rtp->videoSsrc;
		rtp->videoFuaPacket.frame_type = YangFrameTypeVideo;
		rtp->videoFuaPacket.header.sequence = rtp->videoSeq++;
		rtp->videoFuaPacket.header.timestamp = videoFrame->pts;
		rtp->videoFuaPacket.header.marker = (i == num_of_packet - 1) ? 1 : 0;

		rtp->videoFuaPacket.payload_type = YangRtpPacketPayloadTypeFUA2;

		yang_memset(&rtp->videoFua2Data, 0, sizeof(YangFua2H265Data));
		rtp->videoFua2Data.nri = (YangHevcNaluType) header;
		rtp->videoFua2Data.nalu_type = (YangHevcNaluType) nal_type;
		rtp->videoFua2Data.start = (i == 0) ? 1 : 0;
		rtp->videoFua2Data.end = (i == (num_of_packet - 1)) ? 1 : 0;

		rtp->videoFua2Data.payload = rtp->videoBuf;
		rtp->videoFua2Data.nb = packet_size;
		yang_memcpy(rtp->videoFua2Data.payload, p, packet_size);

		p += packet_size;
		nb_left -= packet_size;
#if Yang_Enable_TWCC
		if(i==0){
			rtp->rtpExtension.twcc.sn=rtp->twccSeq++ ;
			rtp->videoFuaPacket.header.extensions=&rtp->rtpExtension;
			session->context.twcc.insertLocal(&session->context.twcc.session,rtp->rtpExtension.twcc.sn);
		}
#endif
		if ((err = yang_h265_encodeVideo(session, rtp, &rtp->videoFuaPacket))
				!= Yang_Ok) {
			return yang_error_wrap(err, "encode packet");
		}
		rtp->videoFuaPacket.header.extensions=NULL;

	}

	return err;
}



int32_t yang_push_h265_video(void *psession, YangPushH265Rtp *rtp,
		YangFrame *videoFrame) {
	int32_t err = Yang_Ok;
	YangRtcSession *session=(YangRtcSession*)psession;
	if (videoFrame->nb <= kRtpMaxPayloadSize) {
		if ((err = yang_h265_package_single_nalu2(session, rtp, videoFrame))
				!= Yang_Ok) {
			return yang_error_wrap(err, "package single nalu");
		}
		session->context.stats.sendStats.videoRtpPacketCount++;
	} else {
		if ((err = yang_h265_package_fu_a(session, rtp, videoFrame,
				kRtpMaxPayloadSize)) != Yang_Ok) {
			return yang_error_wrap(err, "package fu-a");
		}
	}
	session->context.stats.sendStats.frameCount++;
	return err;
}
void yang_create_pushH265(YangPushH265 *push,YangRtpBuffer* videoRtpBuffer) {
	if (push == NULL)		return;
	YangPushH265Rtp *rtp=(YangPushH265Rtp*)yang_calloc(sizeof(YangPushH265Rtp),1);
	push->push=rtp;
	rtp->videoSsrc = 0;
	rtp->videoSeq = 0;

	rtp->videoRtpBuffer = videoRtpBuffer;

	rtp->videoBuf = (char*) yang_calloc(kRtpPacketSize,1);
	yang_memset(&rtp->stapData, 0, sizeof(YangRtpSTAPData));
	yang_create_stap(&rtp->stapData);
#if Yang_Enable_TWCC
	rtp->rtpExtension.has_ext=1;
	rtp->rtpExtension.twcc.has_twcc=1;
	rtp->rtpExtension.twcc.id=Yang_TWCC_ID;
#endif
	push->on_video =yang_push_h265_video;

	push->on_spspps =yang_push_h265_package_stap_a;
}
void yang_destroy_pushH265(YangPushH265 *push) {
	if (push == NULL || push->push == NULL)
		return;
	YangPushH265Rtp *rtp=push->push;
	yang_free(rtp->videoBuf);
	yang_reset_h2645_stap(&rtp->stapData);
	yang_destroy_stap(&rtp->stapData);
	yang_destroy_rtpPacket(&rtp->videoFuaPacket);
	yang_destroy_rtpPacket(&rtp->videoRawPacket);
	yang_destroy_rtpPacket(&rtp->videoStapPacket);

	yang_free(push->push);
}

#endif
