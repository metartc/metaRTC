//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtc/YangPushMjpeg.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
#include <yangavutil/video/YangMeta.h>

#include <yangrtc/YangPush.h>
#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangRtcConnection.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>

#if Yang_Enable_MJpeg_Encoding

int32_t yang_push_mjpeg_encodeVideo(YangRtcSession *session, YangPushMjpegRtp *rtp,
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
	session->context.stats.on_pub_videoRtp(&session->context.stats.sendStats,pkt,&rtp->buf);
	return yang_send_avpacket(session, pkt, &rtp->buf);
}


int32_t yang_push_mjpeg_package_single_nalu2(YangRtcSession *session,
		YangPushMjpegRtp *rtp, YangFrame *videoFrame) {
	int32_t err = Yang_Ok;


	yang_reset_rtpPacket(&rtp->videoRawPacket);
	rtp->videoRawPacket.header.payload_type = YangMjpegPayloadType;
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
	if ((err = yang_push_mjpeg_encodeVideo(session, rtp, &rtp->videoRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	return err;
}

int32_t yang_push_mjpeg_package_single_nalu(YangRtcSession *session,
		YangPushMjpegRtp *rtp, char *p, int32_t plen, int64_t timestamp) {

	int32_t err = Yang_Ok;

	yang_reset_rtpPacket(&rtp->videoRawPacket);
	rtp->videoRawPacket.header.payload_type = YangMjpegPayloadType;
	rtp->videoRawPacket.header.ssrc = rtp->videoSsrc;
	rtp->videoRawPacket.frame_type = YangFrameTypeVideo;
	rtp->videoRawPacket.header.sequence = rtp->videoSeq++;
	rtp->videoRawPacket.header.timestamp = timestamp;

	rtp->videoRawPacket.payload_type = YangRtspPacketPayloadTypeRaw;
	rtp->videoRawData.payload = rtp->videoBuf;
	rtp->videoRawData.nb = plen;
	memcpy(rtp->videoRawData.payload, p, plen);
	if ((err = yang_push_mjpeg_encodeVideo(session, rtp, &rtp->videoRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}

	return err;
}

int32_t yang_push_mjpeg_package_fu_a(YangRtcSession *session, YangPushMjpegRtp *rtp,
		YangFrame *videoFrame, int32_t fu_payload_size) {
	int32_t err = Yang_Ok;
	int32_t plen = videoFrame->nb;
	uint8_t *pdata = videoFrame->payload;
	char *p = (char*) pdata;
	int32_t nb_left = plen - 1;
	uint8_t header = pdata[0];
	uint8_t nal_type = header & kNalTypeMask;


	int32_t num_of_packet = (plen% fu_payload_size==0)?0:1 + plen / fu_payload_size;
	for (int32_t i = 0; i < num_of_packet; ++i) {
		int32_t packet_size = yang_min(nb_left, fu_payload_size);

		yang_reset_rtpPacket(&rtp->videoFuaPacket);
		rtp->videoFuaPacket.header.payload_type = YangMjpegPayloadType;
		rtp->videoFuaPacket.header.ssrc = rtp->videoSsrc;
		rtp->videoFuaPacket.frame_type = YangFrameTypeVideo;
		rtp->videoFuaPacket.header.sequence = rtp->videoSeq++;
		rtp->videoFuaPacket.header.timestamp = videoFrame->pts;
		rtp->videoFuaPacket.header.marker = (i == num_of_packet - 1) ? 1 : 0;

		rtp->videoFuaPacket.payload_type = YangRtspPacketPayloadTypeFUA2;

		memset(&rtp->videoFua2Data, 0, sizeof(YangFua2MjpegData));
		rtp->videoFua2Data.nri = (YangAvcNaluType) header;
		rtp->videoFua2Data.nalu_type = (YangAvcNaluType) nal_type;
		rtp->videoFua2Data.start = (i == 0) ? 1 : 0;
		rtp->videoFua2Data.end = (i == (num_of_packet - 1)) ? 1 : 0;

		rtp->videoFua2Data.payload = rtp->videoBuf;
		rtp->videoFua2Data.nb = packet_size;
		memcpy(rtp->videoFua2Data.payload, p, packet_size);

		p += packet_size;
		nb_left -= packet_size;
#if Yang_Enable_TWCC
		if(i==0){
			rtp->rtpExtension.twcc.sn=rtp->twccSeq++ ;
			rtp->videoFuaPacket.header.extensions=&rtp->rtpExtension;
			session->context.twcc.insertLocal(&session->context.twcc.session,rtp->rtpExtension.twcc.sn);
		}
#endif
		if ((err = yang_push_mjpeg_encodeVideo(session, rtp, &rtp->videoFuaPacket))
				!= Yang_Ok) {
			return yang_error_wrap(err, "encode packet");
		}
		rtp->videoFuaPacket.header.extensions=NULL;

	}

	return err;
}


int32_t yang_push_mjpeg_video(void *psession, YangPushMjpegRtp *rtp,
		YangFrame *videoFrame) {
	int32_t err = Yang_Ok;
	YangRtcSession *session=(YangRtcSession*)psession;
	if (videoFrame->nb <= kRtpMaxPayloadSize) {
		if ((err = yang_push_mjpeg_package_single_nalu2(session, rtp, videoFrame))
				!= Yang_Ok) {
			return yang_error_wrap(err, "package single nalu");
		}
		session->context.stats.sendStats.videoRtpPacketCount++;
	} else {
		if ((err = yang_push_mjpeg_package_fu_a(session, rtp, videoFrame,
				kRtpMaxPayloadSize)) != Yang_Ok) {
			return yang_error_wrap(err, "package fu-a");
		}
	}
	session->context.stats.sendStats.frameCount++;
	return err;
}

void yang_create_pushMjpeg(YangPushMjpeg *push, YangRtpBuffer* audioRtpBuffer,YangRtpBuffer* videoRtpBuffer) {
	if (push == NULL)		return;
	YangPushMjpegRtp *rtp=(YangPushMjpegRtp*)calloc(sizeof(YangPushMjpegRtp),1);
	push->push=rtp;
	rtp->videoSsrc = 0;
	rtp->videoSeq = 0;

	rtp->videoRtpBuffer = videoRtpBuffer;
	rtp->videoBuf = (char*) calloc(kRtpPacketSize,1);

 #if Yang_Enable_TWCC
	rtp->rtpExtension.has_ext=1;
	rtp->rtpExtension.twcc.has_twcc=1;
	rtp->rtpExtension.twcc.id=Yang_TWCC_ID;
#endif
	push->on_video =yang_push_mjpeg_video;

	//push->on_spspps =yang_push_mjpeg_package_stap_a;
}

void yang_destroy_pushMjpeg(YangPushMjpeg *push) {
	if (push == NULL|| push->push==NULL)		return;
	YangPushMjpegRtp *rtp=push->push;
	yang_free(rtp->videoBuf);

	yang_destroy_rtpPacket(&rtp->videoFuaPacket);
	yang_destroy_rtpPacket(&rtp->videoRawPacket);

	yang_free(push->push);
}
#endif
