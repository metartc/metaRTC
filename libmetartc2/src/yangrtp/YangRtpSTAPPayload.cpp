#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangutil/sys/YangLog.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yangavtype_h265.h>
YangSample* yang_get_h264_stap_sps(YangRtpSTAPData* data){
	if(data==NULL||data->nalus==NULL) return NULL;
	int32_t nn_nalus = (int) data->nalus->size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = data->nalus->at(i);
		if (!p || !p->nb) {
			continue;
		}

		YangAvcNaluType nalu_type = (YangAvcNaluType) (p->bytes[0] & kNalTypeMask);
		if (nalu_type == YangAvcNaluTypeSPS) {
			return p;
		}
	}

	return NULL;
}
YangSample* yang_get_h264_stap_pps(YangRtpSTAPData* data){
	if(data==NULL||data->nalus==NULL) return NULL;
	int32_t nn_nalus = (int) data->nalus->size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = data->nalus->at(i);
		if (!p || !p->nb) {
			continue;
		}

		YangAvcNaluType nalu_type = (YangAvcNaluType) (p->bytes[0] & kNalTypeMask);
		if (nalu_type == YangAvcNaluTypePPS) {
			return p;
		}
	}

	return NULL;
}

void yang_reset_h2645_stap(YangRtpSTAPData* data){
	if(data==NULL||data->nalus==NULL) return;
	int32_t nn_nalus = (int) data->nalus->size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = data->nalus->at(i);
		yang_delete(p);
	}
    data->nalus->clear();
}
int32_t yang_decode_h264_stap(YangBuffer* buf,YangRtpSTAPData* data){
	if(data==NULL||data->nalus==NULL) return yang_error_wrap(ERROR_RTC_RTP_MUXER, "decode stap nalu vector is null!");
	if (!buf->require(1)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		// STAP header, RTP payload format for aggregation packets
		// @see https://tools.ietf.org/html/rfc6184#section-5.7
		uint8_t v = buf->read_1bytes();

		// forbidden_zero_bit shoul be zero.
		// @see https://tools.ietf.org/html/rfc6184#section-5.3
		uint8_t f = (v & 0x80);
		if (f == 0x80) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"forbidden_zero_bit should be zero");
		}

		data->nri = YangAvcNaluType(v & (~kNalTypeMask));

		// NALUs.
		while (!buf->empty()) {
			if (!buf->require(2)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
			}

			int32_t size = buf->read_2bytes();
			if (!buf->require(size)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
						size);
			}

			YangSample *sample = new YangSample();
			sample->bytes = buf->head();
			sample->nb = size;
			buf->skip(size);

			if(data->nalus) data->nalus->push_back(sample);
		}

		return Yang_Ok;
}

int32_t yang_encode_h264_stap(YangBuffer* buf,YangRtpSTAPData* data){
	if(data==NULL||data->nalus==NULL) return yang_error_wrap(ERROR_RTC_RTP_MUXER, "stap nalu vector is null!");
	if (!buf->require(1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	uint8_t v = kStapA;
	v |= (data->nri & (~kNalTypeMask));
	buf->write_1bytes(v);

	// NALUs.
	int32_t nn_nalus = (int) data->nalus->size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p =data->nalus->at(i);

		if (!buf->require(2 + p->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					2 + p->nb);
		}

		buf->write_2bytes(p->nb);
		buf->write_bytes(p->bytes, p->nb);
	}

	return Yang_Ok;
}
int32_t yang_decode_h265_stap(YangBuffer* buf,YangRtpSTAPData* data){
	if(data==NULL||data->nalus==NULL) return yang_error_wrap(ERROR_RTC_RTP_MUXER, "decode stap nalu vector is null!");
	if (!buf->require(1)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		// STAP header, RTP payload format for aggregation packets
		// @see https://tools.ietf.org/html/rfc6184#section-5.7
		uint8_t v = buf->read_1bytes();

		// forbidden_zero_bit shoul be zero.
		// @see https://tools.ietf.org/html/rfc6184#section-5.3
		//uint8_t f = (v & 0x80);
		//if (f == 0x80) {
		//	return yang_error_wrap(ERROR_RTC_RTP_MUXER,					"forbidden_zero_bit should be zero");
		//}

		data->nri = YANG_HEVC_NALU_TYPE(v);//YangAvcNaluType(v & (~kNalTypeMask));

		// NALUs.
		while (!buf->empty()) {
			if (!buf->require(2)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
			}

			int32_t size = buf->read_2bytes();
			if (!buf->require(size)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
						size);
			}

			YangSample *sample = new YangSample();
			sample->bytes = buf->head();
			sample->nb = size;
			buf->skip(size);

			if(data->nalus) data->nalus->push_back(sample);
		}

		return Yang_Ok;
}

int32_t yang_encode_h265_stap(YangBuffer* buf,YangRtpSTAPData* data){
	if(data==NULL||data->nalus==NULL) return yang_error_wrap(ERROR_RTC_RTP_MUXER, "stap nalu vector is null!");
	if (!buf->require(1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	uint8_t v = H265_kStapA<<1;
	//v |= (data->nri & (~kNalTypeMask));
	buf->write_1bytes(v);

	// NALUs.
	int32_t nn_nalus = (int) data->nalus->size();
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p =data->nalus->at(i);

		if (!buf->require(2 + p->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					2 + p->nb);
		}

		buf->write_2bytes(p->nb);
		buf->write_bytes(p->bytes, p->nb);
	}

	return Yang_Ok;
}
YangSample* yang_get_h265_stap_sps(YangRtpSTAPData* data){
    if(data==NULL||data->nalus==NULL) return NULL;
    int32_t nn_nalus = (int) data->nalus->size();
    for (int32_t i = 0; i < nn_nalus; i++) {
        YangSample *p = data->nalus->at(i);
        if (!p || !p->nb) {
            continue;
        }

        YangHevcNaluType nalu_type = (YangHevcNaluType) YANG_HEVC_NALU_TYPE(p->bytes[0]);
        if (nalu_type == YANG_NAL_UNIT_SPS) {
            return p;
        }
    }

    return NULL;
}
YangSample* yang_get_h265_stap_pps(YangRtpSTAPData* data){
    if(data==NULL||data->nalus==NULL) return NULL;
    int32_t nn_nalus = (int) data->nalus->size();
    for (int32_t i = 0; i < nn_nalus; i++) {
        YangSample *p = data->nalus->at(i);
        if (!p || !p->nb) {
            continue;
        }

        YangHevcNaluType nalu_type = (YangHevcNaluType) YANG_HEVC_NALU_TYPE(p->bytes[0]);
        if (nalu_type == YANG_NAL_UNIT_PPS) {
            return p;
        }
    }

    return NULL;
}
YangSample* yang_get_h265_stap_vps(YangRtpSTAPData* data){
    if(data==NULL||data->nalus==NULL) return NULL;
    int32_t nn_nalus = (int) data->nalus->size();
    for (int32_t i = 0; i < nn_nalus; i++) {
        YangSample *p = data->nalus->at(i);
        if (!p || !p->nb) {
            continue;
        }

        YangHevcNaluType nalu_type = (YangHevcNaluType) YANG_HEVC_NALU_TYPE(p->bytes[0]);
        if (nalu_type == YANG_NAL_UNIT_VPS) {
            return p;
        }
    }

    return NULL;
}
