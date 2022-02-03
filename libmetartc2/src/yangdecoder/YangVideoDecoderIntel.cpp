#include "YangVideoDecoderIntel.h"
#if HaveLibva
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
#include "xf86drm.h"

using namespace std;
#define CHECK_VASTATUS(X, MSG)                {if ((X) != VA_STATUS_SUCCESS) {cout <<"\n"<<m_fd<<"_ERROR:"<<X<<":"<< #MSG << endl; }}

void YangVideoDecoderIntel::loadLib() {

	yang_vaInitialize = (VAStatus (*)(VADisplay dpy, int32_t *major_version,int32_t *minor_version)) m_lib.loadFunction("vaInitialize");
	//yang_NeAACDecOpen=(NEAACDECAPI NeAACDecHandle (*)(void))m_lib.loadFunction("");
	yang_vaGetDisplayDRM = (VADisplay (*)(int32_t fd)) m_lib1.loadFunction("vaGetDisplayDRM");

	yang_vaDestroyContext =	(VAStatus (*)(VADisplay dpy, VAContextID context)) m_lib.loadFunction("vaDestroyContext");
	yang_vaDestroySurfaces = (VAStatus (*)(VADisplay dpy, VASurfaceID *surfaces,int32_t num_surfaces)) m_lib.loadFunction("vaDestroySurfaces");
	yang_vaDestroyConfig =(VAStatus (*)(VADisplay dpy, VAConfigID config_id)) m_lib.loadFunction("vaDestroyConfig");
	yang_vaTerminate = (VAStatus (*)(VADisplay dpy)) m_lib.loadFunction("vaTerminate");

	yang_vaCreateConfig = (VAStatus (*)(VADisplay dpy, VAProfile profile,
			VAEntrypoint32_t entrypoint, VAConfigAttrib *attrib_list,
			int32_t num_attribs, VAConfigID *config_id)) m_lib.loadFunction("vaCreateConfig");

	yang_vaCreateSurfaces = (VAStatus (*)(VADisplay dpy, uint32_t  format,
			uint32_t  width, uint32_t  height, VASurfaceID *surfaces,
			uint32_t  num_surfaces, VASurfaceAttrib *attrib_list,
			uint32_t  num_attribs)) m_lib.loadFunction("vaCreateSurfaces");

	yang_vaCreateContext = (VAStatus (*)(VADisplay dpy, VAConfigID config_id,
			int32_t picture_width, int32_t picture_height, int32_t flag,
			VASurfaceID *render_targets, int32_t num_render_targets,
			VAContextID *context)) m_lib.loadFunction("vaCreateContext");
	yang_vaCreateBuffer = (VAStatus (*)(VADisplay dpy, VAContextID context,
			VABufferType type, uint32_t  size, uint32_t  num_elements,
			void *data, VABufferID *buf_id)) m_lib.loadFunction(
			"vaCreateBuffer");
	yang_vaBeginPicture = (VAStatus (*)(VADisplay dpy, VAContextID context,	VASurfaceID render_target)) m_lib.loadFunction("vaBeginPicture");
	yang_vaRenderPicture = (VAStatus (*)(VADisplay dpy, VAContextID context,VABufferID *buffers, int32_t num_buffers)) m_lib.loadFunction("vaRenderPicture");

	yang_vaCreateImage = (VAStatus (*)(VADisplay dpy, VAImageFormat *format,int32_t width, int32_t height, VAImage *image)) m_lib.loadFunction("vaCreateImage");
	yang_vaEndPicture =(VAStatus (*)(VADisplay dpy, VAContextID context)) m_lib.loadFunction("vaEndPicture");
	yang_vaGetImage =(VAStatus (*)(VADisplay dpy, VASurfaceID surface, int32_t x, int32_t y,uint32_t  width, uint32_t  height, VAImageID image)) m_lib.loadFunction("vaGetImage");
	yang_vaMapBuffer = (VAStatus (*)(VADisplay dpy, VABufferID buf_id,void **pbuf)) m_lib.loadFunction("vaMapBuffer");
	yang_vaSyncSurface =(VAStatus (*)(VADisplay dpy, VASurfaceID render_target)) m_lib.loadFunction("vaSyncSurface");
	yang_vaUnmapBuffer =(VAStatus (*)(VADisplay dpy, VABufferID buf_id)) m_lib.loadFunction("vaUnmapBuffer");

}

