//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_
#define SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_
#include <yangutil/yangtype.h>
#ifdef __cplusplus
extern "C"{
#endif
 int32_t yang_http_post(yangbool isWhip,YangIpFamilyType familyType,char* rets,char* ip,int32_t port,char* api,uint8_t *p, int32_t plen);
#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGUSERDATA_INCLUDE_YangHttpSocket_H_ */
