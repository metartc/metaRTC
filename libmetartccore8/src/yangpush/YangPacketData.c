//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangpush/YangPushDataSession.h>

#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangutil/yangavinfo.h>
#include <yangutil/yangrtptype.h>

typedef struct{
	uint16_t audioSeq;
	uint16_t twccSeq;

	YangPushDataSession* dataSession;

    YangBuffer buf;
	YangRtpPacket audioRawPacket;
}YangAudioSession;


static int32_t yang_audio_rtp( void *psession,YangFrame *audioFrame) {
	uint8_t padding;
	int32_t err = 0;
	YangAudioSession *session=(YangAudioSession *)psession;

	YangPacket* pkt=NULL;

	if(session==NULL)
		return 1;

	pkt=yang_pkt_getPutPacket(session->dataSession->pushDataBuffer);

	yang_reset_rtpPacket(&session->audioRawPacket);

	session->audioRawPacket.header.payload_type = 0;
	session->audioRawPacket.header.ssrc = 0;//session->audioSsrc;
	session->audioRawPacket.frame_type = YangFrameTypeAudio;
	session->audioRawPacket.header.marker = yangtrue;

	session->audioRawPacket.header.sequence = session->audioSeq++;
	session->audioRawPacket.header.timestamp = audioFrame->pts;
	session->audioRawPacket.header.padding_length = 0;
	session->audioRawPacket.payload_type = YangRtpPacketPayloadTypeRaw;

	yang_init_buffer(&session->buf,(char*) pkt->payload,pkt->capacity);

	if ((err = yang_encode_rtpHeader(&session->buf, &session->audioRawPacket.header)) != Yang_Ok) {
		return yang_error_wrap(err, "audio rtp header(%d) encode packet fail",
				session->audioRawPacket.payload_type);
	}

	yang_write_bytes(&session->buf,(char*)audioFrame->payload, audioFrame->nb);

	if (session->audioRawPacket.header.padding_length > 0) {
		padding = session->audioRawPacket.header.padding_length;
		if (!yang_buffer_require(&session->buf, padding)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"padding requires %d bytes", padding);
		}
		yang_memset(session->buf.head, padding, padding);
		yang_buffer_skip(&session->buf, padding);
	}
	pkt->length=yang_buffer_pos(&session->buf);
	pkt->sequence=session->audioRawPacket.header.sequence;

	return err;
}

static void yang_reset(void* psession){
	YangAudioSession *session=(YangAudioSession *)psession;
	if(session==NULL)
		return;

	session->audioSeq=0;
}

int32_t yang_create_pushDataAudio(YangPushDataAudio* audioData,YangPushDataSession *pushData){
	YangAudioSession *session=NULL;

	if (audioData == NULL || pushData==NULL)
		return 1;

	session=(YangAudioSession*)yang_calloc(sizeof(YangAudioSession),1);
	audioData->session=session;

	session->dataSession=pushData;

	session->audioSeq = 0;
	session->twccSeq = 0;

	audioData->reset=yang_reset;
	audioData->on_audio =yang_audio_rtp;
	return Yang_Ok;
}

void yang_destroy_pushDataAudio(YangPushDataAudio* audioData){
	YangAudioSession *session=NULL;

	if (audioData == NULL|| audioData->session==NULL)
		return;

	session=(YangAudioSession*)audioData->session;

	yang_destroy_rtpPacket(&session->audioRawPacket);
	yang_free(audioData->session);
}




int32_t yang_create_pushData(YangPushData* pushData,YangPacketBuffer* pushDataBuffer) {

	YangPushDataSession *session=NULL;

	if (pushData == NULL||pushDataBuffer==NULL)
		return 1;

	session=(YangPushDataSession*)yang_calloc(sizeof(YangPushDataSession),1);
	pushData->pushData=session;

	session->pushDataBuffer=pushDataBuffer;

	session->buffer = (uint8_t*)yang_malloc(kRtpPacketSize);

	return Yang_Ok;
}


void yang_destroy_pushData(YangPushData* pushData){
	YangPushDataSession *session=NULL;

	if(pushData==NULL || pushData->pushData==NULL)
		return;

	session=(YangPushDataSession *)pushData->pushData;

	yang_free(session->buffer);
	yang_free(pushData->pushData);
}

