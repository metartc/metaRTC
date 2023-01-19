//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGFILE_H_
#define INCLUDE_YANGUTIL_SYS_YANGFILE_H_
#include <yangutil/yangtype.h>
#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_getCurpath(char* path);
int32_t yang_getLibpath(char* path);
int32_t yang_getCaFile(char* pem,char* key);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGFILE_H_ */
