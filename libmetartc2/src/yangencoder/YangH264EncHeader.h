/*
 * YangH264EncHeader.h
 *
 *  Created on: 2019年10月18日
 *      Author: yang
 */

#ifndef YANGENCODER_SRC_YANGH264ENCHEADER_H_
#define YANGENCODER_SRC_YANGH264ENCHEADER_H_
#include "stdint.h"

#if YangLibva
#ifndef _WIN32
#include "va/va.h"
#endif
#include "yangutil/yangavtype.h"

struct YangMeataData {
	int32_t width;
	int32_t height;
	int32_t width_align;
	int32_t height_align;
	int32_t num_ref_frames;
	int32_t numShortTerm;
	int32_t constraint_set_flag;
	int32_t h264_packedheader; /* support pack header? */
	int32_t h264_maxref;
	int32_t h264_entropy_mode; /* cabac */
	int32_t MaxFrameNum;
	int32_t MaxPicOrderCntLsb;
	int32_t Log2MaxFrameNum;
	int32_t Log2MaxPicOrderCntLsb;
	int32_t gop_size;
	int32_t frame_coded;
	int32_t frame_bitrate;
	int32_t frame_slices;
	double frame_size;
	int32_t initial_qp;
	int32_t minimal_qp;
	int32_t intra_period;
	int32_t intra_idr_period;
	int32_t ip_period;
	int32_t rc_mode;
};
void initYangMeta(YangMeataData *pmd, YangVideoParam *ini);
#ifndef _WIN32
void initVaSeq(YangMeataData *pmd, VAEncSequenceParameterBufferH264 *sps);
void initVaPic(YangMeataData *pmd, VAEncPictureParameterBufferH264 *pps);
void initVaPara(YangMeataData *pmd, VAEncSequenceParameterBufferH264 *sps,
		VAEncPictureParameterBufferH264 *pps);
void printSeq(VAEncSequenceParameterBufferH264 *sps);
void printPic(VAEncPictureParameterBufferH264 *pps);
#endif
void setZbVmd(uint8_t *sps, int32_t spsLen, uint8_t *pps, int32_t ppsLen,
		uint8_t *zbconf, int32_t *zbLen);
void setZbVmd_f(uint8_t *buf,int32_t bufLen,uint8_t *configBuf,int32_t *zbLen);
/**class YangH264EncHeader {
 public:
 YangH264EncHeader();
 virtual ~YangH264EncHeader();
 int32_t setSps(uint8_t *sps_buffer,int32_t *spsLen);
 int32_t setPPS(uint8_t *pps_buffer,int32_t *ppsLen);
 };
 **/
#endif /* YANGENCODER_SRC_YANGH264ENCHEADER_H_ */
#endif
