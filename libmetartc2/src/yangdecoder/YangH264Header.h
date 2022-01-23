/*
 * YangH264Header.h
 *
 *  Created on: 2019年8月17日
 *      Author: yang
 */

#ifndef YANGDECODER_SRC_YANGH264HEADER_H_
#define YANGDECODER_SRC_YANGH264HEADER_H_
#include <yangavutil/video/YangBittype.h>
#include "stdint.h"
#include "stddef.h"

//******************sps...27
//0x00,0x00,0x00,0x01,0x67,0x42,0xc0,0x1f,0xd9,0x00,0x50,0x05,0xba,0x10,0x00,0x00,0x03,0x00,0x10,0x00,0x01,0x00,0x00,0xf1,0x83,0x24,0x80,
//******************pps...9
//0x00,0x00,0x00,0x01,0x68,0xcb,0x83,0xcb,0x20,


#define LEFT_MBS 1
#define Yang_MAX_SPS_COUNT          32
#define Yang_MAX_PPS_COUNT         256
#define Yang_MAX_LOG2_MAX_FRAME_NUM    (12 + 4)

#define Yang_MAX_MMCO_COUNT         66
#define Yang_PICT_FRAME         3
#define Yang_EXTENDED_SAR       255
#define Yang_QP(qP, depth) ((qP) + 6 * ((depth) - 8))
#define Yang_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

#define  Yang_NAL_IDR_SLICE     5
#define Yang_PICT_TOP_FIELD     1
#define Yang_FIELD_PICTURE(h) ((h)->picture_structure != Yang_PICT_FRAME)
//#define Yang_FRAME_MBAFF(h) sps->mb_aff && (h->picture_structure == PICT_FRAME)

//#define Yang_FIELD_OR_MBAFF_PICTURE(h) (Yang_FRAME_MBAFF(h) || Yang_FIELD_PICTURE(h))


