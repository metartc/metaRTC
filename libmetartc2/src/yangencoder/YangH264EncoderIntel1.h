#include "YangH264EncoderIntel.h"
#ifndef _WIN32
#include "memory.h"
#include <iostream>
#include "string.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "memory.h"
#include "yangutil/yang_unistd.h"
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include "string.h"
#include "stdio.h"
#if YangLibva
#include "xf86drm.h"

//#include "yangutil/video/yangconvert.h"
using namespace std;
#define CHECK_VASTATUS(X, MSG)                {if ((X) != VA_STATUS_SUCCESS) {cout <<"\n_ERROR:"<<X<<":"<< #MSG << endl; }}
#define NAL_REF_IDC_NONE        0
#define NAL_REF_IDC_LOW         1
#define NAL_REF_IDC_MEDIUM      2
#define NAL_REF_IDC_HIGH        3
#define NAL_NON_IDR             1
#define NAL_IDR                 5
#define NAL_SPS                 7
#define NAL_PPS                 8
#define NAL_SEI			        6
#define SLICE_TYPE_P            0
#define SLICE_TYPE_B            1
#define SLICE_TYPE_I            2
#define IS_P_SLICE(type) (SLICE_TYPE_P == (type))
#define IS_B_SLICE(type) (SLICE_TYPE_B == (type))
#define IS_I_SLICE(type) (SLICE_TYPE_I == (type))

#define ENTROPY_MODE_CAVLC      0
#define ENTROPY_MODE_CABAC      1

#define PROFILE_IDC_BASELINE    66
#define PROFILE_IDC_MAIN        77
#define PROFILE_IDC_HIGH        100

#define BITSTREAM_ALLOCATE_STEPPING     4096
#define FRAME_P 0
#define FRAME_B 1
#define FRAME_I 2
#define FRAME_IDR 7

int32_t rc_default_modes[] = {
VA_RC_VBR,
VA_RC_CQP,
VA_RC_VBR_CONSTRAINED,
VA_RC_CBR,
VA_RC_VCM,
VA_RC_NONE, };
void encoding2display_order(unsigned int64_t encoding_order, int32_t intra_period,
		int32_t intra_idr_period, int32_t ip_period,
		unsigned int64_t *displaying_order, int32_t *frame_type);

uint32_t  va_swap32(uint32_t  val) {
	uint8_t *pval = (uint8_t *) &val;

	return ((pval[0] << 24) | (pval[1] << 16) | (pval[2] << 8) | (pval[3] << 0));
}

void bitstream_start(bitstream *bs) {
	bs->max_size_in_dword = BITSTREAM_ALLOCATE_STEPPING;
	bs->buffer = (unsigned int*) calloc(bs->max_size_in_dword * sizeof(int), 1);
	bs->bit_offset = 0;
}

void bitstream_end(bitstream *bs) {
	int32_t pos = (bs->bit_offset >> 5);
	int32_t bit_offset = (bs->bit_offset & 0x1f);
	int32_t bit_left = 32 - bit_offset;

	if (bit_offset) {
		bs->buffer[pos] = va_swap32((bs->buffer[pos] << bit_left));
	}
}

void bitstream_put_ui(bitstream *bs, uint32_t  val, int32_t size_in_bits) {
	int32_t pos = (bs->bit_offset >> 5);
	int32_t bit_offset = (bs->bit_offset & 0x1f);
	int32_t bit_left = 32 - bit_offset;

	if (!size_in_bits)
		return;

	bs->bit_offset += size_in_bits;

	if (bit_left > size_in_bits) {
		bs->buffer[pos] = (bs->buffer[pos] << size_in_bits | val);
	} else {
		size_in_bits -= bit_left;
		bs->buffer[pos] = (bs->buffer[pos] << bit_left) | (val >> size_in_bits);
		bs->buffer[pos] = va_swap32(bs->buffer[pos]);

		if (pos + 1 == bs->max_size_in_dword) {
			bs->max_size_in_dword += BITSTREAM_ALLOCATE_STEPPING;
			bs->buffer = (unsigned int*) realloc(bs->buffer,
					bs->max_size_in_dword * sizeof(unsigned int));
		}

		bs->buffer[pos + 1] = val;
	}
}

void bitstream_put_ue(bitstream *bs, uint32_t  val) {
	int32_t size_in_bits = 0;
	int32_t tmp_val = ++val;

	while (tmp_val) {
		tmp_val >>= 1;
		size_in_bits++;
	}

	bitstream_put_ui(bs, 0, size_in_bits - 1); // leading zero
	bitstream_put_ui(bs, val, size_in_bits);
}

void bitstream_put_se(bitstream *bs, int32_t val) {
	uint32_t  new_val;

	if (val <= 0)
		new_val = -2 * val;
	else
		new_val = 2 * val - 1;

	bitstream_put_ue(bs, new_val);
}

