#ifndef YANGbittype_H_
#define YANGbittype_H_
#include "stdint.h"
#define Yang_QP_MAX_NUM (51 + 6*6)
typedef struct YangGetBitContext {
	uint8_t *buffer, *buffer_end;
	int32_t index;
	int32_t size_in_bits;
	int32_t size_in_bits_plus8;
} YangGetBitContext;
enum YangPictureType {
    Yang_PICTURE_TYPE_NONE = 0, ///< Undefined
    Yang_PICTURE_TYPE_I,     ///< Intra
    Yang_PICTURE_TYPE_P,     ///< Predicted
    Yang_PICTURE_TYPE_B,     ///< Bi-dir predicted
    Yang_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG-4
    Yang_PICTURE_TYPE_SI,    ///< Switching Intra
    Yang_PICTURE_TYPE_SP,    ///< Switching Predicted
    Yang_PICTURE_TYPE_BI,    ///< BI type
};
typedef struct YangPredWeightTable {
    int32_t use_weight;
    int32_t use_weight_chroma;
    int32_t luma_log2_weight_denom;
    int32_t chroma_log2_weight_denom;
    int32_t luma_weight_flag[2];    ///< 7.4.3.2 luma_weight_lX_flag
    int32_t chroma_weight_flag[2];  ///< 7.4.3.2 chroma_weight_lX_flag
    // The following 2 can be changed to char but that causes a 10 CPU cycles speed loss
    int32_t luma_weight[48][2][2];
    int32_t chroma_weight[48][2][2][2];
    int32_t implicit_weight[48][48][2];
} YangPredWeightTable;
typedef struct YangNAL {
    uint8_t *rbsp_buffer;

    int32_t size;
    uint8_t *data;

    /**
     * Size, in bits, of just the data, excluding the stop bit and any trailing
     * padding. I.e. what HEVC calls SODB.
     */
    int32_t size_bits;

    int32_t raw_size;
     uint8_t *raw_data;

    YangGetBitContext gb;

    /**
     * NAL unit type
     */
    int32_t type;

    /**
     * HEVC only, nuh_temporal_id_plus_1 - 1
     */
    int32_t temporal_id;

    int32_t skipped_bytes;
    int32_t skipped_bytes_pos_size;
    int32_t *skipped_bytes_pos;
    /**
     * H.264 only, nal_ref_idc
     */
    int32_t ref_idc;
} YangNAL;

typedef struct YangRBSP {
    uint8_t *rbsp_buffer;
    int32_t rbsp_buffer_alloc_size;
    int32_t rbsp_buffer_size;
} YangRBSP;

/* an input packet split into unescaped NAL units */
typedef struct YangPacket {
    YangNAL *nals;
    YangRBSP rbsp;
    int32_t nb_nals;
    int32_t nals_allocated;
} YangPacket;
typedef struct YangRational{
    int32_t num; ///< Numerator
    int32_t den; ///< Denominator
} YangRational;
typedef enum Yang_MMCOOpcode {
    Yang_MMCO_END = 0,
    Yang_MMCO_SHORT2UNUSED,
    Yang_MMCO_LONG2UNUSED,
    Yang_MMCO_SHORT2LONG,
    Yang_MMCO_SET_MAX_LONG,
    Yang_MMCO_RESET,
    Yang_MMCO_LONG,
} Yang_MMCOOpcode;
typedef struct YangMMCO {
    Yang_MMCOOpcode opcode;
    int32_t short_pic_num;  ///< pic_num without wrapping (pic_num & max_pic_num)
    int32_t long_arg;       ///< index, pic_num, or num long refs depending on opcode
} YangMMCO;

