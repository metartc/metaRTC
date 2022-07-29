//
// Copyright (c) 2019-2022 JerryQiu
//
#ifndef SRC_YANGP2P_RK_YANGRKCOMMON_H_
#define SRC_YANGP2P_RK_YANGRKCOMMON_H_

#include <unistd.h>
#include "rkmedia_api.h"
#include "rkmedia_venc.h"
#include "rkmedia_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

int video_init(char *resolution);
int video_stop(void);
int video_get_vencChan(void);
CODEC_TYPE_E video_get_enc_type(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* SRC_YANGP2P_RK_YANGRKCOMMON_H_ */