void bitstream_byte_aligning(bitstream *bs, int32_t bit) {
	int32_t bit_offset = (bs->bit_offset & 0x7);
	int32_t bit_left = 8 - bit_offset;
	int32_t new_val;

	if (!bit_offset)
		return;

	//assert(bit == 0 || bit == 1);

	if (bit)
		new_val = (1 << bit_left) - 1;
	else
		new_val = 0;

	bitstream_put_ui(bs, new_val, bit_left);
}

void rbsp_trailing_bits(bitstream *bs) {
	bitstream_put_ui(bs, 1, 1);
	bitstream_byte_aligning(bs, 0);
}

void nal_start_code_prefix(bitstream *bs) {
	bitstream_put_ui(bs, 0x00000001, 32);
}

void nal_header(bitstream *bs, int32_t nal_ref_idc, int32_t nal_unit_type) {
	bitstream_put_ui(bs, 0, 1); /* forbidden_zero_bit: 0 */
	bitstream_put_ui(bs, nal_ref_idc, 2);
	bitstream_put_ui(bs, nal_unit_type, 5);
}

int32_t build_packed_sei_buffer_timing(uint32_t  init_cpb_removal_length,
		uint32_t  init_cpb_removal_delay,
		uint32_t  init_cpb_removal_delay_offset,
		uint32_t  cpb_removal_length, uint32_t  cpb_removal_delay,
		uint32_t  dpb_output_length, uint32_t  dpb_output_delay,
		uint8_t **sei_buffer) {
	uint8_t *byte_buf;
	int32_t bp_byte_size, i, pic_byte_size;

	bitstream nal_bs;
	bitstream sei_bp_bs, sei_pic_bs;

	bitstream_start(&sei_bp_bs);
	bitstream_put_ue(&sei_bp_bs, 0); /*seq_parameter_set_id*/
	bitstream_put_ui(&sei_bp_bs, init_cpb_removal_delay, cpb_removal_length);
	bitstream_put_ui(&sei_bp_bs, init_cpb_removal_delay_offset,
			cpb_removal_length);
	if (sei_bp_bs.bit_offset & 0x7) {
		bitstream_put_ui(&sei_bp_bs, 1, 1);
	}
	bitstream_end(&sei_bp_bs);
	bp_byte_size = (sei_bp_bs.bit_offset + 7) / 8;

	bitstream_start(&sei_pic_bs);
	bitstream_put_ui(&sei_pic_bs, cpb_removal_delay, cpb_removal_length);
	bitstream_put_ui(&sei_pic_bs, dpb_output_delay, dpb_output_length);
	if (sei_pic_bs.bit_offset & 0x7) {
		bitstream_put_ui(&sei_pic_bs, 1, 1);
	}
	bitstream_end(&sei_pic_bs);
	pic_byte_size = (sei_pic_bs.bit_offset + 7) / 8;

	bitstream_start(&nal_bs);
	nal_start_code_prefix(&nal_bs);
	nal_header(&nal_bs, NAL_REF_IDC_NONE, NAL_SEI);

	/* Write the SEI buffer period data */
	bitstream_put_ui(&nal_bs, 0, 8);
	bitstream_put_ui(&nal_bs, bp_byte_size, 8);

	byte_buf = (uint8_t *) sei_bp_bs.buffer;
	for (i = 0; i < bp_byte_size; i++) {
		bitstream_put_ui(&nal_bs, byte_buf[i], 8);
	}
	free(byte_buf);
	/* write the SEI timing data */
	bitstream_put_ui(&nal_bs, 0x01, 8);
	bitstream_put_ui(&nal_bs, pic_byte_size, 8);

	byte_buf = (uint8_t *) sei_pic_bs.buffer;
	for (i = 0; i < pic_byte_size; i++) {
		bitstream_put_ui(&nal_bs, byte_buf[i], 8);
	}
	free(byte_buf);

	rbsp_trailing_bits(&nal_bs);
	bitstream_end(&nal_bs);

	*sei_buffer = (uint8_t *) nal_bs.buffer;

	return nal_bs.bit_offset;
}


