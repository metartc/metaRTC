//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangavutil/video/YangGolomb.h>
#include <yangdecoder/pc/YangH264Header.h>
#include "memory.h"
#include "stdio.h"



static  int32_t yang_clip(int32_t a, int32_t amin, int32_t amax)
{
    if      (a < amin) return amin;
    else if (a > amax) return amax;
    else               return a;
}
#define Yang_CHROMA_QP_TABLE_END(d)                                          \
    Yang_QP(0,  d), Yang_QP(1,  d), Yang_QP(2,  d), Yang_QP(3,  d), Yang_QP(4,  d), Yang_QP(5,  d),   \
    Yang_QP(6,  d), Yang_QP(7,  d), Yang_QP(8,  d), Yang_QP(9,  d), Yang_QP(10, d), Yang_QP(11, d),   \
    Yang_QP(12, d), Yang_QP(13, d), Yang_QP(14, d), Yang_QP(15, d), Yang_QP(16, d), Yang_QP(17, d),   \
    Yang_QP(18, d), Yang_QP(19, d), Yang_QP(20, d), Yang_QP(21, d), Yang_QP(22, d), Yang_QP(23, d),   \
    Yang_QP(24, d), Yang_QP(25, d), Yang_QP(26, d), Yang_QP(27, d), Yang_QP(28, d), Yang_QP(29, d),   \
    Yang_QP(29, d), Yang_QP(30, d), Yang_QP(31, d), Yang_QP(32, d), Yang_QP(32, d), Yang_QP(33, d),   \
    Yang_QP(34, d), Yang_QP(34, d), Yang_QP(35, d), Yang_QP(35, d), Yang_QP(36, d), Yang_QP(36, d),   \
    Yang_QP(37, d), Yang_QP(37, d), Yang_QP(37, d), Yang_QP(38, d), Yang_QP(38, d), Yang_QP(38, d),   \
    Yang_QP(39, d), Yang_QP(39, d), Yang_QP(39, d), Yang_QP(39, d)

/* maximum number of MBs in the DPB for a given level */
static const int32_t yang_level_max_dpb_mbs[][2] = {
    { 10, 396       },
    { 11, 900       },
    { 12, 2376      },
    { 13, 2376      },
    { 20, 2376      },
    { 21, 4752      },
    { 22, 8100      },
    { 30, 8100      },
    { 31, 18000     },
    { 32, 20480     },
    { 40, 32768     },
    { 41, 32768     },
    { 42, 34816     },
    { 50, 110400    },
    { 51, 184320    },
    { 52, 184320    },
};

static const uint8_t yang_default_scaling4[2][16] = {
    {  6, 13, 20, 28, 13, 20, 28, 32,
      20, 28, 32, 37, 28, 32, 37, 42 },
    { 10, 14, 20, 24, 14, 20, 24, 27,
      20, 24, 27, 30, 24, 27, 30, 34 }
};

static const uint8_t yang_default_scaling8[2][64] = {
    {  6, 10, 13, 16, 18, 23, 25, 27,
      10, 11, 16, 18, 23, 25, 27, 29,
      13, 16, 18, 23, 25, 27, 29, 31,
      16, 18, 23, 25, 27, 29, 31, 33,
      18, 23, 25, 27, 29, 31, 33, 36,
      23, 25, 27, 29, 31, 33, 36, 38,
      25, 27, 29, 31, 33, 36, 38, 40,
      27, 29, 31, 33, 36, 38, 40, 42 },
    {  9, 13, 15, 17, 19, 21, 22, 24,
      13, 13, 17, 19, 21, 22, 24, 25,
      15, 17, 19, 21, 22, 24, 25, 27,
      17, 19, 21, 22, 24, 25, 27, 28,
      19, 21, 22, 24, 25, 27, 28, 30,
      21, 22, 24, 25, 27, 28, 30, 32,
      22, 24, 25, 27, 28, 30, 32, 33,
      24, 25, 27, 28, 30, 32, 33, 35 }
};

const uint8_t yang_zigzag_direct[64] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

const uint8_t yang_zigzag_scan[16+1] = {
    0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
    1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
    1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
    3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};

