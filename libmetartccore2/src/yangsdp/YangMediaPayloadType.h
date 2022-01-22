#ifndef SRC_YANGSDP_YANGMEDIAPAYLOADTYPE_H_
#define SRC_YANGSDP_YANGMEDIAPAYLOADTYPE_H_
#include <stdint.h>


#include "YangSdpType.h"
typedef struct{
    int32_t payload_type;

    char encoding_name[16];
    int32_t clock_rate;
    char encoding_param[256];
   char format_specific_param[128];

   YangStrVector rtcp_fb;

}YangMediaPayloadType;
yang_declare_struct(YangMediaPayloadType)
yang_declare_struct_init(YangMediaPayloadType)
yang_declare_struct_destroy(YangMediaPayloadType)
yang_declare_struct_insert(YangMediaPayloadType)
int32_t yang_encode_mediapayloadtype(YangMediaPayloadType* payload,YangBuffer* os);

#endif /* SRC_YANGSDP_YANGMEDIAPAYLOADTYPE_H_ */