void YangVideoDecoderIntel::unloadLib() {
	yang_vaGetDisplayDRM = NULL;
	yang_vaInitialize = NULL;
	yang_vaDestroyContext = NULL;
	yang_vaDestroySurfaces = NULL;
	yang_vaDestroyConfig = NULL;
	yang_vaTerminate = NULL;

	yang_vaCreateConfig = NULL;
	yang_vaCreateSurfaces = NULL;

	yang_vaCreateContext = NULL;
	yang_vaCreateBuffer = NULL;
	yang_vaBeginPicture = NULL;
	yang_vaRenderPicture = NULL;

	yang_vaCreateImage = NULL;
	yang_vaEndPicture = NULL;
	yang_vaGetImage = NULL;
	yang_vaMapBuffer = NULL;
	yang_vaSyncSurface = NULL;
	yang_vaUnmapBuffer = NULL;

}
void initPicPara_1(VAPictureParameterBufferH264 *p_pic_para,
		YangH264Header *header) {
	//Yang_SPS *sps, Yang_PPS *pps) {
	Yang_SPS *sps = &header->m_sps;
	Yang_PPS *pps = &header->m_pps;
	//YangSliceContext *sl=header->sl;
	//(*p_pic_para) =  (VAPictureParameterBufferH264){
	p_pic_para->picture_width_in_mbs_minus1 = sps->mb_width - 1;//conf->Video_Width_Zb - 1;
	p_pic_para->picture_height_in_mbs_minus1 = sps->mb_height - 1;//conf->Video_Height_Zb - 1;

	p_pic_para->bit_depth_luma_minus8 = sps->bit_depth_luma - 8;
	p_pic_para->bit_depth_chroma_minus8 = sps->bit_depth_chroma - 8;
	p_pic_para->num_ref_frames = sps->ref_frame_count;
	//.seq_fields.bits = {
	p_pic_para->seq_fields.bits.chroma_format_idc = sps->chroma_format_idc;
	p_pic_para->seq_fields.bits.residual_colour_transform_flag =
			sps->residual_color_transform_flag;
	p_pic_para->seq_fields.bits.gaps_in_frame_num_value_allowed_flag =
			sps->gaps_in_frame_num_allowed_flag;
	p_pic_para->seq_fields.bits.frame_mbs_only_flag = sps->frame_mbs_only_flag;
	p_pic_para->seq_fields.bits.mb_adaptive_frame_field_flag = sps->mb_aff;
	p_pic_para->seq_fields.bits.direct_8x8_inference_flag =
			sps->direct_8x8_inference_flag;
	p_pic_para->seq_fields.bits.MinLumaBiPredSize8x8 = sps->level_idc >= 31; /* Ap_pic_para->seq_fields.bits.3p_pic_para->seq_fields.bits.3p_pic_para->seq_fields.bits.2 */
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
	p_pic_para->slice_group_change_rate_minus1 = 0; /* FMO is not implemented */
	p_pic_para->pic_init_qp_minus26 = pps->init_qp - 26; //init_qp - 26;
	p_pic_para->pic_init_qs_minus26 = pps->init_qs - 26; //init_qs - 26;
	p_pic_para->chroma_qp_index_offset = pps->chroma_qp_index_offset[0];
	p_pic_para->second_chroma_qp_index_offset = pps->chroma_qp_index_offset[1];
	//.pic_fields.bits = {
	p_pic_para->pic_fields.bits.entropy_coding_mode_flag = pps->cabac;
	p_pic_para->pic_fields.bits.weighted_pred_flag = pps->weighted_pred;
	p_pic_para->pic_fields.bits.weighted_bipred_idc = pps->weighted_bipred_idc;
	p_pic_para->pic_fields.bits.transform_8x8_mode_flag =
			pps->transform_8x8_mode;
	p_pic_para->pic_fields.bits.field_pic_flag = 0; //sl->picture_structure != Yang_PICT_FRAME;
	p_pic_para->pic_fields.bits.constrained_intra_pred_flag =
			pps->constrained_intra_pred;
	p_pic_para->pic_fields.bits.pic_order_present_flag = pps->pic_order_present;
	p_pic_para->pic_fields.bits.deblocking_filter_control_present_flag =
			pps->deblocking_filter_parameters_present;
	p_pic_para->pic_fields.bits.redundant_pic_cnt_present_flag =
			pps->redundant_pic_cnt_present;
	p_pic_para->pic_fields.bits.reference_pic_flag = 1;	  //h->nal_ref_idc != 0;
	// },
	p_pic_para->frame_num = 0;
	// };

}
int32_t yang_get_slice_type(YangSliceContext *sl) {
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
}
YangVideoDecoderIntel::YangVideoDecoderIntel() {
	/**m_va_dpy = NULL;
	 m_config_id = VA_INVALID_ID;
	 memset(pics, 0, sizeof(YangDecodePicture)*Para_Bufs);
	 memset(&m_picPara, 0, sizeof(m_picPara));
	 memset(&iq_matrix, 0, sizeof(iq_matrix));
	 memset(&image_format, 0, sizeof(image_format));
	 memset(&m_slice_param, 0, sizeof(m_slice_param));
	 sid=0;va-drm
	 frameIdx=0;
	 FieldOrderCnt=0;
	 m_ref_count=3;**/
	//printf("\n*****YangH264Decoder***********************************Intel\n");

	m_va_dpy = NULL;
	m_config_id = VA_INVALID_ID;

	m_config_id = 0;
	m_vaContext = 0;
	vas = 0;
	memset(pics, 0, sizeof(YangDecodePicture) * Para_Bufs);
	memset(&m_picPara, 0, sizeof(m_picPara));
	memset(&iq_matrix, 0, sizeof(iq_matrix));
	memset(&image_format, 0, sizeof(image_format));
	memset(&m_slice_param, 0, sizeof(m_slice_param));
	m_fd = 0;
	m_ref_count = 0;
	m_ref_count_m1 = 0;
	sid = 0;
	frameIdx = 0;
	FieldOrderCnt = 0;
	m_ref_count = 3;
	//m_yvp = NULL;
	m_width = 0;
	m_height = 0;
	m_frame = 0;
	yLen = 0;
	uLen = 0;
	uvLen = 0;
	allLen = 0;
	frameIdx = 0;
	FieldOrderCnt = 0;
	posNum = 0;
	m_isInit = 0;
	unloadLib();
}