char *rc_to_string(int32_t rcmode) {
	switch (rcmode) {
	case VA_RC_NONE:
		return "NONE";
	case VA_RC_CBR:
		return "CBR";
	case VA_RC_VBR:
		return "VBR";
	case VA_RC_VCM:
		return "VCM";
	case VA_RC_CQP:
		return "CQP";
	case VA_RC_VBR_CONSTRAINED:
		return "VBR_CONSTRAINED";
	default:
		return "Unknown";
	}
}
/**
 void YangH264EncoderIntel::initSlicePara(VASliceParameterBufferH264 *slice264,
 uint8_t *p_data, int32_t p_len) {
 //initNaluPara(&m_Nalu,p_data);


 YangSliceContext *sl = h264header.sl;
 int32_t ret = (h264header.m_sps.mb_aff
 && (sl->picture_structure == Yang_PICT_FRAME))
 || sl->picture_structure != Yang_PICT_FRAME;
 slice264->slice_data_size = p_len;
 slice264->slice_data_offset = 0;
 slice264->slice_data_flag = VA_SLICE_DATA_FLAG_ALL;
 slice264->slice_data_bit_offset = h264header.get_bits_count(&sl->gb);
 slice264->first_mb_in_slice = (sl->mb_y >> ret) * h264header.m_sps.mb_width
 + sl->mb_x;
 slice264->slice_type = yang_get_slice_type(sl); //sl->slice_type;//m_Nalu.nal_reference_idc;//ff_h264_get_slice_type(sl);
 slice264->direct_spatial_mv_pred_flag =
 sl->slice_type == Yang_PICTURE_TYPE_B ?
 sl->direct_spatial_mv_pred : 0;
 slice264->num_ref_idx_l0_active_minus1 =
 sl->list_count > 0 ? sl->ref_count[0] - 1 : 0;
 slice264->num_ref_idx_l1_active_minus1 =
 sl->list_count > 1 ? sl->ref_count[1] - 1 : 0;
 slice264->cabac_init_idc = sl->cabac_init_idc;
 slice264->slice_qp_delta = sl->qscale - h264header.m_pps.init_qp;
 slice264->disable_deblocking_filter_idc =
 sl->deblocking_filter < 2 ?
 !sl->deblocking_filter : sl->deblocking_filter;
 slice264->slice_alpha_c0_offset_div2 = sl->slice_alpha_c0_offset / 2;
 slice264->slice_beta_offset_div2 = sl->slice_beta_offset / 2;
 slice264->luma_log2_weight_denom = sl->pwt.luma_log2_weight_denom;
 slice264->chroma_log2_weight_denom = sl->pwt.chroma_log2_weight_denom;
 sl = NULL;
 if(slice264->slice_type==2)
 {
 slice264->chroma_weight_l0[0][0]=0;
 slice264->chroma_weight_l0[0][1]=0;
 slice264->chroma_weight_l1[0][0]=0;
 slice264->chroma_weight_l1[0][1]=0;
 slice264->RefPicList0[0].picture_id = 0xffffffff;
 }else{
 slice264->chroma_weight_l0[0][0]=1;
 slice264->chroma_weight_l0[0][1]=1;
 slice264->chroma_weight_l1[0][0]=1;
 slice264->chroma_weight_l1[0][1]=1;
 }

 for (int32_t i = 0; i < 32; i++) {
 slice264->RefPicList0[i].flags = VA_PICTURE_H264_INVALID;
 slice264->RefPicList1[i].flags = VA_PICTURE_H264_INVALID;
 slice264->RefPicList0[i].picture_id = 0xffffffff;
 slice264->RefPicList1[i].picture_id = 0xffffffff;
 }

 }**/
/**
 void YangH264EncoderIntel::cachePic(VAPictureH264 *p_vpic, int32_t p_pocind) {

 if (p_pocind > 0) {
 int32_t i = (p_pocind > m_ref_count_m1 - 1 ? m_ref_count_m1 : p_pocind) - 1;

 for (int32_t i = p_pocind - 1; i > 0; i--)
 memcpy(&m_pic_param.ReferenceFrames[i],
 &m_pic_param.ReferenceFrames[i - 1], m_vap_size);
 }
 memcpy(&m_pic_param.ReferenceFrames[0], p_vpic, m_vap_size);

 m_pic_param.ReferenceFrames[0].flags = VA_PICTURE_H264_SHORT_TERM_REFERENCE;

 }**/
