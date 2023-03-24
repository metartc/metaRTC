//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangsdp/YangMediaPayloadType.h>
#include <yangutil/sys/YangCString.h>

#define kCRLF  "\r\n"

yang_vector_impl(YangMediaPayloadType)

int32_t yang_encode_mediapayloadtype(YangMediaPayloadType *payload,
		YangBuffer *os) {
	int32_t err = Yang_Ok;
	char intstr[32];
	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(payload->payload_type, intstr, 10);
	yang_write_cstring(os, "a=rtpmap:");
	yang_write_cstring(os, intstr);
	yang_write_cstring(os, " ");
	yang_write_cstring(os, payload->encoding_name);
	yang_write_cstring(os, "/");
	yang_memset(intstr, 0, sizeof(intstr));
	yang_itoa(payload->clock_rate, intstr, 10);
	yang_write_cstring(os, intstr);

	if (yang_strlen(payload->encoding_param)) {
		yang_write_cstring(os, "/");
		yang_write_cstring(os, payload->encoding_param);

	}
	yang_write_cstring(os, kCRLF);



	for (int32_t i = 0; i < payload->rtcp_fb.vsize; i++) {
		yang_memset(intstr, 0, sizeof(intstr));
		yang_itoa(payload->payload_type, intstr, 10);
		//yang_write_cstring(os, intstr);
		yang_write_cstring(os, "a=rtcp-fb:");
		yang_write_cstring(os, intstr);
		yang_write_cstring(os, " ");
		yang_write_cstring(os, payload->rtcp_fb.payload[i]);
		yang_write_cstring(os, kCRLF);
	}
	if (yang_strlen(payload->format_specific_param)) {
		yang_write_cstring(os, "a=fmtp:");
		yang_memset(intstr, 0, sizeof(intstr));
		yang_itoa(payload->payload_type, intstr, 10);
		yang_write_cstring(os, intstr);
		yang_write_cstring(os, " ");
		yang_write_cstring(os, payload->format_specific_param);
		yang_write_cstring(os, kCRLF);


	}

	return err;
}

