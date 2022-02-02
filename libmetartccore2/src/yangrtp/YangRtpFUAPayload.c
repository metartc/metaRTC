#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpFUAPayload.h>
#include <yangutil/sys/YangLog.h>
/**
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
	if (!yang_buffer_require(buf,2)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t fu_indicate = kFuA;
	fu_indicate |= (m_nri & (~kNalTypeMask));
	yang_write_1bytes(buf,fu_indicate);

	// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t fu_header = m_nalu_type;
	if (m_start) {
		fu_header |= kStart;
	}
	if (m_end) {
		fu_header |= kEnd;
	}
	yang_write_1bytes(buf,fu_header);

	// FU payload, @see https://tools.ietf.org/html/rfc6184#section-5.8
	int32_t nn_nalus = (int) m_nalus.size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = m_nalus[i];

		if (!yang_buffer_require(buf,p->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					p->nb);
		}

		yang_write_bytes(buf,p->bytes, p->nb);
	}

	return Yang_Ok;
}

int32_t YangRtpFUAPayload::decode(YangBuffer *buf) {

	if (!yang_buffer_require(buf,2)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
	}

	// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
	uint8_t v = yang_read_1bytes(buf);
	m_nri = YangAvcNaluType(v & (~kNalTypeMask));

	// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
	v = yang_read_1bytes(buf);
	m_start = v & kStart;
	m_end = v & kEnd;
	m_nalu_type = YangAvcNaluType(v & kNalTypeMask);

	if (!yang_buffer_require(buf,1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	YangSample *sample = new YangSample();
	sample->bytes = buf->head;
	sample->nb = yang_buffer_left(buf);
	yang_buffer_skip(buf,sample->nb);

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
**/