enum YangColorTransferCharacteristic {
    Yang_TRC_RESERVED0    = 0,
    Yang_TRC_BT709        = 1,  ///< also ITU-R BT1361
    Yang_TRC_UNSPECIFIED  = 2,
    Yang_TRC_RESERVED     = 3,
    Yang_TRC_GAMMA22      = 4,  ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
    Yang_TRC_GAMMA28      = 5,  ///< also ITU-R BT470BG
    Yang_TRC_SMPTE170M    = 6,  ///< also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
    Yang_TRC_SMPTE240M    = 7,
    Yang_TRC_LINEAR       = 8,  ///< "Linear transfer characteristics"
    Yang_TRC_LOG          = 9,  ///< "Logarithmic transfer characteristic (100:1 range)"
    Yang_TRC_LOG_SQRT     = 10, ///< "Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)"
    Yang_TRC_IEC61966_2_4 = 11, ///< IEC 61966-2-4
    Yang_TRC_BT1361_ECG   = 12, ///< ITU-R BT1361 Extended Colour Gamut
    Yang_TRC_IEC61966_2_1 = 13, ///< IEC 61966-2-1 (sRGB or sYCC)
    Yang_TRC_BT2020_10    = 14, ///< ITU-R BT2020 for 10-bit system
    Yang_TRC_BT2020_12    = 15, ///< ITU-R BT2020 for 12-bit system
    Yang_TRC_SMPTE2084    = 16, ///< SMPTE ST 2084 for 10-, 12-, 14- and 16-bit systems
    Yang_TRC_SMPTEST2084  = Yang_TRC_SMPTE2084,
    Yang_TRC_SMPTE428     = 17, ///< SMPTE ST 428-1
    Yang_TRC_SMPTEST428_1 = Yang_TRC_SMPTE428,
    Yang_TRC_ARIB_STD_B67 = 18, ///< ARIB STD-B67, known as "Hybrid log-gamma"
    Yang_TRC_NB                 ///< Not part of ABI
};
enum YangColorPrimaries {
    Yang_PRI_RESERVED0   = 0,
    Yang_PRI_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
    Yang_PRI_UNSPECIFIED = 2,
    Yang_PRI_RESERVED    = 3,
    Yang_PRI_BT470M      = 4,  ///< also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)

    Yang_PRI_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
    Yang_PRI_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    Yang_PRI_SMPTE240M   = 7,  ///< functionally identical to above
    Yang_PRI_FILM        = 8,  ///< colour filters using Illuminant C
    Yang_PRI_BT2020      = 9,  ///< ITU-R BT2020
    Yang_PRI_SMPTE428    = 10, ///< SMPTE ST 428-1 (CIE 1931 XYZ)
    Yang_PRI_SMPTEST428_1 = Yang_PRI_SMPTE428,
    Yang_PRI_SMPTE431    = 11, ///< SMPTE ST 431-2 (2011) / DCI P3
    Yang_PRI_SMPTE432    = 12, ///< SMPTE ST 432-1 (2010) / P3 D65 / Display P3
    Yang_PRI_JEDEC_P22   = 22, ///< JEDEC P22 phosphors
    Yang_PRI_NB                ///< Not part of ABI
};
enum YangColorSpace {
    Yang_SPC_RGB         = 0,  ///< order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB)
    Yang_SPC_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
    Yang_SPC_UNSPECIFIED = 2,
    Yang_SPC_RESERVED    = 3,
    Yang_SPC_FCC         = 4,  ///< FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
    Yang_SPC_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
    Yang_SPC_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    Yang_SPC_SMPTE240M   = 7,  ///< functionally identical to above
    Yang_SPC_YCGCO       = 8,  ///< Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
    Yang_SPC_YCOCG       = Yang_SPC_YCGCO,
    Yang_SPC_BT2020_NCL  = 9,  ///< ITU-R BT2020 non-constant luminance system
    Yang_SPC_BT2020_CL   = 10, ///< ITU-R BT2020 constant luminance system
    Yang_SPC_SMPTE2085   = 11, ///< SMPTE 2085, Y'D'zD'x
    Yang_SPC_CHROMA_DERIVED_NCL = 12, ///< Chromaticity-derived non-constant luminance system
    Yang_SPC_CHROMA_DERIVED_CL = 13, ///< Chromaticity-derived constant luminance system
    Yang_SPC_ICTCP       = 14, ///< ITU-R BT.2100-0, ICtCp
    Yang_SPC_NB                ///< Not part of ABI
};
typedef struct Yang_SPS {
    uint32_t  sps_id;
    int32_t profile_idc;
    int32_t level_idc;
    int32_t chroma_format_idc;
    int32_t transform_bypass;              ///< qpprime_y_zero_transform_bypass_flag
    int32_t log2_max_frame_num;            ///< log2_max_frame_num_minus4 + 4
    int32_t poc_type;                      ///< pic_order_cnt_type
    int32_t log2_max_poc_lsb;              ///< log2_max_pic_order_cnt_lsb_minus4
    int32_t delta_pic_order_always_zero_flag;
    int32_t offset_for_non_ref_pic;
    int32_t offset_for_top_to_bottom_field;
    int32_t poc_cycle_length;              ///< num_ref_frames_in_pic_order_cnt_cycle
    int32_t ref_frame_count;               ///< num_ref_frames
    int32_t gaps_in_frame_num_allowed_flag;
    int32_t mb_width;                      ///< pic_width_in_mbs_minus1 + 1
    ///< (pic_height_in_map_units_minus1 + 1) * (2 - frame_mbs_only_flag)
    int32_t mb_height;
    int32_t frame_mbs_only_flag;
    int32_t mb_aff;                        ///< mb_adaptive_frame_field_flag
    int32_t direct_8x8_inference_flag;
    int32_t crop;                          ///< frame_cropping_flag

    /* those 4 are already in luma samples */
    uint32_t  crop_left;            ///< frame_cropping_rect_left_offset
    uint32_t  crop_right;           ///< frame_cropping_rect_right_offset
    uint32_t  crop_top;             ///< frame_cropping_rect_top_offset
    uint32_t  crop_bottom;          ///< frame_cropping_rect_bottom_offset
    int32_t vui_parameters_present_flag;
    YangRational sar;
    int32_t video_signal_type_present_flag;
    int32_t full_range;
    int32_t colour_description_present_flag;
    //enum YangorPrimaries
	int32_t color_primaries;
   //enum YangorTransferCharacteristic
	int32_t color_trc;
    //enum YangorSpace
	int32_t colorspace;
    int32_t timing_info_present_flag;
    uint32_t num_units_in_tick;
    uint32_t time_scale;
    int32_t fixed_frame_rate_flag;
    short offset_for_ref_frame[256]; // FIXME dyn aloc?
    int32_t bitstream_restriction_flag;
    int32_t num_reorder_frames;
    int32_t scaling_matrix_present;
    uint8_t scaling_matrix4[6][16];
    uint8_t scaling_matrix8[6][64];
    int32_t nal_hrd_parameters_present_flag;
    int32_t vcl_hrd_parameters_present_flag;
    int32_t pic_struct_present_flag;
    int32_t time_offset_length;
    int32_t cpb_cnt;                          ///< See H.264 E.1.2
    int32_t initial_cpb_removal_delay_length; ///< initial_cpb_removal_delay_length_minus1 + 1
    int32_t cpb_removal_delay_length;         ///< cpb_removal_delay_length_minus1 + 1
    int32_t dpb_output_delay_length;          ///< dpb_output_delay_length_minus1 + 1
    int32_t bit_depth_luma;                   ///< bit_depth_luma_minus8 + 8
    int32_t bit_depth_chroma;                 ///< bit_depth_chroma_minus8 + 8
    int32_t residual_color_transform_flag;    ///< residual_colour_transform_flag
    int32_t constraint_set_flags;             ///< constraint_set[0-3]_flag
   // uint8_t data[4096];
   // size_t data_size;
} Yang_SPS;

