//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifdef _WIN32
#include <yangdecoder/pc/YangH264Header1.h>

YangH264Header::YangH264Header() {

	memset(&m_sps, 0, sizeof(m_sps));
	memset(&m_pps, 0, sizeof(m_pps));
	memset(&packet, 0, sizeof(packet));
	//packet.
	nal = (YangNAL*) malloc(sizeof(YangNAL));
	rbsp = (YangRBSP*) malloc(sizeof(YangRBSP));
	rbsp->rbsp_buffer = (uint8_t*) malloc(MAX_MBPAIR_SIZE);
	nal->rbsp_buffer = rbsp->rbsp_buffer;
	packet.nals = nal;
	width = 0;
	height = 0;
	sl = NULL;
	fps=10;
}


YangH264Header::~YangH264Header() {

}

int32_t YangH264Header::h264_decode_seq_parameter_set(uint8_t *buf, int32_t len) {

	yang_packet_split(buf, len, 1, 1);
	YangGetBitContext *gb = &packet.nals->gb;
	int32_t profile_idc, level_idc, constraint_set_flags = 0;
	uint32_t  sps_id;
	int32_t i, log2_max_frame_num_minus4;
	Yang_SPS *sps = &m_sps;
	int32_t ret;

	profile_idc = yang_get_bits(gb, 8);
	constraint_set_flags |= yang_get_bits1(gb) << 0;   // constraint_set0_flag
	constraint_set_flags |= yang_get_bits1(gb) << 1;   // constraint_set1_flag
	constraint_set_flags |= yang_get_bits1(gb) << 2;   // constraint_set2_flag
	constraint_set_flags |= yang_get_bits1(gb) << 3;   // constraint_set3_flag
	constraint_set_flags |= yang_get_bits1(gb) << 4;   // constraint_set4_flag
	constraint_set_flags |= yang_get_bits1(gb) << 5;   // constraint_set5_flag
	yang_skip_bits(gb, 2);                             // reserved_zero_2bits
	level_idc = yang_get_bits(gb, 8);
	sps_id = yang_get_ue_golomb_31(gb);
	sps->sps_id = sps_id;
	sps->time_offset_length = 24;
	sps->profile_idc = profile_idc;
	sps->constraint_set_flags = constraint_set_flags;
	sps->level_idc = level_idc;
	sps->full_range = -1;

	memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
	memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
	sps->scaling_matrix_present = 0;
	sps->colorspace = Yang_SPC_UNSPECIFIED;         //2; //AVCOL_SPC_UNSPECIFIED

	if (sps->profile_idc == 100 ||  // High profile
			sps->profile_idc == 110 ||  // High10 profile
			sps->profile_idc == 122 ||  // High422 profile
			sps->profile_idc == 244 ||  // High444 Predictive profile
			sps->profile_idc == 44 ||  // Cavlc444 profile
			sps->profile_idc == 83 || // Scalable Constrained High profile (SVC)
			sps->profile_idc == 86 ||  // Scalable High Intra profile (SVC)
			sps->profile_idc == 118 ||  // Stereo High profile (MVC)
			sps->profile_idc == 128 ||  // Multiview High profile (MVC)
			sps->profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
			sps->profile_idc == 144) {  // old High444 profile
		sps->chroma_format_idc = yang_get_ue_golomb_31(gb);
		if (sps->chroma_format_idc == 3) {
			sps->residual_color_transform_flag = yang_get_bits1(gb);

		}
		sps->bit_depth_luma = yang_get_ue_golomb(gb) + 8;
		sps->bit_depth_chroma = yang_get_ue_golomb(gb) + 8;
		sps->transform_bypass = yang_get_bits1(gb);
		ret = yang_decode_scaling_matrices(gb, sps, NULL, 1,
				sps->scaling_matrix4, sps->scaling_matrix8);

		sps->scaling_matrix_present |= ret;
	} else {
		sps->chroma_format_idc = 1;
		sps->bit_depth_luma = 8;
		sps->bit_depth_chroma = 8;
	}

	log2_max_frame_num_minus4 = yang_get_ue_golomb(gb);

	sps->log2_max_frame_num = log2_max_frame_num_minus4 + 4;

	sps->poc_type = yang_get_ue_golomb_31(gb);

	if (sps->poc_type == 0) { // FIXME #define
		unsigned t = yang_get_ue_golomb(gb);
		sps->log2_max_poc_lsb = t + 4;
	} else if (sps->poc_type == 1) { // FIXME #define
		sps->delta_pic_order_always_zero_flag = yang_get_bits1(gb);
		sps->offset_for_non_ref_pic = yang_get_se_golomb(gb);
		sps->offset_for_top_to_bottom_field = yang_get_se_golomb(gb);
		sps->poc_cycle_length = yang_get_ue_golomb(gb);
		for (i = 0; i < sps->poc_cycle_length; i++)
			sps->offset_for_ref_frame[i] = yang_get_se_golomb(gb);
	}

	sps->ref_frame_count = yang_get_ue_golomb_31(gb);
	sps->gaps_in_frame_num_allowed_flag = yang_get_bits1(gb);
	sps->mb_width = yang_get_ue_golomb(gb) + 1;
	sps->mb_height = yang_get_ue_golomb(gb) + 1;
	width = sps->mb_width * 16;
	height = sps->mb_height * 16;
	//if (width == 1920)		height = 1080;
	//if (width == 320)		height = 240;
	if (width > 3840)		width = 3840;
	if (width <= 0)		width = 1280;
	if (height > 2160)		height = 2160;
	if (height <= 0)		height = 720;

	sps->frame_mbs_only_flag = yang_get_bits1(gb);

	sps->mb_height *= 2 - sps->frame_mbs_only_flag;

	if (!sps->frame_mbs_only_flag)
		sps->mb_aff = yang_get_bits1(gb);
	else
		sps->mb_aff = 0;

	sps->direct_8x8_inference_flag = yang_get_bits1(gb);
	sps->crop = yang_get_bits1(gb);
	uint32_t  crop_left = 0;
	uint32_t  crop_right = 0;
	uint32_t  crop_top = 0;
	uint32_t  crop_bottom = 0;
	if (sps->crop) {
		crop_left = yang_get_ue_golomb(gb);
		crop_right = yang_get_ue_golomb(gb);
		crop_top = yang_get_ue_golomb(gb);
		crop_bottom = yang_get_ue_golomb(gb);
		// int32_t width  = 16 * sps->mb_width;
		//int32_t height = 16 * sps->mb_height;
		sps->crop_left = 0;
		sps->crop_right = 0;
		sps->crop_top = 0;
		sps->crop_bottom = 0;


		int32_t vsub = (sps->chroma_format_idc == 1) ? 1 : 0;
		int32_t hsub = (sps->chroma_format_idc == 1 || sps->chroma_format_idc == 2) ?1 : 0;
		int32_t step_x = 1 << hsub;
		int32_t step_y = (2 - sps->frame_mbs_only_flag) << vsub;
		sps->crop_left = crop_left * step_x;
		sps->crop_right = crop_right * step_x;
		sps->crop_top = crop_top * step_y;
		sps->crop_bottom = crop_bottom * step_y;
	} else {
		sps->crop_left = 0;
		sps->crop_right = 0;
		sps->crop_top = 0;
		sps->crop_bottom = 0;
	}

	 width = width  - (sps->crop_right + sps->crop_left);
	 height = height - (sps->crop_top   + sps->crop_bottom);
	sps->vui_parameters_present_flag = yang_get_bits1(gb);
	if (sps->vui_parameters_present_flag) {
		//int32_t ret =
		yang_decode_vui_parameters(gb, sps);
	}
	if(sps->timing_info_present_flag){
		 fps=sps->time_scale/(sps->num_units_in_tick);
		 if(sps->fixed_frame_rate_flag)    fps=fps/2;

	}
	/* if the maximum delay is not stored in the SPS, derive it based on the
	 * level */
	/** if (!sps->bitstream_restriction_flag &&
	 (sps->ref_frame_count || avctx->strict_std_compliance >= FF_COMPLIANCE_STRICT)) {
	 sps->num_reorder_frames = MAX_DELAYED_PIC_COUNT - 1;
	 for (i = 0; i < FF_ARRAY_ELEMS(level_max_dpb_mbs); i++) {
	 if (level_max_dpb_mbs[i][0] == sps->level_idc) {
	 sps->num_reorder_frames = FFMIN(level_max_dpb_mbs[i][1] / (sps->mb_width * sps->mb_height),
	 sps->num_reorder_frames);
	 break;
	 }
	 }
	 }**/

	if (!sps->sar.den)
		sps->sar.den = 1;
	return 0;
}

