//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGWEBRTC_YANGSSRCINFO_H_
#define SRC_YANGWEBRTC_YANGSSRCINFO_H_
#include <yangutil/yangtype.h>
#include <yangsdp/YangSdpType.h>

yang_vector_declare(yangsdpint)


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


yang_vector_declare(YangSSRCGroup)


yang_vector_declare(YangSSRCInfo)


int32_t yang_encode_ssrcinfo(YangSSRCInfo* info,YangBuffer* os);
int32_t yang_encode_ssrcgroup(YangSSRCGroup* group,YangBuffer* os);

#endif /* SRC_YANGWEBRTC_YANGSSRCINFO_H_ */
