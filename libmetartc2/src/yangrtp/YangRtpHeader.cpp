#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
int32_t parse_extensions(YangBuffer *buf,YangRtpExtensions* ext) {
	int32_t err = Yang_Ok;

	if (Yang_Ok != (err = ext->decode(buf))) {
		return yang_error_wrap(err, "decode rtp extension");
	}

	return err;
}
int32_t yang_encode_rtpHeader(YangBuffer* buf,YangRtpHeader* header){
	int32_t err = Yang_Ok;

		// Encode the RTP fix header, 12bytes.
		// @see https://tools.ietf.org/html/rfc1889#section-5.1
		// The version, padding, extension and cc, total 1 byte.
		uint8_t v = 0x80 | header->cc;
		if (header->padding_length > 0) {
			v |= 0x20;
		}
		if (header->extensions&&header->extensions->exists()) {
			v |= 0x10;
		}
		buf->write_1bytes(v);

		// The marker and payload type, total 1 byte.
		v = header->payload_type;
		if (header->marker) {
			v |= kRtpMarker;
		}
		buf->write_1bytes(v);

		// The sequence number, 2 bytes.
		buf->write_2bytes(header->sequence);

		// The timestamp, 4 bytes.
		buf->write_4bytes(header->timestamp);

		// The SSRC, 4 bytes.
		buf->write_4bytes(header->ssrc);

		// The CSRC list: 0 to 15 items, each is 4 bytes.
		for (size_t i = 0; i < header->cc; ++i) {
			buf->write_4bytes(header->csrc[i]);
		}

		if (header->extensions&&header->extensions->exists()) {
			if (Yang_Ok != (err = header->extensions->encode(buf))) {
				return yang_error_wrap(err, "encode rtp extension");
			}
		}

		return err;
}
int32_t yang_decode_rtpHeader(YangBuffer* buf,YangRtpHeader* header){
	int32_t err = Yang_Ok;

		if (!buf->require(kRtpHeaderFixedSize)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d+ bytes",
					kRtpHeaderFixedSize);
		}

		/* @see https://tools.ietf.org/html/rfc1889#section-5.1
		 0                   1                   2                   3
		 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 |V=2|P|X|  CC   |M|     PT      |       sequence number         |
		 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 |                           timestamp                           |
		 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 |           synchronization source (SSRC) identifier            |
		 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
		 |            contributing source (CSRC) identifiers             |
		 |                             ....                              |
		 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 */

		uint8_t first = buf->read_1bytes();
		bool padding = (first & 0x20);
		bool extension = (first & 0x10);
		header->cc = (first & 0x0F);

		uint8_t second = buf->read_1bytes();
		header->marker = (second & 0x80);
		header->payload_type = (second & 0x7F);

		header->sequence = buf->read_2bytes();
		header->timestamp = buf->read_4bytes();
		header->ssrc = buf->read_4bytes();



		for (uint8_t i = 0; i < header->cc; ++i) {
			header->csrc[i] = buf->read_4bytes();
		}

		if (extension) {
			if(header->extensions==NULL) header->extensions=new YangRtpExtensions();
			if ((err = parse_extensions(buf,header->extensions)) != Yang_Ok) {
				return yang_error_wrap(err, "fail to parse extension");
			}
		}

		if (padding && !header->ignore_padding && !buf->empty()) {
			header->padding_length = *(reinterpret_cast<uint8_t*>(buf->data() + buf->size()
					- 1));
			if (!buf->require(header->padding_length)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER,
						"padding requires %d bytes",header->padding_length);
			}
		}

		return err;
}
/**

YangRtpHeader::YangRtpHeader() {
	reset();
}

YangRtpHeader::~YangRtpHeader() {
}

void YangRtpHeader::reset(){
	m_cc = 0;
	m_marker = false;
	m_payload_type = 0;
	m_sequence = 0;
	m_timestamp = 0;
	m_ssrc = 0;
	m_padding_length = 0;
	m_ignore_padding = false;
	memset(m_csrc, 0, sizeof(m_csrc));
}
int32_t YangRtpHeader::decode(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	if (!buf->require(kRtpHeaderFixedSize)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d+ bytes",
				kRtpHeaderFixedSize);
	}
**/
	/* @see https://tools.ietf.org/html/rfc1889#section-5.1
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |V=2|P|X|  CC   |M|     PT      |       sequence number         |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                           timestamp                           |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |           synchronization source (SSRC) identifier            |
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 |            contributing source (CSRC) identifiers             |
	 |                             ....                              |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
/**
	uint8_t first = buf->read_1bytes();
	bool padding = (first & 0x20);
	bool extension = (first & 0x10);
	m_cc = (first & 0x0F);

	uint8_t second = buf->read_1bytes();
	m_marker = (second & 0x80);
	m_payload_type = (second & 0x7F);

	m_sequence = buf->read_2bytes();
	m_timestamp = buf->read_4bytes();
	m_ssrc = buf->read_4bytes();

	int32_t ext_bytes = nb_bytes() - kRtpHeaderFixedSize;
	if (!buf->require(ext_bytes)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d+ bytes",
				ext_bytes);
	}

	for (uint8_t i = 0; i < m_cc; ++i) {
		m_csrc[i] = buf->read_4bytes();
	}

	if (extension) {
		if ((err = parse_extensions(buf)) != Yang_Ok) {
			return yang_error_wrap(err, "fail to parse extension");
		}
	}

	if (padding && !m_ignore_padding && !buf->empty()) {
		m_padding_length = *(reinterpret_cast<uint8_t*>(buf->data() + buf->size()
				- 1));
		if (!buf->require(m_padding_length)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"padding requires %d bytes", m_padding_length);
		}
	}

	return err;
}

int32_t YangRtpHeader::parse_extensions(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	if (Yang_Ok != (err = m_extensions.decode(buf))) {
		return yang_error_wrap(err, "decode rtp extension");
	}

	return err;
}

int32_t YangRtpHeader::encode(YangBuffer *buf) {
	int32_t err = Yang_Ok;

	// Encode the RTP fix header, 12bytes.
	// @see https://tools.ietf.org/html/rfc1889#section-5.1
	// The version, padding, extension and cc, total 1 byte.
	uint8_t v = 0x80 | m_cc;
	if (m_padding_length > 0) {
		v |= 0x20;
	}
	if (m_extensions.exists()) {
		v |= 0x10;
	}
	buf->write_1bytes(v);

	// The marker and payload type, total 1 byte.
	v = m_payload_type;
	if (m_marker) {
		v |= kRtpMarker;
	}
	buf->write_1bytes(v);

	// The sequence number, 2 bytes.
	buf->write_2bytes(m_sequence);

	// The timestamp, 4 bytes.
	buf->write_4bytes(m_timestamp);

	// The SSRC, 4 bytes.
	buf->write_4bytes(m_ssrc);

	// The CSRC list: 0 to 15 items, each is 4 bytes.
	for (size_t i = 0; i < m_cc; ++i) {
		buf->write_4bytes(m_csrc[i]);
	}

	if (m_extensions.exists()) {
		if (Yang_Ok != (err = m_extensions.encode(buf))) {
			return yang_error_wrap(err, "encode rtp extension");
		}
	}

	return err;
}
void YangRtpHeader::set_extensions(YangRtpExtensionTypes *extmap) {
	if (extmap) {
		m_extensions.set_types_(extmap);
	}
}

void YangRtpHeader::ignore_padding(bool v) {
	m_ignore_padding = v;
}

int32_t YangRtpHeader::get_twcc_sequence_number(uint16_t &twcc_sn) {
	if (m_extensions.exists()) {
		return m_extensions.get_twcc_sequence_number(twcc_sn);
	}
	return yang_error_wrap(ERROR_RTC_RTP_MUXER, "no rtp extension");
}

int32_t YangRtpHeader::set_twcc_sequence_number(uint8_t id, uint16_t sn) {
	return m_extensions.set_twcc_sequence_number(id, sn);
}

uint64_t YangRtpHeader::nb_bytes() {
	return kRtpHeaderFixedSize + m_cc * 4
			+ (m_extensions.exists() ? m_extensions.nb_bytes() : 0);
}

void YangRtpHeader::set_marker(bool v) {
	m_marker = v;
}

bool YangRtpHeader::get_marker() const {
	return m_marker;
}

void YangRtpHeader::set_payload_type(uint8_t v) {
	m_payload_type = v;
}

uint8_t YangRtpHeader::get_payload_type() const {
	return m_payload_type;
}

void YangRtpHeader::set_sequence(uint16_t v) {
	m_sequence = v;
}

uint16_t YangRtpHeader::get_sequence() const {
	return m_sequence;
}

void YangRtpHeader::set_timestamp(uint32_t v) {
	m_timestamp = v;
}

uint32_t YangRtpHeader::get_timestamp() const {
	return m_timestamp;
}

void YangRtpHeader::set_ssrc(uint32_t v) {
	m_ssrc = v;
}

void YangRtpHeader::set_padding(uint8_t v) {
	m_padding_length = v;
}

uint8_t YangRtpHeader::get_padding() const {
	return m_padding_length;
}
**/