int32_t YangH264Header::h264_decode_picture_parameter_set(uint8_t *buf, int32_t len) {

	yang_packet_split(buf, len, 1, 1);
	YangGetBitContext *gb = &packet.nals->gb;
	const Yang_SPS *sps=&m_sps;
	uint32_t  pps_id = yang_get_ue_golomb(gb);
	Yang_PPS *pps = &m_pps;
	int32_t qp_bd_offset=0;
	int32_t bits_left=0;
	int32_t ret=0;

	/** pps->data_size = gb->buffer_end - gb->buffer;
	 if (pps->data_size > sizeof(pps->data)) {
	 // av_log(avctx, AV_LOG_DEBUG, "Truncating likely oversized PPS (%"SIZE_SPECIFIER" > %"SIZE_SPECIFIER")\n", pps->data_size, sizeof(pps->data));
	 pps->data_size = sizeof(pps->data);
	 }
	 memcpy(pps->data, gb->buffer, pps->data_size);**/

	pps->sps_id = yang_get_ue_golomb_31(gb);
	pps->cabac = yang_get_bits1(gb);
	pps->pic_order_present = yang_get_bits1(gb);
	pps->slice_group_count = yang_get_ue_golomb(gb) + 1;
	if (pps->slice_group_count > 1) {
		pps->mb_slice_group_map_type = yang_get_ue_golomb(gb);
	}
	pps->ref_count[0] = yang_get_ue_golomb(gb) + 1;
	pps->ref_count[1] = yang_get_ue_golomb(gb) + 1;
	qp_bd_offset = 6 * (sps->bit_depth_luma - 8);

	pps->weighted_pred = yang_get_bits1(gb);
	pps->weighted_bipred_idc = yang_get_bits(gb, 2);
	pps->init_qp = yang_get_se_golomb(gb) + 26U + qp_bd_offset;
	pps->init_qs = yang_get_se_golomb(gb) + 26U + qp_bd_offset;
	pps->chroma_qp_index_offset[0] = yang_get_se_golomb(gb);
	if (pps->chroma_qp_index_offset[0] < -12
			|| pps->chroma_qp_index_offset[0] > 12) {
		ret = YangERROR_INVALIDDATA;
	}

	pps->deblocking_filter_parameters_present = yang_get_bits1(gb);
	pps->constrained_intra_pred = yang_get_bits1(gb);
	pps->redundant_pic_cnt_present = yang_get_bits1(gb);

	pps->transform_8x8_mode = 0;
	memcpy(pps->scaling_matrix4, sps->scaling_matrix4,
			sizeof(pps->scaling_matrix4));
	memcpy(pps->scaling_matrix8, sps->scaling_matrix8,
			sizeof(pps->scaling_matrix8));

	bits_left = len - yang_get_bits_count(gb);

	pps->chroma_qp_index_offset[1] = pps->chroma_qp_index_offset[0];
	yang_build_qp_table(pps, 0, pps->chroma_qp_index_offset[0],
			sps->bit_depth_luma);
	yang_build_qp_table(pps, 1, pps->chroma_qp_index_offset[1],
			sps->bit_depth_luma);

	yang_init_dequant_tables(pps, sps);

	if (pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
		pps->chroma_qp_diff = 1;

	return 0;

}

int32_t YangH264Header::yang_packet_split(uint8_t *buf, int32_t length, int32_t isH264,
		int32_t small_padding) {

	int32_t consumed=0, ret = 0;


	YangPacket *pkt = &packet;



	pkt->rbsp.rbsp_buffer_size = 0;
	pkt->nb_nals = 0;

	consumed = yang_extract_rbsp(buf, length, small_padding);
	ret = yang_init_get_bits(&nal->gb, nal->data, nal->size << 3);
	if (ret < 0)
		return ret;


	ret = yang_parse_nal_header(nal);

	if (ret <= 0 || nal->size <= 0 || nal->size_bits <= 0) {

		packet.nb_nals--;
	}

	return 0;
}

int32_t YangH264Header::yang_packet_split1(uint8_t *buf, int32_t length, int32_t isH264,
		int32_t small_padding) {
	//GetByteContext bc;
	int32_t ret = 0;
	YangPacket *pkt = &packet;
	pkt->rbsp.rbsp_buffer_size = 0;
	pkt->nb_nals = 0;
	nal->data = buf;
	nal->raw_data = buf;
	nal->size = length;
	nal->raw_size = length;
	ret = yang_init_get_bits(&nal->gb, nal->data, nal->size << 3);
	if (ret < 0)
		return ret;


	ret = yang_parse_nal_header(nal);

	if (ret <= 0 || nal->size <= 0 || nal->size_bits <= 0) {

		packet.nb_nals--;
	}

	return 0;
}
int32_t YangH264Header::yang_extract_rbsp(uint8_t *src, int32_t length,
		int32_t small_padding) {
	int32_t i, si, di;
	uint8_t *dst;

	packet.nals->skipped_bytes = 0;
#define STARTCODE_TEST                                                  \
        if (i + 2 < length && src[i + 1] == 0 && src[i + 2] <= 3) {     \
            if (src[i + 2] != 3 && src[i + 2] != 0) {                   \
                /* startcode, so we must be past the end */             \
                length = i;                                             \
            }                                                           \
            break;                                                      \
        }
#if HAVE_FAST_UNALIGNED
#define FIND_FIRST_ZERO                                                 \
        if (i > 0 && !src[i])                                           \
            i--;                                                        \
        while (src[i])                                                  \
            i++
#if HAVE_FAST_64BIT
	for (i = 0; i + 1 < length; i += 9) {
		if (!((~AV_RN64(src + i) &
								(AV_RN64(src + i) - 0x0100010001000101ULL)) &
						0x8000800080008080ULL))
		continue;
		FIND_FIRST_ZERO;
		STARTCODE_TEST;
		i -= 7;
	}
#else
	for (i = 0; i + 1 < length; i += 5) {
		if (!((~AV_RN32(src + i) &
								(AV_RN32(src + i) - 0x01000101U)) &
						0x80008080U))
		continue;
		FIND_FIRST_ZERO;
		STARTCODE_TEST;
		i -= 3;
	}
#endif /* HAVE_FAST_64BIT */
#else
	for (i = 0; i + 1 < length; i += 2) {
		if (src[i])
			continue;
		if (i > 0 && src[i - 1] == 0)
			i--;
		STARTCODE_TEST;
	}
#endif /* HAVE_FAST_UNALIGNED */

	if (i >= length - 1 && small_padding) { // no escaped 0
		nal->data = src;
		nal->raw_data = src;
		nal->size = length;
		nal->raw_size = length;
		return length;
	} else if (i > length)
		i = length;


	dst = nal->rbsp_buffer;

	memcpy(dst, src, i);
	si = di = i;
	while (si + 2 < length) {
		// remove escapes (very rare 1:2^22)
		if (src[si + 2] > 3) {
			dst[di++] = src[si++];
			dst[di++] = src[si++];
		} else if (src[si] == 0 && src[si + 1] == 0 && src[si + 2] != 0) {
			if (src[si + 2] == 3) { // escape
				dst[di++] = 0;
				dst[di++] = 0;
				si += 3;

				continue;
			} else
				// next start code
				goto nsc;
		}

		dst[di++] = src[si++];
	}
	while (si < length)
		dst[di++] = src[si++];

	nsc: memset(dst + di, 0, Yang_INPUT_BUFFER_PADDING_SIZE);

	nal->data = dst;
	nal->size = di;
	nal->raw_data = src;
	nal->raw_size = si;
	packet.rbsp.rbsp_buffer_size += si;

	return si;
}

void YangH264Header::parseRtmpHeader(uint8_t *buf) {
	int32_t spsLen = *(buf + 12) + 1;
	uint8_t * spsStart = buf + 13;
	int32_t ppsLen = *(spsStart + spsLen + 1) + 1;
	uint8_t *ppsStart = buf + 13 + spsLen + 2;
	h264_decode_seq_parameter_set(spsStart, spsLen);
	h264_decode_picture_parameter_set(ppsStart, ppsLen);

}
int32_t YangH264Header::h264_slice_header_parse(uint8_t *buf, int32_t length) {
	yang_packet_split1(buf, length, 1, 1);
	Yang_SPS *sps = &m_sps;
	Yang_PPS *pps = &m_pps;
	if (sl == NULL) {
		sl = (YangSliceContext*) malloc(sizeof(YangSliceContext));
		// sl->gb
	}
	yang_init_get_bits(&sl->gb, nal->data, nal->size << 3);

	sl->gb.index = 8;

	int32_t ret;
	uint32_t  slice_type, tmp, i;
	int32_t field_pic_flag, bottom_field_flag;

	int32_t picture_structure;

	sl->first_mb_addr = yang_get_ue_golomb_long(&sl->gb);

	slice_type = yang_get_ue_golomb_31(&sl->gb);

	if (slice_type > 4) {
		slice_type -= 5;
		sl->slice_type_fixed = 1;
	} else
		sl->slice_type_fixed = 0;

	slice_type = yang_h264_golomb_to_pict_type[slice_type];
	sl->slice_type = slice_type;
	sl->slice_type_nos = slice_type & 3;

	if (nal->type == Yang_NAL_IDR_SLICE
			&& sl->slice_type_nos != Yang_PICTURE_TYPE_I) {
		printf("A non-intra slice in an IDR NAL unit.\n");
		return YangERROR_INVALIDDATA;
	}

	sl->pps_id = yang_get_ue_golomb(&sl->gb);
	if (sl->pps_id >= Yang_MAX_PPS_COUNT) {
		printf("pps_id %u out of range\n", sl->pps_id);
		return YangERROR_INVALIDDATA;
	}
	sl->frame_num = yang_get_bits(&sl->gb, sps->log2_max_frame_num);
	sl->mb_mbaff = 0;

	if (sps->frame_mbs_only_flag) {
		picture_structure = Yang_PICT_FRAME;
	} else {
		if (!sps->direct_8x8_inference_flag
				&& slice_type == Yang_PICTURE_TYPE_B) {
			return -1;
		}
		field_pic_flag = yang_get_bits1(&sl->gb);
		if (field_pic_flag) {
			bottom_field_flag = yang_get_bits1(&sl->gb);
			picture_structure = Yang_PICT_TOP_FIELD + bottom_field_flag;
		} else {
			picture_structure = Yang_PICT_FRAME;
		}
	}
	sl->picture_structure = picture_structure;
	sl->mb_field_decoding_flag = picture_structure != Yang_PICT_FRAME;

	if (picture_structure == Yang_PICT_FRAME) {
		sl->curr_pic_num = sl->frame_num;
		sl->max_pic_num = 1 << sps->log2_max_frame_num;
	} else {
		sl->curr_pic_num = 2 * sl->frame_num + 1;
		sl->max_pic_num = 1 << (sps->log2_max_frame_num + 1);
	}

	if (nal->type == Yang_NAL_IDR_SLICE)
		yang_get_ue_golomb_long(&sl->gb); /* idr_pic_id */

	if (sps->poc_type == 0) {
		sl->poc_lsb = yang_get_bits(&sl->gb, sps->log2_max_poc_lsb);

		if (pps->pic_order_present == 1 && picture_structure == Yang_PICT_FRAME)
			sl->delta_poc_bottom = yang_get_se_golomb(&sl->gb);
	}

	if (sps->poc_type == 1 && !sps->delta_pic_order_always_zero_flag) {
		sl->delta_poc[0] = yang_get_se_golomb(&sl->gb);

		if (pps->pic_order_present == 1 && picture_structure == Yang_PICT_FRAME)
			sl->delta_poc[1] = yang_get_se_golomb(&sl->gb);
	}

	sl->redundant_pic_count = 0;
	if (pps->redundant_pic_cnt_present)
		sl->redundant_pic_count = yang_get_ue_golomb(&sl->gb);

	if (sl->slice_type_nos == Yang_PICTURE_TYPE_B)
		sl->direct_spatial_mv_pred = yang_get_bits1(&sl->gb);

	ret = yang_h264_parse_ref_count(&sl->list_count, sl->ref_count, &sl->gb,
			pps, sl->slice_type_nos, picture_structure);
	if (ret < 0)
		return ret;

	if (sl->slice_type_nos != Yang_PICTURE_TYPE_I) {
		ret = yang_h264_decode_ref_pic_list_reordering(sl);
		if (ret < 0) {
			sl->ref_count[1] = sl->ref_count[0] = 0;
			return ret;
		}
	}

	sl->pwt.use_weight = 0;
	for (i = 0; i < 2; i++) {
		sl->pwt.luma_weight_flag[i] = 0;
		sl->pwt.chroma_weight_flag[i] = 0;
	}
	if ((pps->weighted_pred && sl->slice_type_nos == Yang_PICTURE_TYPE_P)
			|| (pps->weighted_bipred_idc == 1
					&& sl->slice_type_nos == Yang_PICTURE_TYPE_B)) {
		ret = yang_h264_pred_weight_table(&sl->gb, sps, sl->ref_count,
				sl->slice_type_nos, &sl->pwt, picture_structure);
		if (ret < 0)
			return ret;
	}

	sl->explicit_ref_marking = 0;
	if (nal->ref_idc) {
		ret = yang_h264_decode_ref_pic_marking(sl, &sl->gb, nal);
		if (ret < 0)
			return YangERROR_INVALIDDATA;
	}

	if (sl->slice_type_nos != Yang_PICTURE_TYPE_I && pps->cabac) {
		tmp = yang_get_ue_golomb_31(&sl->gb);
		if (tmp > 2) {
			return YangERROR_INVALIDDATA;
		}
		sl->cabac_init_idc = tmp;
	}

	sl->last_qscale_diff = 0;
	tmp = pps->init_qp + (unsigned) yang_get_se_golomb(&sl->gb);
	if (tmp > 51 + 6 * (sps->bit_depth_luma - 8)) {
		//av_log(h->avctx, AV_LOG_ERROR, "QP %u out of range\n", tmp);
		return YangERROR_INVALIDDATA;
	}
	sl->qscale = tmp;
	sl->chroma_qp[0] = get_chroma_qp(pps, 0, sl->qscale);
	sl->chroma_qp[1] = get_chroma_qp(pps, 1, sl->qscale);
	// FIXME qscale / qp ... stuff
	if (sl->slice_type == Yang_PICTURE_TYPE_SP)
		yang_get_bits1(&sl->gb); /* sp_for_switch_flag */
	if (sl->slice_type == Yang_PICTURE_TYPE_SP
			|| sl->slice_type == Yang_PICTURE_TYPE_SI)
		yang_get_se_golomb(&sl->gb); /* slice_qs_delta */

	sl->deblocking_filter = 1;
	sl->slice_alpha_c0_offset = 0;
	sl->slice_beta_offset = 0;
	if (pps->deblocking_filter_parameters_present) {
		tmp = yang_get_ue_golomb_31(&sl->gb);
		if (tmp > 2) {
			printf("deblocking_filter_idc %u out of range\n", tmp);
			return YangERROR_INVALIDDATA;
		}
		sl->deblocking_filter = tmp;
		if (sl->deblocking_filter < 2)
			sl->deblocking_filter ^= 1;  // 1<->0

		if (sl->deblocking_filter) {
			int32_t slice_alpha_c0_offset_div2 = yang_get_se_golomb(&sl->gb);
			int32_t slice_beta_offset_div2 = yang_get_se_golomb(&sl->gb);
			if (slice_alpha_c0_offset_div2 > 6
					|| slice_alpha_c0_offset_div2 < -6
					|| slice_beta_offset_div2 > 6
					|| slice_beta_offset_div2 < -6) {
				printf("deblocking filter parameters %d %d out of range\n",
						slice_alpha_c0_offset_div2, slice_beta_offset_div2);
				return YangERROR_INVALIDDATA;
			}
			sl->slice_alpha_c0_offset = slice_alpha_c0_offset_div2 * 2;
			sl->slice_beta_offset = slice_beta_offset_div2 * 2;
		}
	}

	return 0;
}

int32_t YangH264Header::get_bits_count(YangGetBitContext *s) {
	return yang_get_bits_count(s);
}
#endif
