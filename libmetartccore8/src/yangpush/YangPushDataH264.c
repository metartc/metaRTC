//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangrtc/YangRtcRtcp.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>

#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>

#include <yangutil/sys/YangLog.h>
#include <yangvideo/YangMeta.h>
#include <yangpush/YangPushDataSession.h>

typedef struct{
	uint16_t videoSeq;
	uint16_t twccSeq;

	YangPushDataSession *dataBuffer;

	char* videoBuf;

	YangFua2H264Data videoFua2Data;
	YangRtpRawData videoRawData;
	YangRtpSTAPData stapData;

	YangBuffer buf;

	YangRtpPacket videoFuaPacket;
	YangRtpPacket videoRawPacket;
	YangRtpPacket videoStapPacket;
}YangPushVideoSession;


static int32_t yang_encodeVideo(YangPushVideoSession *session,
		YangRtpPacket *pkt,YangPacket* pktData) {
	uint8_t padding;
	int32_t err = 0;

	yang_init_buffer(&session->buf,(char*) pktData->payload,pktData->capacity);

	if ((err = yang_encode_rtpHeader(&session->buf, &pkt->header)) != Yang_Ok) {
		pkt->header.extensions=NULL;
		return yang_error_wrap(err, "h264 rtp header(%d) encode packet fail",
				pkt->payload_type);
	}

	if (pkt->payload_type == YangRtpPacketPayloadTypeRaw) {
		err = yang_encode_h264_raw(&session->buf, &session->videoRawData);
	} else if (pkt->payload_type == YangRtpPacketPayloadTypeFUA2) {
		err = yang_encode_h264_fua2(&session->buf, &session->videoFua2Data);

	} else if (pkt->payload_type == YangRtpPacketPayloadTypeSTAP) {
		err = yang_encode_h264_stap(&session->buf, &session->stapData);
		yang_reset_h2645_stap(&session->stapData);
	}

	if (err != Yang_Ok) {
		pkt->header.extensions=NULL;
		return yang_error_wrap(err, "h264 rtp payload(%d) encode packet fail",
				pkt->payload_type);
	}

	if (pkt->header.padding_length > 0) {
		padding = pkt->header.padding_length;
		if (!yang_buffer_require(&session->buf, padding)) {
			pkt->header.extensions=NULL;
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"padding requires %d bytes", padding);
		}
		yang_memset(session->buf.head, padding, padding);
		yang_buffer_skip(&session->buf, padding);
	}

	pktData->length=yang_buffer_pos(&session->buf);
	pktData->sequence=pkt->header.sequence;
	pkt->header.extensions=NULL;

	return Yang_Ok;
}

static int32_t yang_push_h264_package_stap_a(
		void* psession,YangH2645Conf *conf,uint64_t timestamp) {
	int32_t err = Yang_Ok;
	YangPushVideoSession *session=(YangPushVideoSession*)psession;
	YangSample sps_sample;
	YangSample pps_sample;
	YangPacket* pktData=NULL;
	if(psession==NULL || conf==NULL) return 1;
	pktData=yang_pkt_getPutPacket(session->dataBuffer->pushDataBuffer);

	yang_reset_rtpPacket(&session->videoStapPacket);
	session->videoStapPacket.header.payload_type = 0;
	session->videoStapPacket.header.ssrc =0;
	session->videoStapPacket.frame_type = YangFrameTypeVideo;
	session->videoStapPacket.nalu_type = (YangAvcNaluType) kStapA;
	session->videoStapPacket.header.marker = yangfalse;
	session->videoStapPacket.header.sequence = session->videoSeq++;
	session->videoStapPacket.header.timestamp = timestamp;

	session->videoStapPacket.payload_type = YangRtpPacketPayloadTypeSTAP;

	yang_reset_h2645_stap(&session->stapData);

	sps_sample.nb=conf->spsLen;
	sps_sample.bytes=(char*)conf->sps;

	pps_sample.nb=conf->ppsLen;
	pps_sample.bytes=(char*)conf->pps;


	session->stapData.nri = (YangAvcNaluType) ((uint8_t) sps_sample.bytes[0]);

	yang_insert_YangSampleVector(&session->stapData.nalus, &sps_sample);
	yang_insert_YangSampleVector(&session->stapData.nalus, &pps_sample);

	if ((err = yang_encodeVideo(session, &session->videoStapPacket,pktData))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}

	return err;
}

static int32_t yang_push_h264_package_single_nalu2(YangPushVideoSession *session,YangFrame *videoFrame) {
	int32_t err = Yang_Ok;
	YangPacket* pktData;

	pktData=yang_pkt_getPutPacket(session->dataBuffer->pushDataBuffer);

	yang_reset_rtpPacket(&session->videoRawPacket);
	session->videoRawPacket.header.payload_type = 0;
	session->videoRawPacket.header.ssrc = 0;
	session->videoRawPacket.frame_type = YangFrameTypeVideo;
	session->videoRawPacket.header.sequence = session->videoSeq++;
	session->videoRawPacket.header.timestamp = videoFrame->pts;
	session->videoRawPacket.header.marker = yangtrue;
	session->videoRawPacket.payload_type = YangRtpPacketPayloadTypeRaw;

	session->videoRawData.payload = session->videoBuf;
	session->videoRawData.nb = videoFrame->nb;
	yang_memcpy(session->videoRawData.payload, videoFrame->payload,
			session->videoRawData.nb);

	if ((err = yang_encodeVideo( session, &session->videoRawPacket,pktData))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}

	return err;
}