YangVideoDecoderIntel::~YangVideoDecoderIntel() {
//	printf("\n**************YangVideoDecoderIntel destruct***************\n");
	closeDevice();
	unloadLib();
	m_lib.unloadObject();
	m_lib1.unloadObject();
}

void YangVideoDecoderIntel::initH264Pic() {

	for (int32_t i = 0; i < 16; i++) {
		m_picPara.ReferenceFrames[i].flags = VA_PICTURE_H264_INVALID;
		m_picPara.ReferenceFrames[i].picture_id = 0xffffffff;
		m_picPara.ReferenceFrames[i].TopFieldOrderCnt = 0;
		m_picPara.ReferenceFrames[i].BottomFieldOrderCnt = 0;
	}
}

void YangVideoDecoderIntel::closeDevice() {
	//vaDestroySurfaces(m_va_dpy,&m_vaSurface,1);
	for (int32_t i = 0; i < Para_Bufs; i++) {
		if (m_va_dpy && surface_id[i] != VA_INVALID_ID) {
			vas = yang_vaDestroySurfaces(m_va_dpy, &surface_id[i], 1);
			CHECK_VASTATUS(vas, "vaDestroySurfaces");
		}
	}
	if (m_va_dpy && m_vaContext != VA_INVALID_ID) {
		vas = yang_vaDestroyContext(m_va_dpy, m_vaContext);
		CHECK_VASTATUS(vas, "vaDestroyContext");
	}
	if (m_va_dpy && m_config_id != VA_INVALID_ID) {
		vas = yang_vaDestroyConfig(m_va_dpy, m_config_id);
		CHECK_VASTATUS(vas, "vaDestroyConfig");
	}
	if (m_va_dpy) {
		vas = yang_vaTerminate(m_va_dpy);
		CHECK_VASTATUS(vas, "vaTerminate");
	}
	if (m_fd >= 0)
		close(m_fd);
	m_va_dpy = NULL;

}