const uint8_t yang_h264_chroma_qp[7][Yang_QP_MAX_NUM + 1] = {
    { Yang_CHROMA_QP_TABLE_END(8) },
    { 0, 1, 2, 3, 4, 5,
      Yang_CHROMA_QP_TABLE_END(9) },
    { 0, 1, 2, 3,  4,  5,
      6, 7, 8, 9, 10, 11,
	  Yang_CHROMA_QP_TABLE_END(10) },
    { 0,  1, 2, 3,  4,  5,
      6,  7, 8, 9, 10, 11,
      12,13,14,15, 16, 17,
	  Yang_CHROMA_QP_TABLE_END(11) },
    { 0,  1, 2, 3,  4,  5,
      6,  7, 8, 9, 10, 11,
      12,13,14,15, 16, 17,
      18,19,20,21, 22, 23,
	  Yang_CHROMA_QP_TABLE_END(12) },
    { 0,  1, 2, 3,  4,  5,
      6,  7, 8, 9, 10, 11,
      12,13,14,15, 16, 17,
      18,19,20,21, 22, 23,
      24,25,26,27, 28, 29,
	  Yang_CHROMA_QP_TABLE_END(13) },
    { 0,  1, 2, 3,  4,  5,
      6,  7, 8, 9, 10, 11,
      12,13,14,15, 16, 17,
      18,19,20,21, 22, 23,
      24,25,26,27, 28, 29,
      30,31,32,33, 34, 35,
	  Yang_CHROMA_QP_TABLE_END(14) },
};
const uint8_t yang_h264_quant_div6[Yang_QP_MAX_NUM + 1] = {
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3,  3,  3,
    3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6,  6,  6,
    7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10,
   10,10,10,11,11,11,11,11,11,12,12,12,12,12,12,13,13,13, 13, 13, 13,
   14,14,14,14,
};
const uint8_t yang_h264_quant_rem6[Yang_QP_MAX_NUM + 1] = {
    0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2,
    3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2,
    3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5,
    0, 1, 2, 3,
};

const uint8_t yang_h264_dequant8_coeff_init_scan[16] = {
    0, 3, 4, 3, 3, 1, 5, 1, 4, 5, 2, 5, 3, 1, 5, 1
};

const uint8_t yang_h264_dequant8_coeff_init[6][6] = {
    { 20, 18, 32, 19, 25, 24 },
    { 22, 19, 35, 21, 28, 26 },
    { 26, 23, 42, 24, 33, 31 },
    { 28, 25, 45, 26, 35, 33 },
    { 32, 28, 51, 30, 40, 38 },
    { 36, 32, 58, 34, 46, 43 },
};
const uint8_t yang_h264_dequant4_coeff_init[6][3] = {
    { 10, 13, 16 },
    { 11, 14, 18 },
    { 13, 16, 20 },
    { 14, 18, 23 },
    { 16, 20, 25 },
    { 18, 23, 29 },
};

static const YangRational yang_h264_pixel_aspect[17] = {
    {   0,  1 },
    {   1,  1 },
    {  12, 11 },
    {  10, 11 },
    {  16, 11 },
    {  40, 33 },
    {  24, 11 },
    {  20, 11 },
    {  32, 11 },
    {  80, 33 },
    {  18, 11 },
    {  15, 11 },
    {  64, 33 },
    { 160, 99 },
    {   4,  3 },
    {   3,  2 },
    {   2,  1 },
};

