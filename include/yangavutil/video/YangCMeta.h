//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGCMETA_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGCMETA_H_

#include <stdint.h>

#include <yangutil/yangavtype.h>
void yang_find_start_code(YangVideoCodec pve,uint8_t *buf,int32_t bufLen,int32_t *vpsPos,int32_t *vpsLen,int32_t *spsPos,int32_t *spsLen,int32_t *ppsPos,int32_t *ppsLen);
int32_t yang_find_pre_start_code(uint8_t *buf,int32_t bufLen);
void yang_createH264Meta( YangVideoMeta* pvmd, YangFrame *videoFrame);
void yang_createH265Meta( YangVideoMeta* pvmd, YangFrame *videoFrame);
void yang_getConfig_Flv_H264( YangH2645Conf *p_264, uint8_t *configBuf,int32_t *p_configLen);
void yang_getConfig_Meta_H264( YangSample* sps,  YangSample* pps,uint8_t *configBuf,int32_t *p_configLen);
void yang_getConfig_Flv_H265( YangH2645Conf *p_264, uint8_t *configBuf,int32_t *p_configLen);
void yang_getConfig_Meta_H265( YangSample* vps, YangSample* sps,  YangSample* pps, uint8_t *configBuf,int32_t *p_configLen);

void yang_getH265RtmpHeader(uint8_t *meta, uint8_t *src, int32_t *hLen);
void yang_getH264RtmpHeader(uint8_t *buf, uint8_t *src, int32_t *hLen);

void yang_decodeMetaH264(uint8_t *configBuf,int32_t p_configLen, YangSample* sps, YangSample* pps);
void yang_decodeMetaH265(uint8_t *configBuf,int32_t p_configLen, YangSample* vps, YangSample* sps, YangSample* pps);




#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGCMETA_H_ */
