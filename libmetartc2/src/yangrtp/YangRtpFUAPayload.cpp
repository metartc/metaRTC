#include <yangrtp/YangRtpFUAPayload.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>
YangRtpFUAPayload::YangRtpFUAPayload() {
	m_start = m_end = false;
	m_nri = m_nalu_type = (YangAvcNaluType) 0;
}

YangRtpFUAPayload::~YangRtpFUAPayload() {
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		yang_delete(p);
	}
}

uint64_t YangRtpFUAPayload::nb_bytes() {
	int32_t size = 2;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		size += p->nb;
	}

	return size;
}

int32_t YangRtpFUAPayload::encode(YangBuffer *buf) {
	if (!buf->require(2)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t fu_indicate = kFuA;
	fu_indicate |= (m_nri & (~kNalTypeMask));
	buf->write_1bytes(fu_indicate);

	// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t fu_header = m_nalu_type;
	if (m_start) {
		fu_header |= kStart;
	}
	if (m_end) {
		fu_header |= kEnd;
	}
	buf->write_1bytes(fu_header);

	// FU payload, @see https://tools.ietf.org/html/rfc6184#section-5.8
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];

		if (!buf->require(p->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					p->nb);
		}

		buf->write_bytes(p->bytes, p->nb);
	}

	return Yang_Ok;
}

int32_t YangRtpFUAPayload::decode(YangBuffer *buf) {

	if (!buf->require(2)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
	}

	// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t v = buf->read_1bytes();
	m_nri = YangAvcNaluType(v & (~kNalTypeMask));

	// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
	v = buf->read_1bytes();
	m_start = v & kStart;
	m_end = v & kEnd;
	m_nalu_type = YangAvcNaluType(v & kNalTypeMask);

	if (!buf->require(1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	YangSample *sample = new YangSample();
	sample->bytes = buf->head();
	sample->nb = buf->left();
	buf->skip(sample->nb);

	m_nalus.push_back(sample);

	return Yang_Ok;
}

IYangRtpPayloader* YangRtpFUAPayload::copy() {
	YangRtpFUAPayload *cp = new YangRtpFUAPayload();

	cp->m_nri = m_nri;
	cp->m_start = m_start;
	cp->m_end = m_end;
	cp->m_nalu_type = m_nalu_type;

	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];
		cp->m_nalus.push_back(yang_sample_copy(p));
	}

	return cp;
}
