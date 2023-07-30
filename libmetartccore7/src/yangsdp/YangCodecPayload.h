//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGSDP_YANGCODECPAYLOAD_H_
#define SRC_YANGSDP_YANGCODECPAYLOAD_H_

#include <yangsdp/YangMediaPayloadType.h>

typedef struct{
	int32_t capacity;
	int32_t vlen;
	char** groups;
}YangFbsVector;

typedef struct{
	   char type[16];
	    uint8_t pt;
	    uint8_t pt_of_publisher;
	    char name[32];
	    int32_t sample;

	    YangFbsVector rtcp_fbs;
}YangCodecPayload;

#endif /* SRC_YANGSDP_YANGCODECPAYLOAD_H_ */
