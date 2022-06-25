//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGWEBRTC_YANGRTCRTP_H_
#define YANGWEBRTC_YANGRTCRTP_H_



#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangMath.h>
#include <stdint.h>
#ifndef __cplusplus
#define bool int32_t
#endif

typedef enum YangRtspPacketPayloadType {
	YangRtspPacketPayloadTypeRaw,
	YangRtspPacketPayloadTypeFUA2,
	YangRtspPacketPayloadTypeFUA,
	YangRtspPacketPayloadTypeNALU,
	YangRtspPacketPayloadTypeSTAP,
	YangRtspPacketPayloadTypeUnknown,
} YangRtspPacketPayloadType;

typedef enum YangRtpExtensionType {
	kRtpExtensionNone,
	kRtpExtensionTransportSequenceNumber,
	kRtpExtensionAudioLevel,
	kRtpExtensionNumberOfExtensions
} YangRtpExtensionType;

// Fast parse the SSRC from RTP packet. Return 0 if invalid.
uint32_t yang_rtp_fast_parse_ssrc(char *buf, int32_t size);
uint8_t yang_rtp_fast_parse_pt(char *buf, int32_t size);
int32_t yang_rtp_fast_parse_twcc(char *buf, int32_t size, uint8_t twcc_id,
		uint16_t *twcc_sn);

bool yang_seq_is_newer(uint16_t value, uint16_t pre_value);
bool yang_seq_is_rollback(uint16_t value, uint16_t pre_value);
int32_t yang_seq_distance(uint16_t value, uint16_t pre_value);

typedef struct YangRtpExtensionTypes {
	uint8_t ids[kRtpExtensionNumberOfExtensions];
} YangRtpExtensionTypes;
typedef struct YangRtpExtensionTwcc {
	bool has_twcc;
	uint8_t id;
	uint16_t sn;
} YangRtpExtensionTwcc;

typedef struct YangRtpExtensionOneByte {
	bool has_ext;
	int32_t id;
	uint8_t value;
} YangRtpExtensionOneByte;

struct YangRtpExtension {
	bool has_ext;
	bool decode_twcc_extension;
};

typedef struct YangRtpExtensions {
	bool has_ext;

	bool decode_twcc_extension;
	YangRtpExtensionTwcc twcc;
	YangRtpExtensionOneByte audio_level;
    YangRtpExtensionTypes *types;

} YangRtpExtensions;
#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_encode_rtpExtensions(YangRtpExtensions *ext, YangBuffer *buf);
int32_t yang_encode_rtpExtensionsTwcc(YangRtpExtensionTwcc *twcc,YangBuffer *buf);
int32_t yang_encode_rtpExtensionsOneByte(YangRtpExtensionOneByte *one,YangBuffer *buf);

int32_t yang_decode_rtpExtensions(YangRtpExtensions *ext, YangBuffer *buf);
int32_t yang_decode_rtpExtensionsTwcc(YangRtpExtensionTwcc *twcc,YangBuffer *buf);
int32_t yang_decode_rtpExtensionsOneByte(YangRtpExtensionOneByte *one,YangBuffer *buf);
uint64_t yang_rtpExtensionTwcc_nb(YangRtpExtensionTwcc *twcc);
uint64_t yang_rtpExtensionOneByte_nb(YangRtpExtensionOneByte *onebyte);
#ifdef __cplusplus
}
#endif


#endif /* YANGWEBRTC_YANGRTCRTP_H_ */
