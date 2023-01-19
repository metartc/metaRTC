//
// Copyright (c) 2019-2022 yanggaofeng
//


#ifndef SRC_YANGAVUTIL_YANGSPSPPS_H_
#define SRC_YANGAVUTIL_YANGSPSPPS_H_
#include <yangutil/yangtype.h>

#ifdef __cplusplus
extern "C"{
#endif
void yang_h264_decode_getH264RtmpHeader(uint8_t* ps,uint8_t** sps,int32_t* spsLen);
int yang_h264_decode_spspps(uint8_t * buf,uint32_t nLen,int32_t *p_width,int32_t *p_height,int32_t *p_fps);
#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGAVUTIL_YANGSPSPPS_H_ */