/**void initPicPara_1(VAPictureParameterBufferH264 *p_pic_para,YangH264Header *header){
 //Yang_SPS *sps, Yang_PPS *pps) {
 Yang_SPS *sps=&header->m_sps;
 Yang_PPS *pps=&header->m_pps;
 //YangSliceContext *sl=header->sl;
 //(*p_pic_para) =  (VAPictureParameterBufferH264){
 p_pic_para->picture_width_in_mbs_minus1 = sps->mb_width-1;//conf->Video_Width_Zb - 1;
 p_pic_para->picture_height_in_mbs_minus1 = sps->mb_height-1;//conf->Video_Height_Zb - 1;

 p_pic_para->bit_depth_luma_minus8 = sps->bit_depth_luma-8;
 p_pic_para->bit_depth_chroma_minus8 = sps->bit_depth_chroma-8;
 p_pic_para->num_ref_frames = sps->ref_frame_count;
 //.seq_fields.bits = {
 p_pic_para->seq_fields.bits.chroma_format_idc = sps->chroma_format_idc;
 p_pic_para->seq_fields.bits.residual_colour_transform_flag = sps->residual_color_transform_flag;
 p_pic_para->seq_fields.bits.gaps_in_frame_num_value_allowed_flag =
 sps->gaps_in_frame_num_allowed_flag;
 p_pic_para->seq_fields.bits.frame_mbs_only_flag = sps->frame_mbs_only_flag;
 p_pic_para->seq_fields.bits.mb_adaptive_frame_field_flag =
 sps->mb_aff;
 p_pic_para->seq_fields.bits.direct_8x8_inference_flag =
 sps->direct_8x8_inference_flag;
 p_pic_para->seq_fields.bits.MinLumaBiPredSize8x8 = sps->level_idc >= 31; //Ap_pic_para->seq_fields.bits.3p_pic_para->seq_fields.bits.3p_pic_para->seq_fields.bits.2
 p_pic_para->seq_fields.bits.log2_max_frame_num_minus4 =
 sps->log2_max_frame_num - 4;
 p_pic_para->seq_fields.bits.pic_order_cnt_type = sps->poc_type;
 p_pic_para->seq_fields.bits.log2_max_pic_order_cnt_lsb_minus4 =
 sps->log2_max_poc_lsb - 4;
 p_pic_para->seq_fields.bits.delta_pic_order_always_zero_flag =
 sps->delta_pic_order_always_zero_flag;
 // };
 p_pic_para->num_slice_groups_minus1 = pps->slice_group_count - 1;
 p_pic_para->slice_group_map_type = pps->mb_slice_group_map_type;
 p_pic_para->slice_group_change_rate_minus1 =0; //
 p_pic_para->pic_init_qp_minus26 = pps->init_qp-26; //init_qp - 26;
 p_pic_para->pic_init_qs_minus26 = pps->init_qs-26; //init_qs - 26;
 p_pic_para->chroma_qp_index_offset = pps->chroma_qp_index_offset[0];
 p_pic_para->second_chroma_qp_index_offset = pps->chroma_qp_index_offset[1];
 //.pic_fields.bits = {
 p_pic_para->pic_fields.bits.entropy_coding_mode_flag =pps->cabac;
 p_pic_para->pic_fields.bits.weighted_pred_flag = pps->weighted_pred;
 p_pic_para->pic_fields.bits.weighted_bipred_idc = pps->weighted_bipred_idc;
 p_pic_para->pic_fields.bits.transform_8x8_mode_flag =pps->transform_8x8_mode;
 p_pic_para->pic_fields.bits.field_pic_flag =0;//sl->picture_structure != Yang_PICT_FRAME;
 p_pic_para->pic_fields.bits.constrained_intra_pred_flag =
 pps->constrained_intra_pred;
 p_pic_para->pic_fields.bits.pic_order_present_flag =
 pps->pic_order_present;
 p_pic_para->pic_fields.bits.deblocking_filter_control_present_flag =
 pps->deblocking_filter_parameters_present;
 p_pic_para->pic_fields.bits.redundant_pic_cnt_present_flag =
 pps->redundant_pic_cnt_present;
 p_pic_para->pic_fields.bits.reference_pic_flag = 1;	  //h->nal_ref_idc != 0;
 // },
 p_pic_para->frame_num = 0;
 // };

 }
 int32_t yang_get_slice_type( YangSliceContext *sl)
 {
 switch (sl->slice_type) {
 case Yang_PICTURE_TYPE_P:
 return 0;
 case Yang_PICTURE_TYPE_B:
 return 1;
 case Yang_PICTURE_TYPE_I:
 return 2;
 case Yang_PICTURE_TYPE_SP:
 return 3;
 case Yang_PICTURE_TYPE_SI:
 return 4;
 default:
 return -1;
 }
 }**/

void encoding2display_order(unsigned int64_t encoding_order, int32_t intra_period,
		int32_t intra_idr_period, int32_t ip_period,
		unsigned int64_t *displaying_order, int32_t *frame_type) {
	int32_t encoding_order_gop = 0;

	if (intra_period == 1) { /* all are I/IDR frames */
		*displaying_order = encoding_order;
		if (intra_idr_period == 0)
			*frame_type = (encoding_order == 0) ? FRAME_IDR : FRAME_I;
		else
			*frame_type =
					(encoding_order % intra_idr_period == 0) ?
							FRAME_IDR : FRAME_I;
		return;
	}

	if (intra_period == 0)
		intra_idr_period = 0;

	/* new sequence like
	 * IDR PPPPP IPPPPP
	 * IDR (PBB)(PBB)(IBB)(PBB)
	 */
	encoding_order_gop =
			(intra_idr_period == 0) ?
					encoding_order :
					(encoding_order
							% (intra_idr_period + ((ip_period == 1) ? 0 : 1)));

	if (encoding_order_gop == 0) { /* the first frame */
		*frame_type = FRAME_IDR;
		*displaying_order = encoding_order;
	} else if (((encoding_order_gop - 1) % ip_period) != 0) { /* B frames */
		*frame_type = FRAME_B;
		*displaying_order = encoding_order - 1;
	} else if ((intra_period != 0) && /* have I frames */
	(encoding_order_gop >= 2)
			&& ((ip_period == 1 && encoding_order_gop % intra_period == 0)
					|| /* for IDR PPPPP IPPPP */
					/* for IDR (PBB)(PBB)(IBB) */
					(ip_period >= 2
							&& ((encoding_order_gop - 1) / ip_period
									% (intra_period / ip_period)) == 0))) {
		*frame_type = FRAME_I;
		*displaying_order = encoding_order + ip_period - 1;
	} else {
		*frame_type = FRAME_P;
		*displaying_order = encoding_order + ip_period - 1;
	}
}

