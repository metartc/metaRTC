//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtpPacket.h>

#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpSTAPPayload.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavinfotype.h>



void yang_create_rtpPacket(YangRtpPacket* pkt){
	yang_memset(&pkt->header,0,sizeof(YangRtpHeader));
	pkt->payload_type = YangRtpPacketPayloadTypeUnknown;
	pkt->actual_buffer_size = 0;

	pkt->nalu_type = YangAvcNaluTypeReserved;
	pkt->frame_type = YangFrameTypeAudio;
	pkt->cached_payload_size = 0;

	pkt->payload=NULL;

	pkt->nb=0;
}
void yang_destroy_rtpPacket(YangRtpPacket* pkt){
	if(pkt==NULL) return;
	if(pkt) yang_free(pkt->header.extensions);
}
void yang_reset_rtpPacket(YangRtpPacket* pkt){
	if(pkt==NULL) return;
	yang_free(pkt->header.extensions);
	yang_memset(&pkt->header,0,sizeof(YangRtpHeader));
	pkt->payload_type = YangRtpPacketPayloadTypeUnknown;
	pkt->actual_buffer_size = 0;

	pkt->nalu_type = YangAvcNaluTypeReserved;
	pkt->frame_type = YangFrameTypeAudio;
	pkt->cached_payload_size = 0;

	pkt->payload=NULL;
	pkt->nb=0;
}


int32_t yang_decode_rtpPacket(YangRtpPacket* pkt,YangBuffer *buf) {
	if(pkt==NULL) return 1;
	int32_t err = Yang_Ok;

	if ((err = yang_decode_rtpHeader(buf,&pkt->header)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp header");
	}


	uint8_t padding = pkt->header.padding_length;
	if (!yang_buffer_require(buf,padding)) {
		return yang_error_wrap(err, "requires padding %d bytes", padding);
	}
	buf->size=buf->size - padding;
	pkt->payload=buf->head;
	pkt->nb=yang_buffer_left(buf);
    pkt->payload_type = YangRtpPacketPayloadTypeRaw;

	return err;
}


char* yang_wrap_rtpPacket(YangRtpPacket* pkt,YangRtpBuffer* rtpb,char* data,int32_t nb){
	if(pkt==NULL) return NULL;
	pkt->actual_buffer_size=nb;

	char* p=yang_get_rtpBuffer(rtpb);
	yang_memcpy(p,data,nb);
	return p;
}

void yang_rtpPacket_set_padding(YangRtpPacket* pkt,int32_t size) {
	if(pkt==NULL) return ;
	pkt->header.padding_length=size;
	if (pkt->cached_payload_size) {
		pkt->cached_payload_size += size - pkt->header.padding_length;
	}
}

void yang_rtpPacket_add_padding(YangRtpPacket* pkt,int32_t size) {
	if(pkt==NULL) return ;
	pkt->header.padding_length=pkt->header.padding_length + size;
	if (pkt->cached_payload_size) {
		pkt->cached_payload_size += size;
	}
}


