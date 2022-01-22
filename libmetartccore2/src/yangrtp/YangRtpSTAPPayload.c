#include <yangutil/sys/YangLog.h>
#include <yangutil/yangavtype.h>
#include <yangutil/yangavtype_h265.h>
#include <string.h>

#include <yangrtp/YangCRtpSTAPPayload.h>
#include <yangrtp/YangRtpConstant.h>

void yang_inti_sampleVector(YangSampleVector* vec){
	if(vec==NULL) return;
	vec->allLen=5;
	vec->vlen=0;
	if(vec->samples==NULL) vec->samples=(char*)calloc(1,sizeof(YangSample)*5);

}
void yang_destroy_sampleVector(YangSampleVector* vec){
	if(vec==NULL) return;
	yang_free(vec->samples);
}
void yang_push_sampleVector(YangSampleVector* nalus,YangSample* sample){
	if(nalus==NULL||nalus->samples==NULL||sample==NULL) return;
    memcpy(nalus->samples+(nalus->vlen*sizeof(YangSample)),sample,sizeof(YangSample));
    nalus->vlen++;
}

YangSample* yang_get_sampleVector(YangSampleVector* vec,int32_t index){
	if(vec==NULL||index>=vec->vlen) return NULL;
	return (YangSample*)(vec->samples+index*sizeof(YangSample));
}
YangSample* yang_get_h264_stap_sps(YangRtpSTAPData* data){
	if(data==NULL||data->nalus.samples==NULL) return NULL;
	int32_t nn_nalus = (int) data->nalus.vlen;
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = yang_get_sampleVector(&data->nalus,i);
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
	if(data==NULL||data->nalus.samples==NULL) return NULL;
	int32_t nn_nalus = (int) data->nalus.vlen;
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p = yang_get_sampleVector(&data->nalus,i);
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
	if(data==NULL||data->nalus.samples==NULL||data->nalus.vlen==0) return;
	memset(data->nalus.samples,0,sizeof(YangSample)*data->nalus.vlen);
    data->nalus.vlen=0;
}
int32_t yang_decode_h264_stap(YangBuffer* buf,YangRtpSTAPData* data){
	if(data==NULL||data->nalus.samples==NULL) return yang_error_wrap(ERROR_RTC_RTP_MUXER, "decode stap nalu vector is null!");
	if (!yang_buffer_require(buf,1)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		// STAP header, RTP payload format for aggregation packets
		// @see https://tools.ietf.org/html/rfc6184#section-5.7
		uint8_t v = yang_read_1bytes(buf);

		// forbidden_zero_bit shoul be zero.
		// @see https://tools.ietf.org/html/rfc6184#section-5.3
		uint8_t f = (v & 0x80);
		if (f == 0x80) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"forbidden_zero_bit should be zero");
		}

		data->nri = v & (~kNalTypeMask);

		// NALUs.
		while (!yang_buffer_empty(buf)) {
			if (!yang_buffer_require(buf,2)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
			}

			int32_t size = yang_read_2bytes(buf);
			if (!yang_buffer_require(buf,size)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
						size);
			}

			YangSample sample;// = new YangSample();
			sample.bytes = buf->head;
			sample.nb = size;
			yang_buffer_skip(buf,size);
			//if(data->nalus) data->nalus.push_back(sample);
			yang_push_sampleVector(&data->nalus,&sample);
		}

		return Yang_Ok;
}

int32_t yang_encode_h264_stap(YangBuffer* buf,YangRtpSTAPData* data){
	if(data==NULL||data->nalus.samples==NULL) return yang_error_wrap(ERROR_RTC_RTP_MUXER, "stap nalu vector is null!");
	if (!yang_buffer_require(buf,1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	uint8_t v = 24;
	v |= (data->nri & (~kNalTypeMask));
	yang_write_1bytes(buf,v);

	// NALUs.
	int32_t nn_nalus = (int) data->nalus.vlen;
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p =yang_get_sampleVector(&data->nalus,i);//(YangSample*)(data->nalus.samples+sizeof(YangSample)*i);

		if (!yang_buffer_require(buf,2 + p->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					2 + p->nb);
		}

		yang_write_2bytes(buf,p->nb);
		yang_write_bytes(buf,p->bytes, p->nb);
	}

	return Yang_Ok;
}
int32_t yang_decode_h265_stap(YangBuffer* buf,YangRtpSTAPData* data){
	if(data==NULL||data->nalus.samples==NULL) return yang_error_wrap(ERROR_RTC_RTP_MUXER, "decode stap nalu vector is null!");
	if (!yang_buffer_require(buf,1)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		// STAP header, RTP payload format for aggregation packets
		// @see https://tools.ietf.org/html/rfc6184#section-5.7
		uint8_t v = yang_read_1bytes(buf);

		// forbidden_zero_bit shoul be zero.
		// @see https://tools.ietf.org/html/rfc6184#section-5.3
		//uint8_t f = (v & 0x80);
		//if (f == 0x80) {
		//	return yang_error_wrap(ERROR_RTC_RTP_MUXER,					"forbidden_zero_bit should be zero");
		//}

		data->nri = YANG_HEVC_NALU_TYPE(v);//YangAvcNaluType(v & (~kNalTypeMask));

		// NALUs.
		while (!yang_buffer_empty(buf)) {
			if (!yang_buffer_require(buf,2)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
			}

			int32_t size = yang_read_2bytes(buf);
			if (!yang_buffer_require(buf,size)) {
				return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
						size);
			}

			YangSample sample ;//= new YangSample();
			sample.bytes = buf->head;
			sample.nb = size;
			yang_buffer_skip(buf,size);

			//if(data->nalus) data->nalus.push_back(sample);
			yang_push_sampleVector(&data->nalus,&sample);

		}

		return Yang_Ok;
}

int32_t yang_encode_h265_stap(YangBuffer* buf,YangRtpSTAPData* data){
	if(data==NULL||data->nalus.samples==NULL) return yang_error_wrap(ERROR_RTC_RTP_MUXER, "stap nalu vector is null!");
	if (!yang_buffer_require(buf,1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	uint8_t v = H265_kStapA<<1;

	yang_write_1bytes(buf,v);

	// NALUs.
	int32_t nn_nalus = (int) data->nalus.vlen;
	for (int32_t i = 0; i < nn_nalus; i++) {
		YangSample *p =yang_get_sampleVector(&data->nalus,i);

		if (!yang_buffer_require(buf,2 + p->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					2 + p->nb);
		}

		yang_write_2bytes(buf,p->nb);
		yang_write_bytes(buf,p->bytes, p->nb);
	}

	return Yang_Ok;
}
YangSample* yang_get_h265_stap_sps(YangRtpSTAPData* data){
    if(data==NULL||data->nalus.samples==NULL) return NULL;
    int32_t nn_nalus = (int) data->nalus.vlen;
    for (int32_t i = 0; i < nn_nalus; i++) {
        YangSample *p = yang_get_sampleVector(&data->nalus,i);
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
    if(data==NULL||data->nalus.samples==NULL) return NULL;
    int32_t nn_nalus = (int) data->nalus.vlen;
    for (int32_t i = 0; i < nn_nalus; i++) {
        YangSample *p = yang_get_sampleVector(&data->nalus,i);
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
    if(data==NULL||data->nalus.samples==NULL) return NULL;
    int32_t nn_nalus = (int) data->nalus.vlen;
    for (int32_t i = 0; i < nn_nalus; i++) {
        YangSample *p = yang_get_sampleVector(&data->nalus,i);
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
