//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangrtc/YangPushAudio.h>

#include <yangrtc/YangPush.h>
#include <yangrtc/YangRtcConnection.h>

#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpConstant.h>

int32_t yang_push_audio_encodeAudio(YangRtcSession *session, YangPushAudioRtp *rtp,
		YangRtpPacket *pkt) {
	int err = 0;

	yang_init_buffer(&rtp->buf, yang_get_rtpBuffer(rtp->audioRtpBuffer),	kRtpPacketSize);

	if ((err = yang_encode_rtpHeader(&rtp->buf, &pkt->header)) != Yang_Ok) {
		return yang_error_wrap(err, "audio rtp header(%d) encode packet fail",
				pkt->payload_type);
	}
	err = yang_encode_h264_raw(&rtp->buf, &rtp->audioRawData);
	if (err != Yang_Ok) {
		return yang_error_wrap(err, "audio rtp payload(%d) encode packet fail",
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

    if(session&&session->context.stats.on_pub_audioRtp){
        session->context.stats.on_pub_audioRtp(&session->context.stats.sendStats,pkt,&rtp->buf);
    }

	return yang_send_avpacket(session, pkt, &rtp->buf);
}

int32_t yang_push_audio_rtp(void *psession, YangPushAudioRtp *rtp,
		YangFrame *audioFrame) {
	int err = 0;
	YangRtcSession *session=(YangRtcSession*)psession;
	yang_reset_rtpPacket(&rtp->audioRawPacket);
	rtp->audioRawPacket.header.payload_type = session->audioPayloadType;
	rtp->audioRawPacket.header.ssrc = rtp->audioSsrc;
	rtp->audioRawPacket.frame_type = YangFrameTypeAudio;
	rtp->audioRawPacket.header.marker = yangtrue;

	rtp->audioRawPacket.header.sequence = rtp->audioSeq++;
	rtp->audioRawPacket.header.timestamp = audioFrame->pts;
	rtp->audioRawPacket.header.padding_length = 0;
	rtp->audioRawPacket.payload_type = YangRtpPacketPayloadTypeRaw;

	rtp->audioRawData.payload = rtp->audioBuffer;
    rtp->audioRawData.nb = audioFrame->nb;
	yang_memcpy(rtp->audioRawData.payload, audioFrame->payload,
            rtp->audioRawData.nb);
	if ((err = yang_push_audio_encodeAudio(session, rtp, &rtp->audioRawPacket))
			!= Yang_Ok) {
		return yang_error_wrap(err, "encode packet");
	}
	session->context.stats.sendStats.audioRtpPacketCount++;
	return err;
}

void yang_create_pushAudio(YangPushAudio* push,YangRtpBuffer* audioRtpBuffer){
	if (push == NULL)		return;
	YangPushAudioRtp *rtp=(YangPushAudioRtp*)yang_calloc(sizeof(YangPushAudioRtp),1);
	push->push=rtp;
	rtp->audioBuffer = (char*) yang_calloc(kRtpPacketSize,1);
	rtp->audioSeq = 0;
	rtp->audioSsrc = 0;
	rtp->audioRtpBuffer = audioRtpBuffer;
	push->on_audio =yang_push_audio_rtp;

}

void yang_destroy_pushAudio(YangPushAudio* push){
	if (push == NULL|| push->push==NULL)		return;
	YangPushAudioRtp *rtp=push->push;
	yang_free(rtp->audioBuffer);
	yang_destroy_rtpPacket(&rtp->audioRawPacket);
	yang_free(push->push);
}


