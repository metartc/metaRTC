//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangencoder/YangH264EncHeader.h>
#include "malloc.h"
#include "yangutil/yang_unistd.h"
#include <string>
//#include <arpa/inet.h>

#if YangLibva
#include "string.h"
#ifdef _WIN32
#include <winsock2.h>
#else
#include "va/va.h"
#include <arpa/inet.h>
#endif
void initYangMeta(YangMeataData *pmd,YangVideoParam *pcontext){
	pmd->width=pcontext->outWidth ;
	pmd->height=pcontext->outHeight;
	pmd->width_align=(pcontext->outWidth + 15) & (~15);
	pmd->height_align=(pcontext->outHeight + 15) & (~15);
	pmd->gop_size=20;
	pmd->num_ref_frames=2;
	pmd->numShortTerm=0;
	pmd->constraint_set_flag=0;
	pmd->h264_packedheader=0;/*supportpackheader?*/
	pmd->h264_maxref=(1<<16|1);
	pmd->h264_entropy_mode=0;//1;/*cabac*/
	pmd->MaxFrameNum=(2<<16);
	pmd->MaxPicOrderCntLsb=(2<<8);
	pmd->Log2MaxFrameNum=16;
	pmd->Log2MaxPicOrderCntLsb=4;//8;

	pmd->frame_coded=0;
	pmd->frame_bitrate=1024000;
	pmd->frame_slices=1;
	pmd->frame_size=0;
	pmd->initial_qp=26;
	pmd->minimal_qp=26;
	pmd->intra_period=pmd->gop_size;
	pmd->intra_idr_period=pmd->gop_size;
	pmd->ip_period=1;//3;
	pmd->rc_mode=0;
}
#ifndef _WIN32
void initVaSeq(YangMeataData *pmd,VAEncSequenceParameterBufferH264 *sps){
		sps->seq_parameter_set_id=0;
		sps->level_idc = 31;//41 /*SH_LEVEL_3*/;
		sps->intra_period = pmd->intra_period;
		sps->intra_idr_period = pmd->intra_idr_period;
		sps->ip_period = pmd->ip_period;

		sps->bits_per_second = pmd->frame_bitrate;
		sps->max_num_ref_frames = pmd->num_ref_frames;
		sps->picture_width_in_mbs = pmd->width_align / 16;
		sps->picture_height_in_mbs = pmd->height_align / 16;

		sps->seq_fields.bits.chroma_format_idc = 1;
		sps->seq_fields.bits.frame_mbs_only_flag = 1;
		sps->seq_fields.bits.mb_adaptive_frame_field_flag=0;
		sps->seq_fields.bits.seq_scaling_matrix_present_flag=0;
		sps->seq_fields.bits.direct_8x8_inference_flag = 1;
		sps->seq_fields.bits.log2_max_frame_num_minus4 = 4;//pmd->Log2MaxFrameNum - 4;
		sps->seq_fields.bits.pic_order_cnt_type=0;
		sps->seq_fields.bits.log2_max_pic_order_cnt_lsb_minus4 =pmd->Log2MaxPicOrderCntLsb - 4;
		sps->seq_fields.bits.delta_pic_order_always_zero_flag=0;

		sps->time_scale = 50;
			sps->num_units_in_tick = 1; /* Tc = num_units_in_tick / time_sacle */
		sps->bit_depth_chroma_minus8=0;
		sps->bit_depth_luma_minus8=0;
		if (pmd->width != pmd->width_align || pmd->height != pmd->height_align) {
			sps->frame_cropping_flag = 1;
			sps->frame_crop_left_offset = 0;
			sps->frame_crop_right_offset = (pmd->width_align - pmd->width)
					/ 2;
			sps->frame_crop_top_offset = 0;
			sps->frame_crop_bottom_offset =(pmd->height_align - pmd->height) / 2;
		}
	//	sps->vui_parameters_present_flag=1;
	//	sps->vui_fields.bits.bitstream_restriction_flag=1;
	//	sps->vui_fields.bits.log2_max_mv_length_horizontal=15,sps->vui_fields.bits.log2_max_mv_length_vertical=15;
	//	sps->vui_fields.bits.timing_info_present_flag=1;
}
void initVaPic(YangMeataData *pmd,VAEncPictureParameterBufferH264 *pps){
	pps->pic_fields.bits.entropy_coding_mode_flag = pmd->h264_entropy_mode;
	pps->pic_fields.bits.deblocking_filter_control_present_flag = 1;
	pps->pic_init_qp = pmd->initial_qp;
}
void initVaPara(YangMeataData *pmd,VAEncSequenceParameterBufferH264 *sps,VAEncPictureParameterBufferH264 *pps){
	initVaSeq(pmd,sps);
	initVaPic(pmd,pps);
}
#endif


void setZbVmd(uint8_t *sps,int32_t spsLen,uint8_t *pps,int32_t pps_len,uint8_t *configBuf,int32_t *zbLen){
		configBuf[0] = 0x17;
		configBuf[1] = 0x00;
		configBuf[2] = 0x00;
		configBuf[3] = 0x00;
		configBuf[4] = 0x00;
		configBuf[5] = 0x01;
		configBuf[6] = sps[1];
		configBuf[7] = sps[2];
		configBuf[8] = sps[3];		//0x29;  //AVCLevelIndication1f
		configBuf[9] = 0xff;		//03;//ff;//0x03; AVCLevelIndication
		configBuf[10] = 0xe1;		//01;//e1;//01;numOfSequenceParameterSets
		char * szTmp = (char*) configBuf + 11;

		short slen = spsLen;		//spslen（short）
		slen = htons(slen);
		memcpy(szTmp, &slen, sizeof(short));
		szTmp += sizeof(short);

		memcpy(szTmp, sps,spsLen);
		szTmp += spsLen;
		*szTmp = 0x01;
		szTmp += 1;
		slen = pps_len;		//spslen（short）
		slen = htons(slen);

		memcpy(szTmp, &slen, sizeof(short));
		szTmp += sizeof(short);
		memcpy(szTmp, pps, pps_len);

		szTmp += pps_len;
		*zbLen = szTmp - (char*) configBuf;
		szTmp = NULL;
}
void yang_find_next_start_code(uint8_t *buf,int32_t bufLen,int32_t *spsPos,int32_t *spsLen,int32_t *ppsPos,int32_t *ppsLen)
{
   int32_t i = 0;

   *spsPos=0;*ppsPos=0;
   while (i <bufLen-3) {
       if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
       	*spsPos=i+4;
       	i+=4;
       	 break;
       }

       i++;
   }
   while (i <bufLen-3) {
       if (buf[i] == 0 && buf[i + 1] == 0 &&buf[i + 2] == 0&& buf[i + 3] == 1){
       	*spsLen=i-*spsPos;
       	*ppsPos=i+4;
       	*ppsLen=bufLen-*ppsPos;
       	 break;
       }

       i++;
   }

}
void setZbVmd_f(uint8_t *buf,int32_t bufLen,uint8_t *configBuf,int32_t *zbLen){
	int32_t spsPos=0,ppsPos=0;
	int32_t spsLen=0,ppsLen=0;
	yang_find_next_start_code(buf,bufLen,&spsPos,&spsLen,&ppsPos,&ppsLen);
	setZbVmd(buf+spsPos, spsLen,buf+ppsPos,ppsLen,configBuf,zbLen);


}
#endif