#define partition(ref, field, key, ascending)   \
    while (i <= j) {                            \
        if (ascending) {                        \
            while (ref[i].field < key)          \
                i++;                            \
            while (ref[j].field > key)          \
                j--;                            \
        } else {                                \
            while (ref[i].field > key)          \
                i++;                            \
            while (ref[j].field < key)          \
                j--;                            \
        }                                       \
        if (i <= j) {                           \
            tmp = ref[i];                       \
            ref[i] = ref[j];                    \
            ref[j] = tmp;                       \
            i++;                                \
            j--;                                \
        }                                       \
    }                                           \

void sort_one(VAPictureH264 ref[], int32_t left, int32_t right, int32_t ascending,
		int32_t frame_idx) {
	int32_t i = left, j = right;
	uint32_t  key;
	VAPictureH264 tmp;

	if (frame_idx) {
		key = ref[(left + right) / 2].frame_idx;
		partition(ref, frame_idx, key, ascending);
	} else {
		key = ref[(left + right) / 2].TopFieldOrderCnt;
		partition(ref, TopFieldOrderCnt, (signed int32_t )key, ascending);
	}

	/* recursion */
	if (left < j)
		sort_one(ref, left, j, ascending, frame_idx);

	if (i < right)
		sort_one(ref, i, right, ascending, frame_idx);
}

void sort_two(VAPictureH264 ref[], int32_t left, int32_t right, uint32_t  key,
		uint32_t  frame_idx, int32_t partition_ascending, int32_t list0_ascending,
		int32_t list1_ascending) {
	int32_t i = left, j = right;
	VAPictureH264 tmp;

	if (frame_idx) {
		partition(ref, frame_idx, key, partition_ascending);
	} else {
		partition(ref, TopFieldOrderCnt, (signed int32_t )key, partition_ascending);
	}

	sort_one(ref, left, i - 1, list0_ascending, frame_idx);
	sort_one(ref, j + 1, right, list1_ascending, frame_idx);
}

