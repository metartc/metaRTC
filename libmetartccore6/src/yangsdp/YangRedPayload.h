//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGSDP_YANGREDPAYLOAD_H_
#define SRC_YANGSDP_YANGREDPAYLOAD_H_

#include <yangsdp/YangCodecPayload.h>
typedef struct{

	uint8_t pt;
	// for publish, equals to PT of itself;
	// for subscribe, is the PT of publisher;
	uint8_t pt_of_publisher;
	int32_t channel;
	int32_t sample;
	char name[32];
	char type[16];

	YangFbsVector rtcp_fbs;
}YangRedPayload;
void yang_create_redPayloaddes(YangRedPayload* red,uint8_t pt, char* encode_name, int32_t sample,int32_t channel);

#endif /* SRC_YANGSDP_YANGREDPAYLOAD_H_ */