const uint8_t yang_h264_golomb_to_pict_type[5] = {
    Yang_PICTURE_TYPE_P, Yang_PICTURE_TYPE_B, Yang_PICTURE_TYPE_I,
    Yang_PICTURE_TYPE_SP, Yang_PICTURE_TYPE_SI
};
static inline int32_t get_chroma_qp(Yang_PPS *pps, int32_t t, int32_t qscale)
{
    return pps->chroma_qp_table[t][qscale];
}
static void yang_build_qp_table(Yang_PPS *pps, int32_t t, int32_t index, const int32_t depth)
{
    int32_t i;
    const int32_t max_qp = 51 + 6 * (depth - 8);
    for (i = 0; i < max_qp + 1; i++)
        pps->chroma_qp_table[t][i] =
            yang_h264_chroma_qp[depth - 8][yang_clip(i + index, 0, max_qp)];
}
static void init_dequant8_coeff_table(Yang_PPS *pps, const Yang_SPS *sps)
{
    int32_t i, j, q, x;
    const int32_t max_qp = 51 + 6 * (sps->bit_depth_luma - 8);

    for (i = 0; i < 6; i++) {
        pps->dequant8_coeff[i] = pps->dequant8_buffer[i];
        for (j = 0; j < i; j++)
            if (!memcmp(pps->scaling_matrix8[j], pps->scaling_matrix8[i],
                        64 * sizeof(uint8_t))) {
                pps->dequant8_coeff[i] = pps->dequant8_buffer[j];
                break;
            }
        if (j < i)
            continue;

        for (q = 0; q < max_qp + 1; q++) {
            int32_t shift = yang_h264_quant_div6[q];
            int32_t idx   = yang_h264_quant_rem6[q];
            for (x = 0; x < 64; x++)
                pps->dequant8_coeff[i][q][(x >> 3) | ((x & 7) << 3)] =
                    ((uint32_t)yang_h264_dequant8_coeff_init[idx][yang_h264_dequant8_coeff_init_scan[((x >> 1) & 12) | (x & 3)]] *
                     pps->scaling_matrix8[i][x]) << shift;
        }
    }
}

static void init_dequant4_coeff_table(Yang_PPS *pps, const Yang_SPS *sps)
{
    int32_t i, j, q, x;
    const int32_t max_qp = 51 + 6 * (sps->bit_depth_luma - 8);
    for (i = 0; i < 6; i++) {
        pps->dequant4_coeff[i] = pps->dequant4_buffer[i];
        for (j = 0; j < i; j++)
            if (!memcmp(pps->scaling_matrix4[j], pps->scaling_matrix4[i],
                        16 * sizeof(uint8_t))) {
                pps->dequant4_coeff[i] = pps->dequant4_buffer[j];
                break;
            }
        if (j < i)
            continue;

        for (q = 0; q < max_qp + 1; q++) {
            int32_t shift = yang_h264_quant_div6[q] + 2;
            int32_t idx   = yang_h264_quant_rem6[q];
            for (x = 0; x < 16; x++)
                pps->dequant4_coeff[i][q][(x >> 2) | ((x << 2) & 0xF)] =
                    ((uint32_t)yang_h264_dequant4_coeff_init[idx][(x & 1) + ((x >> 2) & 1)] *
                     pps->scaling_matrix4[i][x]) << shift;
        }
    }
}
static void yang_init_dequant_tables(Yang_PPS *pps, const Yang_SPS *sps)
{
    int32_t i, x;
    init_dequant4_coeff_table(pps, sps);
    memset(pps->dequant8_coeff, 0, sizeof(pps->dequant8_coeff));

    if (pps->transform_8x8_mode)
        init_dequant8_coeff_table(pps, sps);
    if (sps->transform_bypass) {
        for (i = 0; i < 6; i++)
            for (x = 0; x < 16; x++)
                pps->dequant4_coeff[i][0][x] = 1 << 6;
        if (pps->transform_8x8_mode)
            for (i = 0; i < 6; i++)
                for (x = 0; x < 64; x++)
                    pps->dequant8_coeff[i][0][x] = 1 << 6;
    }
}
static int32_t yang_decode_scaling_list(YangGetBitContext *gb, uint8_t *factors, int32_t size,
                                const uint8_t *jvt_list,
                                const uint8_t *fallback_list)
{
    int32_t i, last = 8, next = 8;
    const uint8_t *scan = size == 16 ? yang_zigzag_scan : yang_zigzag_direct;
    if (!yang_get_bits1(gb)) /* matrix not written, we use the predicted one */
        memcpy(factors, fallback_list, size * sizeof(uint8_t));
    else
        for (i = 0; i < size; i++) {
            if (next) {
                int32_t v = yang_get_se_golomb(gb);
                next = (last + v) & 0xff;
            }
            if (!i && !next) { /* matrix not written, we use the preset one */
                memcpy(factors, jvt_list, size * sizeof(uint8_t));
                break;
            }
            last = factors[scan[i]] = next ? next : last;
        }
    return 0;
}

