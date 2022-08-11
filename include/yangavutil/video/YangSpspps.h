//
// Copyright (c) 2019-2022 yanggaofeng
//


#ifndef SRC_YANGAVUTIL_YANGSPSPPS_H_
#define SRC_YANGAVUTIL_YANGSPSPPS_H_
#include <stdint.h>
typedef struct {
	int32_t forbidden_zero_bit;
	int32_t nal_ref_idc;
	int32_t nal_unit_type;
	int32_t profile_idc;
	int32_t constraint_set0_flag;
	int32_t constraint_set1_flag;
	int32_t constraint_set2_flag;
	int32_t constraint_set3_flag;
	int32_t reserved_zero_4bits;
	int32_t level_idc;
	int32_t seq_parameter_set_id;
	int32_t chroma_format_idc;
	int32_t residual_colour_transform_flag;
	int32_t bit_depth_luma_minus8;
	int32_t bit_depth_chroma_minus8;
	int32_t qpprime_y_zero_transform_bypass_flag;
	int32_t seq_scaling_matrix_present_flag;
	int32_t seq_scaling_list_present_flag[8];
	int32_t log2_max_frame_num_minus4;
	int32_t pic_order_cnt_type;
	int32_t log2_max_pic_order_cnt_lsb_minus4;
	int32_t delta_pic_order_always_zero_flag;
	int32_t offset_for_non_ref_pic;
	int32_t offset_for_top_to_bottom_field;
	int32_t num_ref_frames_in_pic_order_cnt_cycle;
	int32_t num_ref_frames;
	int32_t gaps_in_frame_num_value_allowed_flag;
	int32_t pic_width_in_mbs_minus1;
	int32_t pic_height_in_map_units_minus1;
	int32_t frame_mbs_only_flag;
	int32_t mb_adaptive_frame_field_flag;
	int32_t direct_8x8_inference_flag;
	int32_t frame_cropping_flag;
	int32_t frame_crop_left_offset;
	int32_t frame_crop_right_offset;
	int32_t frame_crop_top_offset;
	int32_t frame_crop_bottom_offset;
	int32_t vui_parameter_present_flag;
	int32_t aspect_ratio_info_present_flag;
	int32_t aspect_ratio_idc;
	int32_t sar_width;
	int32_t sar_height;
	int32_t overscan_info_present_flag;
	int32_t overscan_appropriate_flagu;
	int32_t video_signal_type_present_flag;
	int32_t video_format;
	int32_t video_full_range_flag;
	int32_t colour_description_present_flag;
	int32_t colour_primaries;
	int32_t transfer_characteristics;
	int32_t matrix_coefficients;
	int32_t chroma_loc_info_present_flag;
	int32_t chroma_sample_loc_type_top_field;
	int32_t chroma_sample_loc_type_bottom_field;
	int32_t timing_info_present_flag;
	int32_t num_units_in_tick;
	int32_t time_scale;
}YangSps;
#ifdef __cplusplus
extern "C"{
#endif
int yang_h264_decode_spspps(uint8_t * buf,uint32_t nLen,int32_t *p_width,int32_t *p_height,int32_t *p_fps);
#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGAVUTIL_YANGSPSPPS_H_ */
