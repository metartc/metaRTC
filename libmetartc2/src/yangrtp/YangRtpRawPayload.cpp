#include <yangrtp/YangRtpRawPayload.h>
#include <yangutil/sys/YangLog.h>
int32_t yang_encode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt){
	if (pkt->nb <= 0) {
			return Yang_Ok;
		}

		if (!buf->require(pkt->nb)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes",pkt->nb);
		}

		buf->write_bytes(pkt->payload, pkt->nb);

		return Yang_Ok;
}
int32_t yang_decode_h264_raw(YangBuffer* buf,YangRtpRawData* pkt){
	if (buf->empty()) {
		return Yang_Ok;
	}

	pkt->payload = buf->head();
	pkt->nb = buf->left();

	return Yang_Ok;
}

