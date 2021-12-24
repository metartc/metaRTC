#include <yangrtp/YangRtp.h>
#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpRawPayload.h>
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yangavinfotype.h>



YangRtpPacket::YangRtpPacket() {
	memset(&m_header,0,sizeof(YangRtpHeader));
	m_payload_type = YangRtspPacketPayloadTypeUnknown;
	m_actual_buffer_size = 0;

	m_nalu_type = YangAvcNaluTypeReserved;
	m_frame_type = YangFrameTypeAudio;
	m_cached_payload_size = 0;

	m_payload=NULL;

	m_nb=0;


}

YangRtpPacket::~YangRtpPacket() {

	m_payload=NULL;

}
void YangRtpPacket::reset(){
	//m_header.reset();
	yang_delete(m_header.extensions);
	memset(&m_header,0,sizeof(YangRtpHeader));
	m_payload_type = YangRtspPacketPayloadTypeUnknown;
	m_actual_buffer_size = 0;

	m_nalu_type = YangAvcNaluTypeReserved;
	m_frame_type = YangFrameTypeAudio;
	m_cached_payload_size = 0;

	m_payload=NULL;
	m_nb=0;
}
char* YangRtpPacket::wrap(YangRtpBuffer* rtpb,char* data,int32_t nb){
	m_actual_buffer_size=nb;

	char* p=rtpb->getBuffer();
	memcpy(p,data,nb);
	return p;
}

void YangRtpPacket::set_padding(int32_t size) {
	m_header.padding_length=size;
	if (m_cached_payload_size) {
		m_cached_payload_size += size - m_header.padding_length;
	}
}

void YangRtpPacket::add_padding(int32_t size) {
	m_header.padding_length=m_header.padding_length + size;
	if (m_cached_payload_size) {
		m_cached_payload_size += size;
	}
}


bool YangRtpPacket::is_audio() {
	return m_frame_type == YangFrameTypeAudio;
}

void YangRtpPacket::set_extension_types(YangRtpExtensionTypes *v) {
	 if( m_header.extensions) m_header.extensions->set_types_(v);
}

int32_t YangRtpPacket::decode(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	if ((err = yang_decode_rtpHeader(buf,&m_header)) != Yang_Ok) {
		return yang_error_wrap(err, "rtp header");
	}


	uint8_t padding = m_header.padding_length;
	if (!buf->require(padding)) {
		return yang_error_wrap(err, "requires padding %d bytes", padding);
	}
	buf->set_size(buf->size() - padding);
	m_payload=buf->head();
	m_nb=buf->left();
    m_payload_type = YangRtspPacketPayloadTypeRaw;



	return err;
}





