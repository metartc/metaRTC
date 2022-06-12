//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtpRawPayload.h>
#include <yangutil/sys/YangLog.h>
int32_t yang_encode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt){
	if (pkt->nb <= 0) {
			return Yang_Ok;
		}

		if (!yang_buffer_require(buf,pkt->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",pkt->nb);
		}

		yang_write_bytes(buf,pkt->payload, pkt->nb);

		return Yang_Ok;
}
int32_t yang_decode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt){
	if (yang_buffer_empty(buf)) {
		return Yang_Ok;
	}

	pkt->payload = buf->head;
	pkt->nb = yang_buffer_left(buf);

	return Yang_Ok;
}