typedef struct YangSliceContext {
   // struct H264Context *h264;
    YangGetBitContext gb;
   // ERContext er;

    int32_t slice_num;
    int32_t slice_type;
    int32_t slice_type_nos;         ///< S free slice type (SI/SP are remapped to I/P)
    int32_t slice_type_fixed;

    int32_t qscale;
    int32_t chroma_qp[2];   // QPc
    int32_t qp_thresh;      ///< QP threshold to skip loopfilter
    int32_t last_qscale_diff;

    // deblock
    int32_t deblocking_filter;          ///< disable_deblocking_filter_idc with 1 <-> 0
    int32_t slice_alpha_c0_offset;
    int32_t slice_beta_offset;

    YangPredWeightTable pwt;

    int32_t prev_mb_skipped;
    int32_t next_mb_skipped;

    int32_t chroma_pred_mode;
    int32_t intra16x16_pred_mode;

    char intra4x4_pred_mode_cache[5 * 8];
    char (*intra4x4_pred_mode);

    int32_t topleft_mb_xy;
    int32_t top_mb_xy;
    int32_t topright_mb_xy;
    int32_t left_mb_xy[LEFT_MBS];

    int32_t topleft_type;
    int32_t top_type;
    int32_t topright_type;
    int32_t left_type[LEFT_MBS];

    const uint8_t *left_block;
    int32_t topleft_partition;

    uint32_t  topleft_samples_available;
    uint32_t  top_samples_available;
    uint32_t  topright_samples_available;
    uint32_t  left_samples_available;

    ptrdiff_t linesize, uvlinesize;
    ptrdiff_t mb_linesize;  ///< may be equal to s->linesize or s->linesize * 2, for mbaff
    ptrdiff_t mb_uvlinesize;

    int32_t mb_x, mb_y;
    int32_t mb_xy;
    int32_t resync_mb_x;
    int32_t resync_mb_y;
    uint32_t  first_mb_addr;
    // index of the first MB of the next slice
    int32_t next_slice_idx;
    int32_t mb_skip_run;
    int32_t is_complex;

    int32_t picture_structure;
    int32_t mb_field_decoding_flag;
    int32_t mb_mbaff;               ///< mb_aff_frame && mb_field_decoding_flag

    int32_t redundant_pic_count;

    /**
     * number of neighbors (top and/or left) that used 8x8 dct
     */
    int32_t neighbor_transform_size;

    int32_t direct_spatial_mv_pred;
    int32_t col_parity;
    int32_t col_fieldoff;

    int32_t cbp;
    int32_t top_cbp;
    int32_t left_cbp;

    int32_t dist_scale_factor[32];
    int32_t dist_scale_factor_field[2][32];
    int32_t map_col_to_list0[2][16 + 32];
    int32_t map_col_to_list0_field[2][2][16 + 32];

    /**
     * num_ref_idx_l0/1_active_minus1 + 1
     */
    uint32_t  ref_count[2];          ///< counts frames or fields, depending on current mb mode
    uint32_t  list_count;
   // H264Ref ref_list[2][48];        /**< 0..15: frame refs, 16..47: mbaff field refs.
           //                              *   Reordered version of default_ref_list
           //                              *   according to picture reordering in slice header */
    struct {
        uint8_t op;
        uint32_t val;
    } ref_modifications[2][32];
    int32_t nb_ref_modifications[2];

    uint32_t  pps_id;

    const uint8_t *intra_pcm_ptr;
    int16_t *dc_val_base;

    uint8_t *bipred_scratchpad;
    uint8_t *edge_emu_buffer;
    uint8_t (*top_borders[2])[(16 * 3) * 2];
    int32_t bipred_scratchpad_allocated;
    int32_t edge_emu_buffer_allocated;
    int32_t top_borders_allocated[2];

    /**
     * non zero coeff count cache.
     * is 64 if not available.
     */
    /**
    DECLARE_ALIGNED(8, uint8_t, non_zero_count_cache)[15 * 8];
    DECLARE_ALIGNED(16, int16_t, mv_cache)[2][5 * 8][2];
    DECLARE_ALIGNED(8,  int8_t, ref_cache)[2][5 * 8];
    DECLARE_ALIGNED(16, uint8_t, mvd_cache)[2][5 * 8][2];
    uint8_t direct_cache[5 * 8];

    DECLARE_ALIGNED(8, uint16_t, sub_mb_type)[4];

    ///< as a DCT coefficient is int32_t in high depth, we need to reserve twice the space.
    DECLARE_ALIGNED(16, int16_t, mb)[16 * 48 * 2];
    DECLARE_ALIGNED(16, int16_t, mb_luma_dc)[3][16 * 2];
    **/
    ///< as mb is addressed by scantable[i] and scantable is uint8_t we can either
    ///< check that i is not too large or ensure that there is some unused stuff after mb
    int16_t mb_padding[256 * 2];

    uint8_t (*mvd_table[2])[2];

    /**
     * Cabac
     */
    //CABACContext cabac;
    //uint8_t cabac_state[1024];
    int32_t cabac_init_idc;

    YangMMCO mmco[Yang_MAX_MMCO_COUNT];
    int32_t  nb_mmco;
    int32_t explicit_ref_marking;

    int32_t frame_num;
    int32_t poc_lsb;
    int32_t delta_poc_bottom;
    int32_t delta_poc[2];
    int32_t curr_pic_num;
    int32_t max_pic_num;
} YangSliceContext;
#define MAX_MBPAIR_SIZE (256*1024)


class YangH264Header {
public:
	YangH264Header();
	virtual ~YangH264Header();
	Yang_SPS m_sps;
	Yang_PPS m_pps;
	YangPacket packet;
	YangNAL *nal;
	YangRBSP *rbsp;
	YangSliceContext *sl;
	int32_t width,height,fps;
	//void initGb(YangGetBitContext *gb,uint8_t *buf,int32_t bitsize);
	int32_t h264_decode_seq_parameter_set(uint8_t *buf,int32_t len);//, int32_t ignore_truncation
	int32_t h264_decode_picture_parameter_set(uint8_t *buf,int32_t len);
	int32_t yang_packet_split(uint8_t *buf, int32_t length,int32_t isH264, int32_t small_padding);
	int32_t h264_slice_header_parse(uint8_t *buf, int32_t length);
	 int32_t get_bits_count(YangGetBitContext *s);
	 void parseRtmpHeader(uint8_t *buf);
	// void getRtmpHeader(uint8_t *buf,uint8_t *src,int32_t *hLen);
private:
	int32_t yang_extract_rbsp( uint8_t *src, int32_t length, int32_t small_padding);
	int32_t yang_packet_split1(uint8_t *buf, int32_t length,int32_t isH264, int32_t small_padding);
};

#endif /* YANGDECODER_SRC_YANGH264HEADER_H_ */
