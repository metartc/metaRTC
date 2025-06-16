//
// Copyright (c) 2019-2025 yanggaofeng
//
#include <yangrtp/YangRtpSTAPPayload.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/sys/YangLog.h>

#include <yangutil/yangavinfo.h>

yang_vector_impl(YangSample)


void yang_create_stap(YangRtpSTAPData* stap){
	if(stap==NULL)
		return;

	yang_create_YangSampleVector(&stap->nalus);
}

void yang_destroy_stap(YangRtpSTAPData* stap){
	if(stap==NULL)
		return;

	yang_destroy_YangSampleVector(&stap->nalus);
}

YangSample* yang_get_h264_stap_sps(YangRtpSTAPData* data){
	int32_t nn_nalus,i;
	YangAvcNaluType nalu_type;
	YangSample *sample;

	if(data==NULL)
		return NULL;

	nn_nalus = (int32_t) data->nalus.vsize;

	for (i = 0; i < nn_nalus; i++) {
		sample = &data->nalus.payload[i];
		if (!sample || !sample->nb) {
			continue;
		}

		nalu_type = (YangAvcNaluType) (sample->bytes[0] & kNalTypeMask);
		if (nalu_type == YangAvcNaluTypeSPS) {
			return sample;
		}
	}

	return NULL;
}

YangSample* yang_get_h264_stap_pps(YangRtpSTAPData* data){
	int32_t nn_nalus,i;
	YangAvcNaluType nalu_type;
	YangSample *sample;

	if(data==NULL)
		return NULL;

	nn_nalus = (int) data->nalus.vsize;
	for (i = 0; i < nn_nalus; i++) {
		sample = &data->nalus.payload[i];

		if (!sample || !sample->nb) {
			continue;
		}

		nalu_type = (YangAvcNaluType) (sample->bytes[0] & kNalTypeMask);

		if (nalu_type == YangAvcNaluTypePPS) {
			return sample;
		}
	}

	return NULL;
}

void yang_reset_h2645_stap(YangRtpSTAPData* data){
	if(data==NULL||data->nalus.vsize==0)
		return;

	data->nb=0;
	data->nri=0;
	data->payload=NULL;
	yang_clear_YangSampleVector(&data->nalus);

}

int32_t yang_decode_h264_stap(YangBuffer* buf,YangRtpSTAPData* data){
	uint8_t v,f;
	int32_t size;

	if(data==NULL)
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "decode stap nalu vector is null!");

	if (!yang_buffer_require(buf,1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	v = yang_read_1bytes(buf);

	// forbidden_zero_bit shoul be zero.
	// @see https://tools.ietf.org/html/rfc6184#section-5.3
	f = (v & 0x80);
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

		size = yang_read_2bytes(buf);
		if (!yang_buffer_require(buf,size)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					size);
		}

		YangSample sample;
		sample.bytes = buf->head;
		sample.nb = size;
		yang_buffer_skip(buf,size);
		yang_insert_YangSampleVector(&data->nalus,&sample);
	}

	return Yang_Ok;
}

