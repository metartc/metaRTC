#ifndef SRC_YANGWEBRTC_YANGSSRCINFO_H_
#define SRC_YANGWEBRTC_YANGSSRCINFO_H_
#include <stdint.h>
#include <yangsdp/YangSdpType.h>

yang_declare_struct(yangsdpint)
yang_declare_struct_init(yangsdpint)
yang_declare_struct_insert(yangsdpint)
yang_declare_struct_destroy(yangsdpint)

typedef struct{
	yangsdpintVector groups;
	char semantic[64];
}YangSSRCGroup;
typedef struct{
    uint32_t ssrc;
    char cname[64];
    char msid[64];
    char msid_tracker[64];
    char mslabel[64];
    char label[64];
}YangSSRCInfo;


yang_declare_struct(YangSSRCGroup)
yang_declare_struct_init(YangSSRCGroup)
yang_declare_struct_insert(YangSSRCGroup)
yang_declare_struct_destroy(YangSSRCGroup)

yang_declare_struct(YangSSRCInfo)
yang_declare_struct_insert(YangSSRCInfo)


int32_t yang_encode_ssrcinfo(YangSSRCInfo* info,YangBuffer* os);
int32_t yang_encode_ssrcgroup(YangSSRCGroup* group,YangBuffer* os);

void yang_init_SSRCGroup(YangSSRCGroup* group);
void yang_destroy_SSRCGroup(YangSSRCGroup* group);

#endif /* SRC_YANGWEBRTC_YANGSSRCINFO_H_ */
