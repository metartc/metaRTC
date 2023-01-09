//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGSDP_YANGRTXPAYLOADDES_H_
#define SRC_YANGSDP_YANGRTXPAYLOADDES_H_

#include <yangsdp/YangCodecPayload.h>
typedef struct{

	uint8_t pt;
	// for publish, equals to PT of itself;
	// for subscribe, is the PT of publisher;
	uint8_t pt_of_publisher;
	uint8_t apt_;
	int32_t sample;
	char name[32];
	char type[16];
	YangFbsVector rtcp_fbs;
}YangRtxPayloadDes;
void yang_create_rtxPayloaddes(YangRtxPayloadDes* des,uint8_t pt, char* encode_name, int32_t sample);

#endif /* SRC_YANGSDP_YANGRTXPAYLOADDES_H_ */