void YangH264EncoderIntel::sps_rbsp(bitstream *bs) {
	int32_t profile_idc = PROFILE_IDC_BASELINE;

	//	if (h264_profile == VAProfileH264High)
	//	profile_idc = PROFILE_IDC_HIGH;
	//	else if (h264_profile == VAProfileH264Main)
	//profile_idc = PROFILE_IDC_MAIN;

	bitstream_put_ui(bs, profile_idc, 8); /* profile_idc */
	bitstream_put_ui(bs, !!(m_ymd.constraint_set_flag & 1), 1); /* constraint_set0_flag */
	bitstream_put_ui(bs, !!(m_ymd.constraint_set_flag & 2), 1); /* constraint_set1_flag */
	bitstream_put_ui(bs, !!(m_ymd.constraint_set_flag & 4), 1); /* constraint_set2_flag */
	bitstream_put_ui(bs, !!(m_ymd.constraint_set_flag & 8), 1); /* constraint_set3_flag */
	bitstream_put_ui(bs, 0, 4); /* reserved_zero_4bits */
	bitstream_put_ui(bs, m_seq_param.level_idc, 8); /* level_idc */
	bitstream_put_ue(bs, m_seq_param.seq_parameter_set_id); /* seq_parameter_set_id */

	if (profile_idc == PROFILE_IDC_HIGH) {
		bitstream_put_ue(bs, 1); /* chroma_format_idc = 1, 4:2:0 */
		bitstream_put_ue(bs, 0); /* bit_depth_luma_minus8 */
		bitstream_put_ue(bs, 0); /* bit_depth_chroma_minus8 */
		bitstream_put_ui(bs, 0, 1); /* qpprime_y_zero_transform_bypass_flag */
		bitstream_put_ui(bs, 0, 1); /* seq_scaling_matrix_present_flag */
	}

	bitstream_put_ue(bs, m_seq_param.seq_fields.bits.log2_max_frame_num_minus4); /* log2_max_frame_num_minus4 */
	bitstream_put_ue(bs, m_seq_param.seq_fields.bits.pic_order_cnt_type); /* pic_order_cnt_type */

	if (m_seq_param.seq_fields.bits.pic_order_cnt_type == 0)
		bitstream_put_ue(bs,
				m_seq_param.seq_fields.bits.log2_max_pic_order_cnt_lsb_minus4); /* log2_max_pic_order_cnt_lsb_minus4 */
	else {
		//assert(0);
	}

	bitstream_put_ue(bs, m_seq_param.max_num_ref_frames); /* num_ref_frames */
	bitstream_put_ui(bs, 0, 1); /* gaps_in_frame_num_value_allowed_flag */

	bitstream_put_ue(bs, m_seq_param.picture_width_in_mbs - 1); /* pic_width_in_mbs_minus1 */
	bitstream_put_ue(bs, m_seq_param.picture_height_in_mbs - 1); /* pic_height_in_map_units_minus1 */
	bitstream_put_ui(bs, m_seq_param.seq_fields.bits.frame_mbs_only_flag, 1); /* frame_mbs_only_flag */

	if (!m_seq_param.seq_fields.bits.frame_mbs_only_flag) {
		// assert(0);
	}

	bitstream_put_ui(bs, m_seq_param.seq_fields.bits.direct_8x8_inference_flag,
			1); /* direct_8x8_inference_flag */
	bitstream_put_ui(bs, m_seq_param.frame_cropping_flag, 1); /* frame_cropping_flag */

	if (m_seq_param.frame_cropping_flag) {
		bitstream_put_ue(bs, m_seq_param.frame_crop_left_offset); /* frame_crop_left_offset */
		bitstream_put_ue(bs, m_seq_param.frame_crop_right_offset); /* frame_crop_right_offset */
		bitstream_put_ue(bs, m_seq_param.frame_crop_top_offset); /* frame_crop_top_offset */
		bitstream_put_ue(bs, m_seq_param.frame_crop_bottom_offset); /* frame_crop_bottom_offset */
	}

	//if ( frame_bit_rate < 0 ) { //TODO EW: the vui header isn't correct
	//if (!m_seq_param.vui_parameters_present_flag) {
	if (1) {
		bitstream_put_ui(bs, 0, 1); /* vui_parameters_present_flag */
	} else {
		bitstream_put_ui(bs, m_seq_param.vui_parameters_present_flag, 1); /* vui_parameters_present_flag */
		bitstream_put_ui(bs, m_seq_param.vui_fields.bits.aspect_ratio_info_present_flag, 1); /* aspect_ratio_info_present_flag */
		bitstream_put_ui(bs, 0, 1); /* overscan_info_present_flag */
		bitstream_put_ui(bs, 0, 1); /* video_signal_type_present_flag */
		bitstream_put_ui(bs, 0, 1); /* chroma_loc_info_present_flag */
		bitstream_put_ui(bs, m_seq_param.vui_fields.bits.timing_info_present_flag, 1); /* timing_info_present_flag */
		if(m_seq_param.vui_fields.bits.timing_info_present_flag){
			bitstream_put_ui(bs, 15, 32);
			bitstream_put_ui(bs, 50, 32);
			bitstream_put_ui(bs, m_seq_param.vui_fields.bits.fixed_frame_rate_flag, 1);
		}
		bitstream_put_ui(bs, 0, 1); /* nal_hrd_parameters_present_flag */
		if(0){
			// hrd_parameters
			bitstream_put_ue(bs, 0); /* cpb_cnt_minus1 */
			bitstream_put_ui(bs, 4, 4); /* bit_rate_scale */
			bitstream_put_ui(bs, 6, 4); /* cpb_size_scale */

			bitstream_put_ue(bs, m_ymd.frame_bitrate - 1); /* bit_rate_value_minus1[0] */
			bitstream_put_ue(bs, m_ymd.frame_bitrate * 8 - 1); /* cpb_size_value_minus1[0] */
			bitstream_put_ui(bs, 1, 1); /* cbr_flag[0] */

			bitstream_put_ui(bs, 23, 5); /* initial_cpb_removal_delay_length_minus1 */
			bitstream_put_ui(bs, 23, 5); /* cpb_removal_delay_length_minus1 */
			bitstream_put_ui(bs, 23, 5); /* dpb_output_delay_length_minus1 */
			bitstream_put_ui(bs, 23, 5); /* time_offset_length  */
		}
		bitstream_put_ui(bs, 0, 1); /* vcl_hrd_parameters_present_flag */
		bitstream_put_ui(bs, 0, 1); /* low_delay_hrd_flag */

		bitstream_put_ui(bs, 0, 1); /* pic_struct_present_flag */
		bitstream_put_ui(bs, 0, 1); /* bitstream_restriction_flag */
	}

	rbsp_trailing_bits(bs); /* rbsp_trailing_bits */
}