int32_t yang_encode_h264_stap(YangBuffer* buf,YangRtpSTAPData* data){
	uint8_t v;
	int32_t nn_nalus,i;
	YangSample *sample;

	if(data==NULL)
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "stap nalu vector is null!");

	if (!yang_buffer_require(buf,1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	v = 24;
	v |= (data->nri & (~kNalTypeMask));
	yang_write_1bytes(buf,v);

	// NALUs.
	nn_nalus = (int) data->nalus.vsize;
	for (i = 0; i < nn_nalus; i++) {
		sample =&data->nalus.payload[i];

		if (!yang_buffer_require(buf,2 + sample->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					2 + sample->nb);
		}

		yang_write_2bytes(buf,sample->nb);
		yang_write_bytes(buf,sample->bytes, sample->nb);
	}

	return Yang_Ok;
}

int32_t yang_decode_h265_stap(YangBuffer* buf,YangRtpSTAPData* data){
	uint8_t v;
	int32_t size;

	if(data==NULL)
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "decode stap nalu vector is null!");

	if (!yang_buffer_require(buf,1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	v = yang_read_1bytes(buf);
	yang_read_1bytes(buf);
	// forbidden_zero_bit shoul be zero.
	// @see https://tools.ietf.org/html/rfc6184#section-5.3
	data->nri = YANG_HEVC_NALU_TYPE(v);

	// NALUs.
	while (!yang_buffer_empty(buf)) {
		if (!yang_buffer_require(buf,2)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
		}

		size = yang_read_2bytes(buf);

		if (!yang_buffer_require(buf,size)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					size);
		}

		YangSample sample ;//= new YangSample();
		sample.bytes = buf->head;
		sample.nb = size;
		yang_buffer_skip(buf,size);

		yang_insert_YangSampleVector(&data->nalus,&sample);

	}

	return Yang_Ok;
}

int32_t yang_encode_h265_stap(YangBuffer* buf,YangRtpSTAPData* data){
	uint8_t v;
	int32_t nn_nalus,i;

	YangSample *sample;

	if(data==NULL) 
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "stap nalu vector is null!");

	if (!yang_buffer_require(buf,1)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	}

	// STAP header, RTP payload format for aggregation packets
	// @see https://tools.ietf.org/html/rfc6184#section-5.7
	v = H265_kStapA<<1;

	yang_write_1bytes(buf,v);
	yang_write_1bytes(buf,0x00);
	// NALUs.
	nn_nalus = (int) data->nalus.vsize;
	for (i = 0; i < nn_nalus; i++) {
		sample =&data->nalus.payload[i];

		if (!yang_buffer_require(buf,2 + sample->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",
					2 + sample->nb);
		}

		yang_write_2bytes(buf,sample->nb);
		yang_write_bytes(buf,sample->bytes, sample->nb);
	}

	return Yang_Ok;
}

YangSample* yang_get_h265_stap_sps(YangRtpSTAPData* data){
	int32_t nn_nalus,i;
	YangHevcNaluType nalu_type;

	YangSample *sample;

	if(data==NULL||data->nalus.vsize==0)
		return NULL;

	nn_nalus = (int) data->nalus.vsize;
	for (i = 0; i < nn_nalus; i++) {
		sample = &data->nalus.payload[i];
		if (!sample || !sample->nb) {
			continue;
		}

		nalu_type = (YangHevcNaluType) YANG_HEVC_NALU_TYPE(sample->bytes[0]);
		if (nalu_type == YANG_NAL_UNIT_SPS) {
			return sample;
		}
	}

	return NULL;
}


YangSample* yang_get_h265_stap_pps(YangRtpSTAPData* data){
	int32_t nn_nalus,i;
	YangHevcNaluType nalu_type;

	YangSample *sample;

	if(data==NULL||data->nalus.vsize==0)
		return NULL;

	nn_nalus = (int32_t) data->nalus.vsize;
	for (i = 0; i < nn_nalus; i++) {
		sample = &data->nalus.payload[i];
		if (!sample || !sample->nb) {
			continue;
		}

		nalu_type = (YangHevcNaluType) YANG_HEVC_NALU_TYPE(sample->bytes[0]);
		if (nalu_type == YANG_NAL_UNIT_PPS) {
			return sample;
		}
	}

	return NULL;
}

YangSample* yang_get_h265_stap_vps(YangRtpSTAPData* data){
	int32_t nn_nalus,i;
	YangHevcNaluType nalu_type;

	YangSample *sample;
	if(data==NULL||data->nalus.vsize==0)
		return NULL;

	nn_nalus = (int) data->nalus.vsize;
	for (i = 0; i < nn_nalus; i++) {
		sample = &data->nalus.payload[i];
		if (!sample || !sample->nb) {
			continue;
		}

		nalu_type = (YangHevcNaluType) YANG_HEVC_NALU_TYPE(sample->bytes[0]);
		if (nalu_type == YANG_NAL_UNIT_VPS) {
			return sample;
		}
	}

	return NULL;
}
