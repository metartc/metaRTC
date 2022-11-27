//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_

#include <yangutil/yangtype.h>
#include <yangutil/yangavtype.h>
#include <stdint.h>

#define Yang_Nalu_SpsppsBufferLen 256

 typedef struct {
	int spsppsPos;
	int keyframePos;
}YangH264NaluData;

 typedef struct {
	int spsPos;
	int ppsPos;
	int spsLen;
	int ppsLen;
	int keyframePos;
}YangH264NaluData2;

int32_t yang_parseH264Nalu(YangFrame* videoFrame, YangH264NaluData* pnalu);
int32_t yang_parseH264Nalu2(YangFrame* videoFrame, YangH264NaluData2* pnalu);
int32_t yang_getH264SpsppseNalu(YangFrame* videoFrame,uint8_t* pnaludata);
int32_t yang_getH264KeyframeNalu(YangFrame* videoFrame);
yangbool yang_hasH264Pframe(uint8_t* p);

int32_t yang_getH265SpsppseNalu(YangFrame* videoFrame,uint8_t* pnaludata);
int32_t yang_parseH265Nalu(YangFrame* videoFrame,YangH264NaluData* pnalu);

int32_t yang_getNalupos(uint8_t* data,int plen);

#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGCNALU_H_ */