int32_t YangH264EncoderIntel::build_packed_pic_buffer(
		uint8_t **header_buffer) {
	bitstream bs;

	bitstream_start(&bs);
	nal_start_code_prefix(&bs);
	nal_header(&bs, NAL_REF_IDC_HIGH, NAL_PPS);
	pps_rbsp(&bs);
	bitstream_end(&bs);

	*header_buffer = (uint8_t *) bs.buffer;
	return bs.bit_offset;
}

int32_t YangH264EncoderIntel::build_packed_slice_buffer(
		uint8_t **header_buffer) {
	bitstream bs;
	int32_t is_idr = !!m_pic_param.pic_fields.bits.idr_pic_flag;
	int32_t is_ref = !!m_pic_param.pic_fields.bits.reference_pic_flag;

	bitstream_start(&bs);
	nal_start_code_prefix(&bs);

	if (IS_I_SLICE(m_slice_param.slice_type)) {
		nal_header(&bs, NAL_REF_IDC_HIGH, is_idr ? NAL_IDR : NAL_NON_IDR);
	} else if (IS_P_SLICE(m_slice_param.slice_type)) {
		nal_header(&bs, NAL_REF_IDC_MEDIUM, NAL_NON_IDR);
	} else {
		//assert(IS_B_SLICE(slice_param.slice_type));
		nal_header(&bs, is_ref ? NAL_REF_IDC_LOW : NAL_REF_IDC_NONE,
		NAL_NON_IDR);
	}

	slice_header(&bs);
	bitstream_end(&bs);

	*header_buffer = (uint8_t *) bs.buffer;
	return bs.bit_offset;
}

int32_t YangH264EncoderIntel::build_packed_seq_buffer(
		uint8_t **header_buffer) {
	bitstream bs;

	bitstream_start(&bs);
	nal_start_code_prefix(&bs);
	nal_header(&bs, NAL_REF_IDC_HIGH, NAL_SPS);
	sps_rbsp(&bs);
	bitstream_end(&bs);

	*header_buffer = (uint8_t *) bs.buffer;
	return bs.bit_offset;
}

void YangH264EncoderIntel::pps_rbsp(bitstream *bs) {
	bitstream_put_ue(bs, m_pic_param.pic_parameter_set_id); /* m_pic_parameter_set_id */
	bitstream_put_ue(bs, m_pic_param.seq_parameter_set_id); /* seq_parameter_set_id */

	bitstream_put_ui(bs, m_pic_param.pic_fields.bits.entropy_coding_mode_flag,
			1); /* entropy_coding_mode_flag */

	bitstream_put_ui(bs, 0, 1); /* pic_order_present_flag: 0 */

	bitstream_put_ue(bs, 0); /* num_slice_groups_minus1 */

	bitstream_put_ue(bs, m_pic_param.num_ref_idx_l0_active_minus1); /* num_ref_idx_l0_active_minus1 */
	bitstream_put_ue(bs, m_pic_param.num_ref_idx_l1_active_minus1); /* num_ref_idx_l1_active_minus1 1 */

	bitstream_put_ui(bs, m_pic_param.pic_fields.bits.weighted_pred_flag, 1); /* weighted_pred_flag: 0 */
	bitstream_put_ui(bs, m_pic_param.pic_fields.bits.weighted_bipred_idc, 2); /* weighted_bipred_idc: 0 */

	bitstream_put_se(bs, m_pic_param.pic_init_qp - 26); /* pic_init_qp_minus26 */
	bitstream_put_se(bs, 0); /* pic_init_qs_minus26 */
	bitstream_put_se(bs, 0); /* chroma_qp_index_offset */

	bitstream_put_ui(bs,
			m_pic_param.pic_fields.bits.deblocking_filter_control_present_flag,
			1); /* deblocking_filter_control_present_flag */
	bitstream_put_ui(bs, 0, 1); /* constrained_intra_pred_flag */
	bitstream_put_ui(bs, 0, 1); /* redundant_pic_cnt_present_flag */

	/* more_rbsp_data */
	bitstream_put_ui(bs, m_pic_param.pic_fields.bits.transform_8x8_mode_flag,
			1); /*transform_8x8_mode_flag */
	bitstream_put_ui(bs, 0, 1); /* pic_scaling_matrix_present_flag */
	bitstream_put_se(bs, m_pic_param.second_chroma_qp_index_offset); /*second_chroma_qp_index_offset */

	rbsp_trailing_bits(bs);
}