static int32_t yang_decode_scaling_matrices(YangGetBitContext *gb, const Yang_SPS *sps,
                                    const Yang_PPS *pps, int32_t is_sps,
                                    uint8_t(*scaling_matrix4)[16],
                                    uint8_t(*scaling_matrix8)[64])
{
    int32_t fallback_sps = !is_sps && sps->scaling_matrix_present;
    const uint8_t *fallback[4] = {
        fallback_sps ? sps->scaling_matrix4[0] : yang_default_scaling4[0],
        fallback_sps ? sps->scaling_matrix4[3] : yang_default_scaling4[1],
        fallback_sps ? sps->scaling_matrix8[0] : yang_default_scaling8[0],
        fallback_sps ? sps->scaling_matrix8[3] : yang_default_scaling8[1]
    };
    int32_t ret = 0;
    if (yang_get_bits1(gb)) {
        ret |= yang_decode_scaling_list(gb, scaling_matrix4[0], 16, yang_default_scaling4[0], fallback[0]);        // Intra, Y
        ret |= yang_decode_scaling_list(gb, scaling_matrix4[1], 16, yang_default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
        ret |= yang_decode_scaling_list(gb, scaling_matrix4[2], 16, yang_default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
        ret |= yang_decode_scaling_list(gb, scaling_matrix4[3], 16, yang_default_scaling4[1], fallback[1]);        // Inter, Y
        ret |= yang_decode_scaling_list(gb, scaling_matrix4[4], 16, yang_default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
        ret |= yang_decode_scaling_list(gb, scaling_matrix4[5], 16, yang_default_scaling4[1], scaling_matrix4[4]); // Inter, Cb
        if (is_sps || pps->transform_8x8_mode) {
            ret |= yang_decode_scaling_list(gb, scaling_matrix8[0], 64, yang_default_scaling8[0], fallback[2]); // Intra, Y
            ret |= yang_decode_scaling_list(gb, scaling_matrix8[3], 64, yang_default_scaling8[1], fallback[3]); // Inter, Y
            if (sps->chroma_format_idc == 3) {
                ret |= yang_decode_scaling_list(gb, scaling_matrix8[1], 64, yang_default_scaling8[0], scaling_matrix8[0]); // Intra, Cr
                ret |= yang_decode_scaling_list(gb, scaling_matrix8[4], 64, yang_default_scaling8[1], scaling_matrix8[3]); // Inter, Cr
                ret |= yang_decode_scaling_list(gb, scaling_matrix8[2], 64, yang_default_scaling8[0], scaling_matrix8[1]); // Intra, Cb
                ret |= yang_decode_scaling_list(gb, scaling_matrix8[5], 64, yang_default_scaling8[1], scaling_matrix8[4]); // Inter, Cb
            }
        }
        if (!ret)
            ret = is_sps;
    }

    return ret;
}
static  int32_t yang_decode_hrd_parameters(YangGetBitContext *gb, Yang_SPS *sps)
{
    int32_t cpb_count=0, i=0;
    cpb_count = yang_get_ue_golomb_31(gb) + 1;

    if (cpb_count > 32) {
       // av_log(avctx, AV_LOG_ERROR, "cpb_count %d invalid\n", cpb_count);
        return YangERROR_INVALIDDATA;
    }

    yang_get_bits(gb, 4); /* bit_rate_scale */
    yang_get_bits(gb, 4); /* cpb_size_scale */
    for (i = 0; i < cpb_count; i++) {
        yang_get_ue_golomb_long(gb); /* bit_rate_value_minus1 */
        yang_get_ue_golomb_long(gb); /* cpb_size_value_minus1 */
        yang_get_bits1(gb);          /* cbr_flag */
    }
    sps->initial_cpb_removal_delay_length = yang_get_bits(gb, 5) + 1;
    sps->cpb_removal_delay_length         = yang_get_bits(gb, 5) + 1;
    sps->dpb_output_delay_length          = yang_get_bits(gb, 5) + 1;
    sps->time_offset_length               = yang_get_bits(gb, 5);
    sps->cpb_cnt                          = cpb_count;
    return 0;
}
static  int32_t yang_decode_vui_parameters(YangGetBitContext *gb,Yang_SPS *sps)
{
    int32_t aspect_ratio_info_present_flag;
    uint32_t  aspect_ratio_idc;

    aspect_ratio_info_present_flag = yang_get_bits1(gb);

    if (aspect_ratio_info_present_flag) {
        aspect_ratio_idc = yang_get_bits(gb, 8);
        if (aspect_ratio_idc == Yang_EXTENDED_SAR) {
            sps->sar.num = yang_get_bits(gb, 16);
            sps->sar.den = yang_get_bits(gb, 16);
        } else if (aspect_ratio_idc < Yang_ARRAY_ELEMS(yang_h264_pixel_aspect)) {
            sps->sar = yang_h264_pixel_aspect[aspect_ratio_idc];
        } else {
           // av_log(avctx, AV_LOG_ERROR, "illegal aspect ratio\n");
            return YangERROR_INVALIDDATA;
        }
    } else {
        sps->sar.num =
        sps->sar.den = 0;
    }

    if (yang_get_bits1(gb))      /* overscan_info_present_flag */
        yang_get_bits1(gb);      /* overscan_appropriate_flag */

    sps->video_signal_type_present_flag = yang_get_bits1(gb);
    if (sps->video_signal_type_present_flag) {
        yang_get_bits(gb, 3);                 /* video_format */
        sps->full_range = yang_get_bits1(gb); /* video_full_range_flag */

        sps->colour_description_present_flag = yang_get_bits1(gb);
        if (sps->colour_description_present_flag) {
            sps->color_primaries = yang_get_bits(gb, 8); /* colour_primaries */
            sps->color_trc       = yang_get_bits(gb, 8); /* transfer_characteristics */
            sps->colorspace      = yang_get_bits(gb, 8); /* matrix_coefficients */

            // Set invalid values to "unspecified"
         //   if (!av_color_primaries_name(sps->color_primaries))
                sps->color_primaries = Yang_PRI_UNSPECIFIED;
           // if (!av_color_transfer_name(sps->color_trc))
                sps->color_trc = Yang_TRC_UNSPECIFIED;
           // if (!av_color_space_name(sps->colorspace))
                sps->colorspace = Yang_SPC_UNSPECIFIED;
        }
    }

    /* chroma_location_info_present_flag */
    if (yang_get_bits1(gb)) {
        /* chroma_sample_location_type_top_field */
       // avctx->chroma_sample_location = yang_get_ue_golomb(gb) + 1;
    	int32_t chroma_sample_location = yang_get_ue_golomb(gb) + 1;
        yang_get_ue_golomb(gb);  /* chroma_sample_location_type_bottom_field */
    }

    if (yang_show_bits1(gb) && yang_get_bits_left(gb) < 10) {
      //  av_log(avctx, AV_LOG_WARNING, "Truncated VUI\n");
        return 0;
    }

    sps->timing_info_present_flag = yang_get_bits1(gb);
    if (sps->timing_info_present_flag) {
        unsigned num_units_in_tick = yang_get_bits_long(gb, 32);
        unsigned time_scale        = yang_get_bits_long(gb, 32);
        if (!num_units_in_tick || !time_scale) {
          //  av_log(avctx, AV_LOG_ERROR,"time_scale/num_units_in_tick invalid or unsupported (%u/%u)\n",  time_scale, num_units_in_tick);
            sps->timing_info_present_flag = 0;
        } else {
            sps->num_units_in_tick = num_units_in_tick;
            sps->time_scale = time_scale;
        }
        sps->fixed_frame_rate_flag = yang_get_bits1(gb);
    }

    sps->nal_hrd_parameters_present_flag = yang_get_bits1(gb);
    if (sps->nal_hrd_parameters_present_flag)
        if (yang_decode_hrd_parameters(gb, sps) < 0)
            return YangERROR_INVALIDDATA;
    sps->vcl_hrd_parameters_present_flag = yang_get_bits1(gb);
    if (sps->vcl_hrd_parameters_present_flag)
        if (yang_decode_hrd_parameters(gb,  sps) < 0)
            return YangERROR_INVALIDDATA;
    if (sps->nal_hrd_parameters_present_flag ||
        sps->vcl_hrd_parameters_present_flag)
        yang_get_bits1(gb);     /* low_delay_hrd_flag */
    sps->pic_struct_present_flag = yang_get_bits1(gb);
    if (!yang_get_bits_left(gb))
        return 0;
    sps->bitstream_restriction_flag = yang_get_bits1(gb);
    if (sps->bitstream_restriction_flag) {
        yang_get_bits1(gb);     /* motion_vectors_over_pic_boundaries_flag */
        yang_get_ue_golomb(gb); /* max_bytes_per_pic_denom */
        yang_get_ue_golomb(gb); /* max_bits_per_mb_denom */
        yang_get_ue_golomb(gb); /* log2_max_mv_length_horizontal */
        yang_get_ue_golomb(gb); /* log2_max_mv_length_vertical */
        sps->num_reorder_frames = yang_get_ue_golomb(gb);
        yang_get_ue_golomb(gb); /*max_dec_frame_buffering*/

        if (yang_get_bits_left(gb) < 0) {
            sps->num_reorder_frames         = 0;
            sps->bitstream_restriction_flag = 0;
        }

        if (sps->num_reorder_frames > 16U
            /* max_dec_frame_buffering || max_dec_frame_buffering > 16 */) {
           // av_log(avctx, AV_LOG_ERROR, "Clipping illegal num_reorder_frames %d\n",  sps->num_reorder_frames);
            sps->num_reorder_frames = 16;
            return YangERROR_INVALIDDATA;
        }
    }

    return 0;
}

int32_t yang_parse_nal_header(YangNAL *nal)
{
    YangGetBitContext *gb = &nal->gb;

    if (yang_get_bits1(gb) != 0)
        return YangERROR_INVALIDDATA;

    nal->ref_idc = yang_get_bits(gb, 2);
    nal->type    = yang_get_bits(gb, 5);
    return 1;
}


int32_t yang_h264_parse_ref_count(uint32_t  *plist_count, uint32_t  ref_count[2],
                            YangGetBitContext *gb,  Yang_PPS *pps,
                            int32_t slice_type_nos, int32_t picture_structure)
{
    int32_t list_count;
    int32_t num_ref_idx_active_override_flag;

    // set defaults, might be overridden a few lines later
    ref_count[0] = pps->ref_count[0];
    ref_count[1] = pps->ref_count[1];

    if (slice_type_nos != Yang_PICTURE_TYPE_I) {
        unsigned max[2];
        max[0] = max[1] = picture_structure == Yang_PICT_FRAME ? 15 : 31;

        num_ref_idx_active_override_flag = yang_get_bits1(gb);

        if (num_ref_idx_active_override_flag) {
            ref_count[0] = yang_get_ue_golomb(gb) + 1;
            if (slice_type_nos == Yang_PICTURE_TYPE_B) {
                ref_count[1] = yang_get_ue_golomb(gb) + 1;
            } else
                // full range is spec-ok in this case, even for frames
                ref_count[1] = 1;
        }

        if (ref_count[0] - 1 > max[0] || ref_count[1] - 1 > max[1]) {
            printf("reference overflow %u > %u or %u > %u\n",
                   ref_count[0] - 1, max[0], ref_count[1] - 1, max[1]);
            ref_count[0] = ref_count[1] = 0;
            *plist_count = 0;
            goto fail;
        }

        if (slice_type_nos == Yang_PICTURE_TYPE_B)
            list_count = 2;
        else
            list_count = 1;
    } else {
        list_count   = 0;
        ref_count[0] = ref_count[1] = 0;
    }

    *plist_count = list_count;

    return 0;
fail:
    *plist_count = 0;
    ref_count[0] = 0;
    ref_count[1] = 0;
    return YangERROR_INVALIDDATA;
}

int32_t yang_h264_decode_ref_pic_list_reordering(YangSliceContext *sl)
{
    int32_t list=0, index=0;

    sl->nb_ref_modifications[0] = 0;
    sl->nb_ref_modifications[1] = 0;

    for (list = 0; list < sl->list_count; list++) {
        if (!yang_get_bits1(&sl->gb))    // ref_pic_list_modification_flag_l[01]
            continue;

        for (index = 0; ; index++) {
            uint32_t  op = yang_get_ue_golomb_31(&sl->gb);

            if (op == 3)
                break;

            if (index >= sl->ref_count[list]) {
                printf("reference count overflow\n");
                return YangERROR_INVALIDDATA;
            } else if (op > 2) {
                printf("illegal modification_of_pic_nums_idc %u\n", op);
                return YangERROR_INVALIDDATA;
            }
            sl->ref_modifications[list][index].val = yang_get_ue_golomb_long(&sl->gb);
            sl->ref_modifications[list][index].op  = op;
            sl->nb_ref_modifications[list]++;
        }
    }

    return 0;
}
int32_t yang_h264_pred_weight_table(YangGetBitContext *gb, Yang_SPS *sps,
                              uint32_t  *ref_count, int32_t slice_type_nos,
                              YangPredWeightTable *pwt,
                              int32_t picture_structure)
{
    int32_t list, i, j;
    int32_t luma_def, chroma_def;

    pwt->use_weight             = 0;
    pwt->use_weight_chroma      = 0;

    pwt->luma_log2_weight_denom = yang_get_ue_golomb(gb);
    if (pwt->luma_log2_weight_denom > 7U) {
        printf( "luma_log2_weight_denom %d is out of range\n", pwt->luma_log2_weight_denom);
        pwt->luma_log2_weight_denom = 0;
    }
    luma_def = 1 << pwt->luma_log2_weight_denom;

    if (sps->chroma_format_idc) {
        pwt->chroma_log2_weight_denom = yang_get_ue_golomb(gb);
        if (pwt->chroma_log2_weight_denom > 7U) {
            printf("chroma_log2_weight_denom %d is out of range\n", pwt->chroma_log2_weight_denom);
            pwt->chroma_log2_weight_denom = 0;
        }
        chroma_def = 1 << pwt->chroma_log2_weight_denom;
    }

    for (list = 0; list < 2; list++) {
        pwt->luma_weight_flag[list]   = 0;
        pwt->chroma_weight_flag[list] = 0;
        for (i = 0; i < ref_count[list]; i++) {
            int32_t luma_weight_flag, chroma_weight_flag;

            luma_weight_flag = yang_get_bits1(gb);
            if (luma_weight_flag) {
                pwt->luma_weight[i][list][0] = yang_get_se_golomb(gb);
                pwt->luma_weight[i][list][1] = yang_get_se_golomb(gb);
                if ((char)pwt->luma_weight[i][list][0] != pwt->luma_weight[i][list][0] ||
                    (char)pwt->luma_weight[i][list][1] != pwt->luma_weight[i][list][1])
                    goto out_range_weight;
                if (pwt->luma_weight[i][list][0] != luma_def ||
                    pwt->luma_weight[i][list][1] != 0) {
                    pwt->use_weight             = 1;
                    pwt->luma_weight_flag[list] = 1;
                }
            } else {
                pwt->luma_weight[i][list][0] = luma_def;
                pwt->luma_weight[i][list][1] = 0;
            }

            if (sps->chroma_format_idc) {
                chroma_weight_flag = yang_get_bits1(gb);
                if (chroma_weight_flag) {
                    int32_t j;
                    for (j = 0; j < 2; j++) {
                        pwt->chroma_weight[i][list][j][0] = yang_get_se_golomb(gb);
                        pwt->chroma_weight[i][list][j][1] = yang_get_se_golomb(gb);
                        if ((char)pwt->chroma_weight[i][list][j][0] != pwt->chroma_weight[i][list][j][0] ||
                            (char)pwt->chroma_weight[i][list][j][1] != pwt->chroma_weight[i][list][j][1]) {
                            pwt->chroma_weight[i][list][j][0] = chroma_def;
                            pwt->chroma_weight[i][list][j][1] = 0;
                            goto out_range_weight;
                        }
                        if (pwt->chroma_weight[i][list][j][0] != chroma_def ||
                            pwt->chroma_weight[i][list][j][1] != 0) {
                            pwt->use_weight_chroma        = 1;
                            pwt->chroma_weight_flag[list] = 1;
                        }
                    }
                } else {
                    int32_t j;
                    for (j = 0; j < 2; j++) {
                        pwt->chroma_weight[i][list][j][0] = chroma_def;
                        pwt->chroma_weight[i][list][j][1] = 0;
                    }
                }
            }

            // for MBAFF
            if (picture_structure == Yang_PICT_FRAME) {
                pwt->luma_weight[16 + 2 * i][list][0] = pwt->luma_weight[16 + 2 * i + 1][list][0] = pwt->luma_weight[i][list][0];
                pwt->luma_weight[16 + 2 * i][list][1] = pwt->luma_weight[16 + 2 * i + 1][list][1] = pwt->luma_weight[i][list][1];
                if (sps->chroma_format_idc) {
                    for (j = 0; j < 2; j++) {
                        pwt->chroma_weight[16 + 2 * i][list][j][0] = pwt->chroma_weight[16 + 2 * i + 1][list][j][0] = pwt->chroma_weight[i][list][j][0];
                        pwt->chroma_weight[16 + 2 * i][list][j][1] = pwt->chroma_weight[16 + 2 * i + 1][list][j][1] = pwt->chroma_weight[i][list][j][1];
                    }
                }
            }
        }
        if (slice_type_nos != Yang_PICTURE_TYPE_B)
            break;
    }
    pwt->use_weight = pwt->use_weight || pwt->use_weight_chroma;
    return 0;
out_range_weight:
   // avpriv_request_sample(logctx, "Out of range weight");
    return YangERROR_INVALIDDATA;
}
int32_t yang_h264_decode_ref_pic_marking(YangSliceContext *sl, YangGetBitContext *gb,
                                   YangNAL *nal)
{
    int32_t i;
    YangMMCO *mmco = sl->mmco;
    int32_t nb_mmco = 0;

    if (nal->type == Yang_NAL_IDR_SLICE) { // FIXME fields
        yang_skip_bits1(gb); // broken_link
        if (yang_get_bits1(gb)) {
            mmco[0].opcode   = Yang_MMCO_LONG;
            mmco[0].long_arg = 0;
            nb_mmco          = 1;
        }
        sl->explicit_ref_marking = 1;
    } else {
        sl->explicit_ref_marking = yang_get_bits1(gb);
        if (sl->explicit_ref_marking) {
            for (i = 0; i < Yang_MAX_MMCO_COUNT; i++) {
                Yang_MMCOOpcode opcode = Yang_MMCOOpcode(yang_get_ue_golomb_31(gb));

                mmco[i].opcode = opcode;
                if (opcode == Yang_MMCO_SHORT2UNUSED || opcode == Yang_MMCO_SHORT2LONG) {
                    mmco[i].short_pic_num =
                        (sl->curr_pic_num - yang_get_ue_golomb_long(gb) - 1) &
                            (sl->max_pic_num - 1);
                }
                if (opcode == Yang_MMCO_SHORT2LONG || opcode == Yang_MMCO_LONG2UNUSED ||
                    opcode == Yang_MMCO_LONG || opcode == Yang_MMCO_SET_MAX_LONG) {
                    uint32_t  long_arg = yang_get_ue_golomb_31(gb);
                    if (long_arg >= 32 ||
                        (long_arg >= 16 && !(opcode == Yang_MMCO_SET_MAX_LONG &&
                                             long_arg == 16) &&
                         !(opcode == Yang_MMCO_LONG2UNUSED && Yang_FIELD_PICTURE(sl)))) {
                        printf( "illegal long ref in memory management control operation %d\n", opcode);
                        return -1;
                    }
                    mmco[i].long_arg = long_arg;
                }

                if (opcode > (unsigned) Yang_MMCO_LONG) {
                    printf("illegal memory management control operation %d\n",opcode);
                    return -1;
                }
                if (opcode == Yang_MMCO_END)
                    break;
            }
            nb_mmco = i;
        }
    }

    sl->nb_mmco = nb_mmco;

    return 0;
}
