//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGSOCKETUTIL_H_
#define YANGSOCKETUTIL_H_
#include <stdint.h>
#include <yangutil/sys/YangVector.h>
#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_getLocalInfo(char* p);
int32_t yang_getLocalInfoList(YangStringVector* p);
void yang_getIp( char* domain, char* ip);
#ifdef __cplusplus
}
#endif
#endif /* YANGSOCKETUTIL_H_ */