void YangH264EncoderIntel::slice_header(bitstream *bs) {
	int32_t first_mb_in_slice = m_slice_param.macroblock_address;

	bitstream_put_ue(bs, first_mb_in_slice); /* first_mb_in_slice: 0 */
	bitstream_put_ue(bs, m_slice_param.slice_type); /* slice_type */
	bitstream_put_ue(bs, m_slice_param.pic_parameter_set_id); /* m_pic_parameter_set_id: 0 */
	bitstream_put_ui(bs, m_pic_param.frame_num,
			m_seq_param.seq_fields.bits.log2_max_frame_num_minus4 + 4); /* frame_num */

	/* frame_mbs_only_flag == 1 */
	if (!m_seq_param.seq_fields.bits.frame_mbs_only_flag) {
		/* FIXME: */
		// assert(0);
	}

	if (m_pic_param.pic_fields.bits.idr_pic_flag)
		bitstream_put_ue(bs, m_slice_param.idr_pic_id); /* idr_pic_id: 0 */

	if (m_seq_param.seq_fields.bits.pic_order_cnt_type == 0) {
		bitstream_put_ui(bs, m_pic_param.CurrPic.TopFieldOrderCnt,
				m_seq_param.seq_fields.bits.log2_max_pic_order_cnt_lsb_minus4
						+ 4);
		/* pic_order_present_flag == 0 */
	} else {
		/* FIXME: */
		// assert(0);
	}

	/* redundant_pic_cnt_present_flag == 0 */
	/* slice type */
	if (IS_P_SLICE(m_slice_param.slice_type)) {
		bitstream_put_ui(bs, m_slice_param.num_ref_idx_active_override_flag, 1); /* num_ref_idx_active_override_flag: */

		if (m_slice_param.num_ref_idx_active_override_flag)
			bitstream_put_ue(bs, m_slice_param.num_ref_idx_l0_active_minus1);

		/* ref_pic_list_reordering */
		bitstream_put_ui(bs, 0, 1); /* ref_pic_list_reordering_flag_l0: 0 */
	} else if (IS_B_SLICE(m_slice_param.slice_type)) {
		bitstream_put_ui(bs, m_slice_param.direct_spatial_mv_pred_flag, 1); /* direct_spatial_mv_pred: 1 */

		bitstream_put_ui(bs, m_slice_param.num_ref_idx_active_override_flag, 1); /* num_ref_idx_active_override_flag: */

		if (m_slice_param.num_ref_idx_active_override_flag) {
			bitstream_put_ue(bs, m_slice_param.num_ref_idx_l0_active_minus1);
			bitstream_put_ue(bs, m_slice_param.num_ref_idx_l1_active_minus1);
		}

		/* ref_pic_list_reordering */
		bitstream_put_ui(bs, 0, 1); /* ref_pic_list_reordering_flag_l0: 0 */
		bitstream_put_ui(bs, 0, 1); /* ref_pic_list_reordering_flag_l1: 0 */
	}

	if ((m_pic_param.pic_fields.bits.weighted_pred_flag
			&& IS_P_SLICE(m_slice_param.slice_type))
			|| ((m_pic_param.pic_fields.bits.weighted_bipred_idc == 1)
					&& IS_B_SLICE(m_slice_param.slice_type))) {
		/* FIXME: fill weight/offset table */
		// assert(0);
	}

	/* dec_ref_pic_marking */
	if (m_pic_param.pic_fields.bits.reference_pic_flag) { /* nal_ref_idc != 0 */
		uint8_t no_output_of_prior_pics_flag = 0;
		uint8_t long_term_reference_flag = 0;
		uint8_t adaptive_ref_pic_marking_mode_flag = 0;

		if (m_pic_param.pic_fields.bits.idr_pic_flag) {
			bitstream_put_ui(bs, no_output_of_prior_pics_flag, 1); /* no_output_of_prior_pics_flag: 0 */
			bitstream_put_ui(bs, long_term_reference_flag, 1); /* long_term_reference_flag: 0 */
		} else {
			bitstream_put_ui(bs, adaptive_ref_pic_marking_mode_flag, 1); /* adaptive_ref_pic_marking_mode_flag: 0 */
		}
	}

	if (m_pic_param.pic_fields.bits.entropy_coding_mode_flag
			&& !IS_I_SLICE(m_slice_param.slice_type))
		bitstream_put_ue(bs, m_slice_param.cabac_init_idc); /* cabac_init_idc: 0 */

	bitstream_put_se(bs, m_slice_param.slice_qp_delta); /* slice_qp_delta: 0 */

	/* ignore for SP/SI */

	if (m_pic_param.pic_fields.bits.deblocking_filter_control_present_flag) {
		bitstream_put_ue(bs, m_slice_param.disable_deblocking_filter_idc); /* disable_deblocking_filter_idc: 0 */

		if (m_slice_param.disable_deblocking_filter_idc != 1) {
			bitstream_put_se(bs, m_slice_param.slice_alpha_c0_offset_div2); /* slice_alpha_c0_offset_div2: 2 */
			bitstream_put_se(bs, m_slice_param.slice_beta_offset_div2); /* slice_beta_offset_div2: 2 */
		}
	}

	if (m_pic_param.pic_fields.bits.entropy_coding_mode_flag) {
		bitstream_byte_aligning(bs, 1);
	}
}
#endif
#endif