void YangVideoDecoderIntel::initSlicePara(VASliceParameterBufferH264 *slice264,
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
	if (slice264->slice_type == 2) {
		slice264->chroma_weight_l0[0][0] = 0;
		slice264->chroma_weight_l0[0][1] = 0;
		slice264->chroma_weight_l1[0][0] = 0;
		slice264->chroma_weight_l1[0][1] = 0;
		slice264->RefPicList0[0].picture_id = 0xffffffff;
	} else {
		slice264->chroma_weight_l0[0][0] = 1;
		slice264->chroma_weight_l0[0][1] = 1;
		slice264->chroma_weight_l1[0][0] = 1;
		slice264->chroma_weight_l1[0][1] = 1;
	}

	for (int32_t i = 0; i < 32; i++) {
		slice264->RefPicList0[i].flags = VA_PICTURE_H264_INVALID;
		slice264->RefPicList1[i].flags = VA_PICTURE_H264_INVALID;
		slice264->RefPicList0[i].picture_id = 0xffffffff;
		slice264->RefPicList1[i].picture_id = 0xffffffff;
	}
}

void YangVideoDecoderIntel::cachePic(VAPictureH264 *p_vpic, int32_t p_pocind) {

	if (p_pocind > 0) {
		if (p_pocind > m_ref_count_m1) {
			memcpy(&m_picPara.ReferenceFrames[m_ref_count_m1],	&m_pic[m_ref_count_m1], sizeof(VAPictureH264));
			memcpy(&m_slice_param.RefPicList0[m_ref_count_m1],	&m_pic[m_ref_count_m1], sizeof(VAPictureH264));
		}
		int32_t i = (p_pocind > m_ref_count_m1 - 1 ? m_ref_count_m1 : p_pocind) - 1;

		for (; i >= 0; i--) {
			memcpy(&m_picPara.ReferenceFrames[i], &m_pic[i],sizeof(VAPictureH264));
			memcpy(&m_slice_param.RefPicList0[i], &m_pic[i],sizeof(VAPictureH264));
			memcpy(&m_pic[i + 1], &m_pic[i], sizeof(VAPictureH264));
		}
	}
	memcpy(&m_pic[0], p_vpic, sizeof(VAPictureH264));
	m_pic[0].flags = VA_PICTURE_H264_SHORT_TERM_REFERENCE;

}
void YangVideoDecoderIntel::init() {

}
void YangVideoDecoderIntel::parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid,
		int32_t *phei, int32_t *pfps) {
	//printf("\n*****************************header.........\n");
	//for(int32_t i=0;i<60;i++){
	//	printf("%02x,",*(p+i));
	//}
	//printf("\n**********************************************\n");
	m_lib.loadSysObject("libva");
	m_lib1.loadSysObject("libva-drm");
	loadLib();
	m_fd = open("/dev/dri/renderD128", O_RDWR);
	m_va_dpy = yang_vaGetDisplayDRM(m_fd);
	int32_t major_version = 0, minor_version = 0;
	if (m_va_dpy)
		vas = yang_vaInitialize(m_va_dpy, &major_version, &minor_version);
	CHECK_VASTATUS(vas, "vaInitialize");
	//printf("Initialised VAAPI connection: version %d.%d,..md=%d\n",	major_version, minor_version, m_fd);
	image_format.fourcc = VA_FOURCC_NV12;
	image_format.depth = 8;
	frameIdx = 0;
	posNum = 0;
	h264header.parseRtmpHeader(p);
	initWH();
	*pwid = m_width;
	*phei = m_height;
	*pfps = h264header.fps;
	m_isInit = 1;
}
void YangVideoDecoderIntel::initWH() {
	memset(iq_matrix.ScalingList4x4, 0x10, sizeof(iq_matrix.ScalingList4x4));
	memset(iq_matrix.ScalingList8x8[0], 0x10,
			sizeof(iq_matrix.ScalingList8x8[0]));
	memset(iq_matrix.ScalingList8x8[1], 0x10,
			sizeof(iq_matrix.ScalingList8x8[0]));
	m_width = h264header.width;
	m_height = h264header.height;
	//m_yvp->initSdlWin(2,&m_yvp->rects[0], m_width, m_height, 0, 0);
//	printf("\nmd=%d*********************intel decode init wid=%d,hei=%d,fps=%d,profile=%d..........%d\n",
//			m_fd, m_width, m_height, h264header.fps,
//			h264header.m_sps.profile_idc, h264header.m_sps.ref_frame_count);
	initPicPara_1(&m_picPara, &h264header);
	yLen = m_width * m_height;
	uLen = yLen / 4;
	allLen = yLen * 3 / 2;
	uvLen = yLen / 2;
	m_ref_count = h264header.m_sps.ref_frame_count;
	if (m_ref_count > 16)
		m_ref_count = 16;
	if (m_ref_count < 3)
		m_ref_count = 3;
	m_ref_count_m1 = m_ref_count - 1;
	VAConfigAttrib attrib;
	VAProfile vap = VAProfileH264ConstrainedBaseline;
	if (h264header.m_sps.profile_idc == 77)
		vap = VAProfileH264Main;
	if (h264header.m_sps.profile_idc == 100
			|| h264header.m_sps.profile_idc == 110
			|| h264header.m_sps.profile_idc == 122)
		vap = VAProfileH264High;
	vas = yang_vaCreateConfig(m_va_dpy, vap, VAEntrypointVLD, &attrib, 1,
			&m_config_id);
	CHECK_VASTATUS(vas, "vaCreateConfig");
	vas = yang_vaCreateSurfaces(m_va_dpy, VA_RT_FORMAT_YUV420, m_width,
			m_height, surface_id, Para_Bufs, NULL, 0);
	CHECK_VASTATUS(vas, "vaCreateSurfaces");

	vas = yang_vaCreateContext(m_va_dpy, m_config_id, m_width, m_height,
			VA_PROGRESSIVE, &surface_id[0], Para_Bufs, &m_vaContext);
	CHECK_VASTATUS(vas, "vaCreateContext");
}
void YangVideoDecoderIntel::init(YangVideoParam *p_config) {

}

