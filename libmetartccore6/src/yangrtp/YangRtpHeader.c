//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtpHeader.h>
#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangLog.h>


int32_t parse_extensions(YangBuffer *buf,YangRtpExtensions* ext) {
	int32_t err = Yang_Ok;

	if (Yang_Ok != (err = yang_decode_rtpExtensions(ext,buf))) {
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
	if (header->extensions&&header->extensions->has_ext) {
		v |= 0x10;
	}

	yang_write_1bytes(buf,v);

	// The marker and payload type, total 1 byte.
	v = header->payload_type;
	if (header->marker) {
		v |= kRtpMarker;
	}

	yang_write_1bytes(buf,v);

	// The sequence number, 2 bytes.
	yang_write_2bytes(buf,header->sequence);

	// The timestamp, 4 bytes.
	yang_write_4bytes(buf,header->timestamp);

	// The SSRC, 4 bytes.
	yang_write_4bytes(buf,header->ssrc);

	// The CSRC list: 0 to 15 items, each is 4 bytes.
	for (size_t i = 0; i < header->cc; ++i) {
		yang_write_4bytes(buf,header->csrc[i]);
	}

	if (header->extensions&&header->extensions->has_ext) {

		if (Yang_Ok != (err = yang_encode_rtpExtensions(header->extensions,buf))) {
			return yang_error_wrap(err, "encode rtp extension");
		}
	}

	return err;
}
int32_t yang_decode_rtpHeader(YangBuffer* buf,YangRtpHeader* header){
	int32_t err = Yang_Ok;

	if (!yang_buffer_require(buf,12)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d+ bytes",
				12);
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

	uint8_t first = yang_read_1bytes(buf);
	yangbool padding = (first & 0x20);
	yangbool extension = (first & 0x10);
	header->cc = (first & 0x0F);

	uint8_t second = yang_read_1bytes(buf);
	header->marker = (second & 0x80)==0?0:1;
	header->payload_type = (second & 0x7F);

	header->sequence = yang_read_2bytes(buf);
	header->timestamp = yang_read_4bytes(buf);
	header->ssrc = yang_read_4bytes(buf);

	for (uint8_t i = 0; i < header->cc; ++i) {
		header->csrc[i] = yang_read_4bytes(buf);
	}

	if (extension) {

		if(header->extensions==NULL) header->extensions=(YangRtpExtensions*)yang_calloc(1,sizeof(YangRtpExtensions));
		if ((err = parse_extensions(buf,header->extensions)) != Yang_Ok) {
			return yang_error_wrap(err, "fail to parse extension");
		}
	}

	if (padding && !header->ignore_padding && !yang_buffer_empty(buf)) {
		header->padding_length = *((uint8_t*)(buf->data + buf->size - 1));
		if (!yang_buffer_require(buf,header->padding_length)) {
			return yang_error_wrap(ERROR_RTC_RTP_MUXER,
					"padding requires %d bytes",header->padding_length);
		}
	}

	return err;
}

