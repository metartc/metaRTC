#ifndef INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_
#define INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_
#include <stdint.h>
#include <string>
#include <yangutil/yangavtype.h>
struct YangH264NaluData{
	int spsppsPos;
	int keyframePos;
};
struct YangH264NaluData2{
	int spsPos;
	int ppsPos;
	int spsLen;
	int ppsLen;
	int keyframePos;
};
int32_t yang_parseH264Nalu(YangFrame* videoFrame,YangH264NaluData* pnalu);
int32_t yang_parseH264Nalu2(YangFrame* videoFrame,YangH264NaluData2* pnalu);
int32_t yang_getH264SpsppseNalu(YangFrame* videoFrame,uint8_t* pnaludata);
int32_t yang_getH264KeyframeNalu(YangFrame* videoFrame);
bool yang_hasH264Pframe(uint8_t* p);

int32_t yang_getH265SpsppseNalu(YangFrame* videoFrame,uint8_t* pnaludata);
int32_t yang_parseH265Nalu(YangFrame* videoFrame,YangH264NaluData* pnalu);

/**
 * Table 7-1 - NAL unit type codes, syntax element categories, and NAL unit type classes
 * ISO_IEC_14496-10-AVC-2012.pdf, page 83.
 */

std::string yang_avc_nalu2str(YangAvcNaluType nalu_type);

#endif /* INCLUDE_YANGAVUTIL_VIDEO_YANGNALU_H_ */