int32_t YangVideoDecoderIntel::decode(YangVideoBuffer *pvpl, int32_t isIframe,
		uint8_t *pData, int32_t nSize, int64_t ptimestamp,
		uint8_t *dest, int32_t *pnFrameReturned) {

	h264header.h264_slice_header_parse(pData, nSize);
	initSlicePara(&m_slice_param, pData, nSize);
	initH264Pic();
	//m_picPara.frame_num = frameIdx;
	if (h264header.sl)
		m_picPara.pic_fields.bits.field_pic_flag =
				h264header.sl->picture_structure != Yang_PICT_FRAME;

	if (m_slice_param.slice_type == 2) {
		posNum = 0;
		FieldOrderCnt = 0;
	}
	m_picPara.CurrPic.picture_id = surface_id[sid];
	m_picPara.CurrPic.frame_idx = frameIdx;
	m_picPara.CurrPic.flags = 0;
	m_picPara.CurrPic.BottomFieldOrderCnt = FieldOrderCnt;
	m_picPara.CurrPic.TopFieldOrderCnt = FieldOrderCnt;
	m_picPara.frame_num = posNum;

	cachePic(&m_picPara.CurrPic, posNum);
	//yang_printme(&m_slice_param);
	//yang_printMe2(&m_picPara);
	vas = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAPictureParameterBufferType, sizeof(VAPictureParameterBufferH264),
			1, &m_picPara, &pics[sid].picBuf);
	CHECK_VASTATUS(vas, "vaCreateBuffer VAPictureParameterBufferType");

	vas = yang_vaCreateBuffer(m_va_dpy, m_vaContext, VAIQMatrixBufferType,
			sizeof(VAIQMatrixBufferH264), 1, &iq_matrix, &pics[sid].IQMBuf);
	CHECK_VASTATUS(vas, "vaCreateBuffer VAIQMatrixBufferType");

	vas = yang_vaCreateBuffer(m_va_dpy, m_vaContext, VASliceParameterBufferType,
			sizeof(VASliceParameterBufferH264), 1, &m_slice_param,
			&pics[sid].sliceParaBuf);
	CHECK_VASTATUS(vas, "vaCreateBuffer VASliceParameterBufferType");
	vas = yang_vaCreateBuffer(m_va_dpy, m_vaContext, VASliceDataBufferType,
			nSize, 1, pData, &pics[sid].sliceDataBuf);
	CHECK_VASTATUS(vas, "vaCreateBuffer VASliceDataBufferType");

	vas = yang_vaBeginPicture(m_va_dpy, m_vaContext, surface_id[sid]);
	CHECK_VASTATUS(vas, "vaBeginPicture");

	bufids[0] = pics[sid].picBuf;
	bufids[1] = pics[sid].IQMBuf;
	vas = yang_vaRenderPicture(m_va_dpy, m_vaContext, bufids, 2);
	CHECK_VASTATUS(vas, "vaRenderPicture 1,2");

	bufids[0] = pics[sid].sliceParaBuf;
	bufids[1] = pics[sid].sliceDataBuf;
	vas = yang_vaRenderPicture(m_va_dpy, m_vaContext, bufids, 2);
	CHECK_VASTATUS(vas, "vaRenderPicture 3,4");

	vas = yang_vaEndPicture(m_va_dpy, m_vaContext);
	CHECK_VASTATUS(vas, "vaEndPicture");
	vas = yang_vaSyncSurface(m_va_dpy, surface_id[sid]);
	CHECK_VASTATUS(vas, "vaSyncSurface");

	frameIdx++;
	if (frameIdx > 15)
		frameIdx = 0;
	FieldOrderCnt += 2;
	posNum++;
	//memcpy(&m_old_pic, &m_picPara.CurrPic, sizeof(VAPictureH264));
	// vas = vaPutSurface(m_va_dpy, surface_id[sid], win, 0, 0, m_width, m_height, 0, 0, m_width, m_height, NULL, 0, VA_FRAME_PICTURE);
	// CHECK_VASTATUS(vas, "vaPutSurface");

	uint8_t *picData1;
	vas = yang_vaCreateImage(m_va_dpy, &image_format, m_width, m_height,
			&m_img);
	CHECK_VASTATUS(vas, "vaCreateImage");
	vas = yang_vaGetImage(m_va_dpy, surface_id[sid], 0, 0, m_width, m_height,
			m_img.image_id);
	CHECK_VASTATUS(vas, "vaGetImage");
	vas = yang_vaMapBuffer(m_va_dpy, m_img.buf, (void**) &picData1);
	CHECK_VASTATUS(vas, "vaMapBuffer img");

	memcpy(dest, picData1, allLen);
	*pnFrameReturned = m_img.data_size;
	//if (vas == 0)	t_yvp.yuv_show(dest, m_width);
	if (vas == 0)
		pvpl->putVideo(picData1, allLen, ptimestamp);
	vas = yang_vaUnmapBuffer(m_va_dpy, m_img.buf);
	picData1=NULL;
	//mapDrm(sid);
	sid++;
	if (sid == Para_Bufs)
		sid = 0;
	return 1;

}
#endif
