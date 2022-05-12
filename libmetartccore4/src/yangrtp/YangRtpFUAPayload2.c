//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtpFUAPayload2.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yangavtype.h>
#include <yangutil/sys/YangLog.h>
#include <memory.h>

int32_t yang_decode_h264_fua2(YangBuffer* buf,Fua2H264DataCache* pkt){
	if (!yang_buffer_require(buf,2)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
		}
	// @see: https://tools.ietf.org/html/rfc6184#section-5.8
	//const uint8_t kStart            = 0x80; // Fu-header start bit
	//const uint8_t kEnd              = 0x40; // Fu-header end bit
		// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t v = yang_read_1bytes(buf);
		pkt->nri = (YangAvcNaluType)(v & (~kNalTypeMask));

		// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
		v = yang_read_1bytes(buf);
		pkt->start = v & kStart;
		pkt->end = v & kEnd;
		pkt->nalu_type = (YangAvcNaluType)(v & kNalTypeMask);
		if (!yang_buffer_require(buf,1)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}

		return Yang_Ok;
}

int32_t yang_encode_h264_fua2(YangBuffer* buf,YangFua2H264Data* pkt){
	if (!yang_buffer_require(buf,2 + pkt->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
		}
	// @see: https://tools.ietf.org/html/rfc6184#section-5.8
	//const uint8_t kStart            = 0x80; // Fu-header start bit
	//const uint8_t kEnd              = 0x40; // Fu-header end bit
	//const uint8_t kFuA              = 28;
		// Fast encoding.
		char *p = buf->head;

		// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t fu_indicate = kFuA;
		fu_indicate |= (pkt->nri & (~kNalTypeMask));
		*p++ = fu_indicate;

		// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t fu_header = pkt->nalu_type;
		if (pkt->start) {
			fu_header |= kStart;
		}
		if (pkt->end) {
			fu_header |= kEnd;
		}
		*p++ = fu_header;

		// FU payload, @see https://tools.ietf.org/html/rfc6184#section-5.8
		memcpy(p, pkt->payload, pkt->nb);

		// Consume bytes.
		yang_buffer_skip(buf,2 + pkt->nb);

		return Yang_Ok;
}
int32_t yang_decode_h265_fua2(YangBuffer* buf,Fua2H265DataCache* pkt){
	if (!yang_buffer_require(buf,3)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "decode_h265_fua2 requires %d bytes", 3);
		}
	// @see: https://tools.ietf.org/html/rfc6184#section-5.8

		// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t v = yang_read_1bytes(buf);
        pkt->nri =  (YangHevcNaluType)((v>>1) & 0x3f);	// YangHevcNaluType(v & (~kNalTypeMask));
		v=yang_read_1bytes(buf);
		// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
		v = yang_read_1bytes(buf);
		pkt->start = v & kStart;
		pkt->end = v & kEnd;
		pkt->nalu_type = (YangHevcNaluType)(v & 0x3F);
		if (!yang_buffer_require(buf,1)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "decode_h265_fua2 requires %d bytes", 1);
		}

		return Yang_Ok;
}
int32_t yang_encode_h265_fua2(YangBuffer* buf,YangFua2H265Data* pkt){
	if (!yang_buffer_require(buf,3 + pkt->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 3 + pkt->nb);
		}
	// @see: https://tools.ietf.org/html/rfc6184#section-5.8

		// Fast encoding.
		char *p = buf->head;

		// FU indicator, @see https://tools.ietf.org/html/rfc6184#section-5.8
		//uint8_t fu_indicate = 49 << 1;
		//fu_indicate |= (pkt->nri & (~kNalTypeMask));
		//uint8_t nal_type = (pkt->nri >> 1) & 0x3F;
		*p++=H265_kFuA<<1;//49<<1;
		*p++=1;
		//*p++ = nal_type|(1 << 7);

		// FU header, @see https://tools.ietf.org/html/rfc6184#section-5.8
		uint8_t fu_header = pkt->nalu_type;
		if (pkt->start) {
			fu_header |= kStart;
		}
		if (pkt->end) {
			fu_header |= kEnd;
		}
		*p++ = fu_header;

		// FU payload, @see https://tools.ietf.org/html/rfc6184#section-5.8
		memcpy(p, pkt->payload, pkt->nb);

		// Consume bytes.
		yang_buffer_skip(buf,3 + pkt->nb);

		return Yang_Ok;
}
