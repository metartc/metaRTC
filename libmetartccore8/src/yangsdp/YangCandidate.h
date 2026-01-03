//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef SRC_YANGSDP_YANGCANDIDATE_H_
#define SRC_YANGSDP_YANGCANDIDATE_H_
#include <yangutil/yangtype.h>
#include <yangutil/yangavinfo.h>
#include <yangutil/buffer/YangCBuffer.h>
#include <yangutil/sys/YangSocket.h>
typedef struct
{
	YangIceCandidateType candidateType;
	uint32_t priority;
	YangSocketProtocol socketProtocol;
	YangIpAddress address;

}YangCandidate;


int32_t yang_candidate_parse(YangCandidate* candidate,char* candidateStr,YangIpFamilyType familyType);
int32_t yang_candidate_toBuffer(YangCandidate* candidate,int32_t foundation,YangBuffer *os);
int32_t yang_candidate_toString(YangCandidate* candidate,int32_t foundation,char **candidateStr);
int32_t yang_candidate_toJson(YangCandidate* candidate,int32_t foundation,char* ufrag,char* stunStr,char **candidateStr);
#endif /* SRC_YANGSDP_YANGCANDIDATE_H_ */