static int32_t yang_push_h264_package_fu_a(YangPushVideoSession *session,
		YangFrame *videoFrame, int32_t fu_payload_size) {
	int32_t err = Yang_Ok;
	int32_t plen = videoFrame->nb;
	int32_t packet_size,i;
	uint8_t *pdata = videoFrame->payload;
	char *p = (char*) pdata + 1;
	int32_t nb_left = plen - 1;
	uint8_t header = pdata[0];
	uint8_t nal_type = header & kNalTypeMask;

	YangPacket* pktData=NULL;
	uint32_t num_of_packet = ((plen - 1) % fu_payload_size==0)?0:1 + (plen - 1) / fu_payload_size;

	yang_pkt_checkBuffer(session->dataBuffer->pushDataBuffer,num_of_packet);

	for (i = 0; i < num_of_packet; ++i) {
		packet_size = yang_min(nb_left, fu_payload_size);

		yang_reset_rtpPacket(&session->videoFuaPacket);
		session->videoFuaPacket.header.payload_type = 0;
		session->videoFuaPacket.header.ssrc = 0;
		session->videoFuaPacket.frame_type = YangFrameTypeVideo;
		session->videoFuaPacket.header.sequence = session->videoSeq++;
		session->videoFuaPacket.header.timestamp = videoFrame->pts;
		session->videoFuaPacket.header.marker = (i == num_of_packet - 1) ? 1 : 0;

		session->videoFuaPacket.payload_type = YangRtpPacketPayloadTypeFUA2;

		yang_memset(&session->videoFua2Data, 0, sizeof(YangFua2H264Data));
		session->videoFua2Data.nri = (YangAvcNaluType) header;
		session->videoFua2Data.nalu_type = (YangAvcNaluType) nal_type;
		session->videoFua2Data.start = (i == 0) ? 1 : 0;
		session->videoFua2Data.end = (i == (num_of_packet - 1)) ? 1 : 0;

		session->videoFua2Data.payload = session->videoBuf;
		session->videoFua2Data.nb = packet_size;
		yang_memcpy(session->videoFua2Data.payload, p, packet_size);

		p += packet_size;
		nb_left -= packet_size;

		pktData=yang_pkt_getPutPacket(session->dataBuffer->pushDataBuffer);

		if ((err = yang_encodeVideo(session, &session->videoFuaPacket,pktData))
				!= Yang_Ok) {
			return yang_error_wrap(err, "encode packet");
		}
		session->videoFuaPacket.header.extensions=NULL;
	}

	return err;
}


static int32_t yang_push_h264_video(void *psession,
		YangFrame *videoFrame) {
	int32_t err = Yang_Ok;
	YangPushVideoSession *session=(YangPushVideoSession*)psession;
	//if (videoFrame->frametype == YANG_Frametype_Spspps)
			//return yang_push_h264_package_stap_a(session, videoFrame);
	if (videoFrame->nb <= kRtpMaxPayloadSize) {
		if ((err = yang_push_h264_package_single_nalu2(session,videoFrame))
				!= Yang_Ok) {
			return yang_error_wrap(err, "package single nalu");
		}

	} else {
		if ((err = yang_push_h264_package_fu_a(session, videoFrame,
				kRtpMaxPayloadSize)) != Yang_Ok) {
			return yang_error_wrap(err, "package fu-a");
		}
	}

	return err;
}

static void yang_reset(void* psession){
	YangPushVideoSession *session=(YangPushVideoSession*)psession;
	if(session==NULL)
		return;
	session->videoSeq=0;
}

int32_t yang_create_pushVideoDataH264(YangPushDataVideo* videoData,YangPushDataSession *pushData){
	YangPushVideoSession* session;
	if (videoData == NULL || pushData==NULL)		return 1;
	session= (YangPushVideoSession*)yang_calloc(sizeof(YangPushVideoSession),1);
	videoData->session=session;
	session->dataBuffer = pushData;

	session->videoSeq = 0;

	session->videoBuf = (char*) yang_calloc(kRtpPacketSize,1);

	yang_memset(&session->stapData, 0, sizeof(YangRtpSTAPData));
	yang_create_stap(&session->stapData);


	videoData->reset=yang_reset;
	videoData->on_video =yang_push_h264_video;
	videoData->on_meta = yang_push_h264_package_stap_a;

	return Yang_Ok;
}

void yang_destroy_pushVideoDataH264(YangPushDataVideo *videoData) {
	YangPushVideoSession *session=NULL;
	if (videoData->session==NULL)		return;
	session=videoData->session;

	yang_free(session->videoBuf);
	yang_reset_h2645_stap(&session->stapData);

	yang_destroy_stap(&session->stapData);

	yang_destroy_rtpPacket(&session->videoFuaPacket);
	yang_destroy_rtpPacket(&session->videoRawPacket);
	yang_destroy_rtpPacket(&session->videoStapPacket);

	yang_free(videoData->session);
}

