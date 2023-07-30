//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGSDP_YANGMEDIAPAYLOADTYPE_H_
#define SRC_YANGSDP_YANGMEDIAPAYLOADTYPE_H_

#include <yangutil/yangtype.h>
#include "YangSdpType.h"

typedef struct{
    int32_t payload_type;

    char encoding_name[16];
    int32_t clock_rate;
    char encoding_param[256];
   char format_specific_param[128];

   YangStringVector rtcp_fb;

}YangMediaPayloadType;
yang_vector_declare(YangMediaPayloadType)

int32_t yang_encode_mediapayloadtype(YangMediaPayloadType* payload,YangBuffer* os);

#endif /* SRC_YANGSDP_YANGMEDIAPAYLOADTYPE_H_ */