/**
 * Picture parameter set
 */
typedef struct Yang_PPS {
    uint32_t  sps_id;
    int32_t cabac;                  ///< entropy_coding_mode_flag
    int32_t pic_order_present;      ///< pic_order_present_flag
    int32_t slice_group_count;      ///< num_slice_groups_minus1 + 1
    int32_t mb_slice_group_map_type;
    uint32_t  ref_count[2];  ///< num_ref_idx_l0/1_active_minus1 + 1
    int32_t weighted_pred;          ///< weighted_pred_flag
    int32_t weighted_bipred_idc;
    int32_t init_qp;                ///< pic_init_qp_minus26 + 26
    int32_t init_qs;                ///< pic_init_qs_minus26 + 26
    int32_t chroma_qp_index_offset[2];
    int32_t deblocking_filter_parameters_present; ///< deblocking_filter_parameters_present_flag
    int32_t constrained_intra_pred;     ///< constrained_intra_pred_flag
    int32_t redundant_pic_cnt_present;  ///< redundant_pic_cnt_present_flag
    int32_t transform_8x8_mode;         ///< transform_8x8_mode_flag
    uint8_t scaling_matrix4[6][16];
    uint8_t scaling_matrix8[6][64];
    uint8_t chroma_qp_table[2][Yang_QP_MAX_NUM+1];  ///< pre-scaled (with chroma_qp_index_offset) version of qp_table
    int32_t chroma_qp_diff;
    //uint8_t data[4096];
   // size_t data_size;

    uint32_t dequant4_buffer[6][Yang_QP_MAX_NUM + 1][16];
    uint32_t dequant8_buffer[6][Yang_QP_MAX_NUM + 1][64];
    uint32_t(*dequant4_coeff[6])[16];
    uint32_t(*dequant8_coeff[6])[64];
} Yang_PPS;
#endif
