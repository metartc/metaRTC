#include "YangH264EncoderIntel1.h"
#ifndef _WIN32
#if YangLibva
void YangH264EncoderIntel::loadLib(){
	//const SpeexMode * (*yang_speex_lib_get_mode) (int32_t mode);
	yang_vaGetConfigAttributes=(VAStatus (*) (VADisplay dpy, VAProfile profile, VAEntrypoint32_t entrypoint,
	    VAConfigAttrib *attrib_list, int32_t num_attribs))m_lib.loadFunction("vaGetConfigAttributes");
	yang_vaDeriveImage=(VAStatus (*)  (VADisplay dpy,VASurfaceID surface,VAImage *image))m_lib.loadFunction("vaDeriveImage");
	yang_vaDestroyImage=(VAStatus (*)  (VADisplay dpy,VAImageID image))m_lib.loadFunction("vaDestroyImage");

	 yang_vaInitialize=(VAStatus (*) ( VADisplay dpy, int32_t *major_version,int32_t *minor_version))m_lib.loadFunction("vaInitialize");
	//yang_NeAACDecOpen=(NEAACDECAPI NeAACDecHandle (*)(void))m_lib.loadFunction("");
	yang_vaGetDisplayDRM=(VADisplay (*)(int32_t fd))m_lib1.loadFunction("vaGetDisplayDRM");

	yang_vaDestroyContext=(VAStatus (*) (VADisplay dpy,VAContextID context))m_lib.loadFunction("vaDestroyContext");
	yang_vaDestroySurfaces=(VAStatus (*) (VADisplay dpy,VASurfaceID *surfaces,int32_t num_surfaces))m_lib.loadFunction("vaDestroySurfaces");
	yang_vaDestroyConfig=(VAStatus (*) (VADisplay dpy,VAConfigID config_id))m_lib.loadFunction("vaDestroyConfig");
	yang_vaTerminate=(VAStatus (*) ( VADisplay dpy))m_lib.loadFunction("vaTerminate");


	yang_vaCreateConfig=(VAStatus (*) (VADisplay dpy, VAProfile profile, VAEntrypoint32_t entrypoint, VAConfigAttrib *attrib_list, int32_t num_attribs, VAConfigID *config_id ))m_lib.loadFunction("vaCreateConfig");

	yang_vaCreateSurfaces=(VAStatus (*)(VADisplay  dpy,uint32_t   format,uint32_t    width,
	    uint32_t         height,VASurfaceID  *surfaces,uint32_t   num_surfaces,  VASurfaceAttrib    *attrib_list,
	    uint32_t         num_attribs))m_lib.loadFunction("vaCreateSurfaces");

	yang_vaCreateContext=(VAStatus (*) (VADisplay dpy,VAConfigID config_id, int32_t picture_width,
	    int32_t picture_height, int32_t flag, VASurfaceID *render_targets,
	    int32_t num_render_targets,VAContextID *context))m_lib.loadFunction("vaCreateContext");
	yang_vaCreateBuffer=(VAStatus (*) (VADisplay dpy,VAContextID context, VABufferType type,
	    uint32_t  size, uint32_t  num_elements, void *data, VABufferID *buf_id
	))m_lib.loadFunction("vaCreateBuffer");
	yang_vaBeginPicture=(VAStatus (*) (VADisplay dpy,VAContextID context,VASurfaceID render_target))m_lib.loadFunction("vaBeginPicture");
	yang_vaRenderPicture=(VAStatus (*) (VADisplay dpy,VAContextID context, VABufferID *buffers,int32_t num_buffers))m_lib.loadFunction("vaRenderPicture");

	yang_vaCreateImage=(VAStatus (*) (VADisplay dpy,VAImageFormat *format, int32_t width, int32_t height, VAImage *image))m_lib.loadFunction("vaCreateImage");
	yang_vaEndPicture=(VAStatus (*) (VADisplay dpy,VAContextID context))m_lib.loadFunction("vaEndPicture");
	yang_vaGetImage=(VAStatus (*) (VADisplay dpy,VASurfaceID surface, int32_t x,
	    int32_t y,uint32_t  width, uint32_t  height,VAImageID image))m_lib.loadFunction("vaGetImage");
	yang_vaMapBuffer=(VAStatus (*) (VADisplay dpy,VABufferID buf_id, void **pbuf))m_lib.loadFunction("vaMapBuffer");
	yang_vaSyncSurface=(VAStatus (*) (VADisplay dpy,VASurfaceID render_target))m_lib.loadFunction("vaSyncSurface");
	yang_vaUnmapBuffer=(VAStatus (*) (VADisplay dpy,VABufferID buf_id))m_lib.loadFunction("vaUnmapBuffer");

}

void YangH264EncoderIntel::unloadLib(){
	yang_vaGetDisplayDRM=NULL;
	yang_vaGetConfigAttributes=NULL;
	yang_vaDeriveImage=NULL;
	yang_vaDestroyImage=NULL;
	yang_vaInitialize=NULL;
		yang_vaDestroyContext=NULL;
		yang_vaDestroySurfaces=NULL;
		yang_vaDestroyConfig=NULL;
		yang_vaTerminate=NULL;

		yang_vaCreateConfig=NULL;
		yang_vaCreateSurfaces=NULL;

		yang_vaCreateContext=NULL;
		yang_vaCreateBuffer=NULL;
		yang_vaBeginPicture=NULL;
		yang_vaRenderPicture=NULL;

		yang_vaCreateImage=NULL;
		yang_vaEndPicture=NULL;
		yang_vaGetImage=NULL;
		yang_vaMapBuffer=NULL;
		yang_vaSyncSurface=NULL;
		yang_vaUnmapBuffer=NULL;



}
YangH264EncoderIntel::YangH264EncoderIntel() {
	//printf("\n*****YangH264Encoder***********************************Intel\n");
	//YangH264Encoder();
	//m_context = pcontext;

	m_config_id = VA_INVALID_ID;
	m_config_id = 0;
	m_vaContext = 0;
	vas = 0;
	//memset(pics, 0, sizeof(YangDecodePicture) * Para_Bufs);
	memset(&m_pic_param, 0, sizeof(m_pic_param));
	memset(&image_format, 0, sizeof(image_format));
	memset(&m_slice_param, 0, sizeof(m_slice_param));
	memset(&m_seq_param, 0, sizeof(m_seq_param));
	memset(&m_ymd,0,sizeof(m_ymd));

	m_fd = 0;
	//m_ref_count = 0;
	//m_ref_count_m1 = 0;
	sid = 0;
	frameIdx = 0;
	FieldOrderCnt = 0;
	//m_inWidth = 0;
//	m_inHeight = 0;
//	m_outWidth = 0;
//	m_outHeight = 0;
	m_frame = 0;
	yLen = 0;
	uLen = 0;
	uvLen = 0;
	allLen = 0;
	frameIdx = 0;
	FieldOrderCnt = 0;
	posNum = 0;
	m_isInit = 0;
	m_vap_size = sizeof(VAPictureH264);
	enc_packed_header_idx = 0;
	m_width_mbaligned = 0;
	m_height_mbaligned = 0;
	current_frame_type = 0;
	initYangMeta(&m_ymd,&m_yvp);
	/**num_ref_frames = m_ymd.num_ref_frames;
	numShortTerm = m_ymd.numShortTerm;
	constraint_set_flag = m_ymd.constraint_set_flag;
	h264_packedheader = m_ymd.h264_packedheader; //support pack header?
	h264_maxref = m_ymd.h264_maxref;
	h264_entropy_mode = m_ymd.h264_entropy_mode; // cabac
	MaxFrameNum = m_ymd.MaxFrameNum;
	MaxPicOrderCntLsb = m_ymd.MaxPicOrderCntLsb;
	Log2MaxFrameNum = m_ymd.Log2MaxFrameNum;
	Log2MaxPicOrderCntLsb = m_ymd.Log2MaxPicOrderCntLsb;
	frame_count = m_ymd.frame_count;
	frame_coded = m_ymd.frame_coded;
	frame_bitrate = m_ymd.frame_bitrate;
	frame_slices = m_ymd.frame_slices;
	frame_size = m_ymd.frame_size;
	initial_qp = m_ymd.initial_qp;
	minimal_qp = m_ymd.minimal_qp;
	intra_period = m_ymd.intra_period;
	intra_idr_period = m_ymd.intra_idr_period;
	ip_period = m_ymd.ip_period;
	rc_mode = m_ymd.rc_mode;**/
	current_frame_encoding = 0;
	current_frame_display = 0;
	current_IDR_display = 0;
	current_frame_num = 0;
	misc_priv_type = 0;
	misc_priv_value = 0;
	h264_profile = VAProfileH264ConstrainedBaseline;
	config_attrib_num = 0,

	m_va_dpy = NULL;

	m_vmd = NULL;
	//isConvert = 0;
	unloadLib();
}

YangH264EncoderIntel::~YangH264EncoderIntel() {
	closeDevice();
	m_va_dpy = NULL;
	unloadLib();
	m_lib.unloadObject();
	m_lib1.unloadObject();

}

void YangH264EncoderIntel::closeDevice() {
	//vaDestroySurfaces(m_va_dpy,&m_vaSurface,1);
	for (int32_t i = 0; i < Para_Bufs; i++) {
		if (m_va_dpy && src_surface[i] != VA_INVALID_ID) {
			vas = yang_vaDestroySurfaces(m_va_dpy, &src_surface[i], 1);
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

void YangH264EncoderIntel::setVideoMetaData(YangVideoMeta *pvmd){
	//if(1) return;
	uint32_t  length_in_bits=0,length_in_bits1=0;
	uint8_t *packedseq_buffer = NULL;
	uint8_t *packedpic_buffer = NULL;
	length_in_bits = build_packed_seq_buffer(&packedseq_buffer);
	length_in_bits1 = build_packed_pic_buffer(&packedpic_buffer);
	memset(pvmd->livingMeta.buffer,0,sizeof(pvmd->livingMeta.buffer));
	setZbVmd(packedseq_buffer+4,(length_in_bits+7)/8-4,packedpic_buffer+4,(length_in_bits1+7)/8-4,pvmd->livingMeta.buffer,&pvmd->livingMeta.bufLen);

	free(packedseq_buffer);
	free(packedpic_buffer);
	packedseq_buffer=NULL;
	packedpic_buffer=NULL;
}

int32_t YangH264EncoderIntel::render_packedsequence(void) {
	VAEncPackedHeaderParameterBuffer packedheader_param_buffer;
	VABufferID packedseq_para_bufid, packedseq_data_bufid, render_id[2];
	uint32_t  length_in_bits=0;
	uint8_t *packedseq_buffer = NULL;
	VAStatus va_status;

	length_in_bits = build_packed_seq_buffer(&packedseq_buffer);
	//printf("\n*************sps********************%d\n",length_in_bits/8);
	//for(int32_t j=0;j<length_in_bits/8;j++){
	//	printf("%02x,",*(packedseq_buffer+j));
	//}
	//printf("\n*************sps********************\n");
	packedheader_param_buffer.type = VAEncPackedHeaderSequence;

	packedheader_param_buffer.bit_length = length_in_bits; /*length_in_bits*/
	packedheader_param_buffer.has_emulation_bytes = 0;
	va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncPackedHeaderParameterBufferType,
			sizeof(packedheader_param_buffer), 1, &packedheader_param_buffer,
			&packedseq_para_bufid);
	CHECK_VASTATUS(va_status, "vaCreateBuffer");

	va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncPackedHeaderDataBufferType, (length_in_bits + 7) / 8, 1,
			packedseq_buffer, &packedseq_data_bufid);
	CHECK_VASTATUS(va_status, "vaCreateBuffer");

	render_id[0] = packedseq_para_bufid;
	render_id[1] = packedseq_data_bufid;
	va_status = yang_vaRenderPicture(m_va_dpy, m_vaContext, render_id, 2);
	CHECK_VASTATUS(va_status, "vaRenderPicture");

	free(packedseq_buffer);

	return Yang_Ok;
}
int32_t YangH264EncoderIntel::upload_surface_yuv(VADisplay va_dpy, VASurfaceID surface_id, int32_t src_fourcc,
		int32_t src_width, int32_t src_height, uint8_t *src_Y,
		uint8_t *src_U, uint8_t *src_V) {
	VAImage surface_image;
	uint8_t *surface_p = NULL, *Y_start = NULL, *U_start = NULL;
	int32_t Y_pitch = 0, U_pitch = 0, row;
	VAStatus va_status;

	va_status = yang_vaDeriveImage(va_dpy, surface_id, &surface_image);
	CHECK_VASTATUS(va_status, "vaDeriveImage");

	yang_vaMapBuffer(va_dpy, surface_image.buf, (void **) &surface_p);
	// assert(VA_STATUS_SUCCESS == va_status);

	Y_start = surface_p;
	Y_pitch = surface_image.pitches[0];
	U_start = (uint8_t *) surface_p + surface_image.offsets[1];
	U_pitch = surface_image.pitches[1];

	for (row = 0; row < src_height; row++) {
		uint8_t *Y_row = Y_start + row * Y_pitch;
		memcpy(Y_row, src_Y + row * src_width, src_width);
	}

	for (row = 0; row < src_height / 2; row++) {
		uint8_t *U_row = U_start + row * U_pitch;
		memcpy(U_row, src_U + row * src_width, src_width);
	}

	yang_vaUnmapBuffer(va_dpy, surface_image.buf);

	yang_vaDestroyImage(va_dpy, surface_image.image_id);

	return Yang_Ok;
}
int32_t YangH264EncoderIntel::render_packedpicture(void) {
	VAEncPackedHeaderParameterBuffer packedheader_param_buffer;
	VABufferID packedpic_para_bufid, packedpic_data_bufid, render_id[2];
	uint32_t  length_in_bits;
	uint8_t *packedpic_buffer = NULL;
	VAStatus va_status;

	length_in_bits = build_packed_pic_buffer(&packedpic_buffer);
	//printf("\n*************pps********************%d\n",length_in_bits/8);
	//for(int32_t j=0;j<length_in_bits/8;j++){
	//	printf("%02x,",*(packedpic_buffer+j));
	//}
	//printf("\n*************pps********************\n");
	packedheader_param_buffer.type = VAEncPackedHeaderPicture;
	packedheader_param_buffer.bit_length = length_in_bits;
	packedheader_param_buffer.has_emulation_bytes = 0;

	va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncPackedHeaderParameterBufferType,
			sizeof(packedheader_param_buffer), 1, &packedheader_param_buffer,
			&packedpic_para_bufid);
	CHECK_VASTATUS(va_status, "vaCreateBuffer");

	va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncPackedHeaderDataBufferType, (length_in_bits + 7) / 8, 1,
			packedpic_buffer, &packedpic_data_bufid);
	CHECK_VASTATUS(va_status, "vaCreateBuffer");

	render_id[0] = packedpic_para_bufid;
	render_id[1] = packedpic_data_bufid;
	va_status = yang_vaRenderPicture(m_va_dpy, m_vaContext, render_id, 2);
	CHECK_VASTATUS(va_status, "vaRenderPicture");

	free(packedpic_buffer);

	return Yang_Ok;
}

void YangH264EncoderIntel::init(YangVideoParam *pvp,YangVideoEncParam *penc) {
	if(m_isInit ==1) return;
	m_lib.loadObject("va");
		m_lib1.loadObject("va-drm");
		loadLib();
		setVideoPara(pvp,penc);
	initVaPara(&m_ymd,&m_seq_param,&m_pic_param);
	//m_ref_count = 3;
//	m_ref_count_m1 = m_ref_count - 1;

	m_width_mbaligned = (m_yvp.outWidth + 15) & (~15);
	m_height_mbaligned = (m_yvp.outHeight + 15) & (~15);
	yLen = m_yvp.outWidth * m_yvp.outHeight;
	uLen = yLen / 4;
	allLen = yLen * 3 / 2;
	uvLen = yLen / 2;

	m_fd = open("/dev/dri/renderD128", O_RDWR);
	m_va_dpy = yang_vaGetDisplayDRM(m_fd);
	int32_t major_version = 0, minor_version = 0;
	if (m_va_dpy)	vas = yang_vaInitialize(m_va_dpy, &major_version, &minor_version);
	CHECK_VASTATUS(vas, "vaInitialize");
	//printf("Initialised VAAPI connection: version %d.%d,..md=%d\n",	major_version, minor_version, m_fd);

	//int32_t num_entrypoints = vaMaxNumEntrypoints(m_va_dpy);
	//VAEntrypoint32_t *entrypoints = (VAEntrypoint*) malloc(num_entrypoints * sizeof(*entrypoints));

	int32_t i = 0;
	for (i = 0; i < VAConfigAttribTypeMax; i++)
		attrib[i].type = (VAConfigAttribType) i;
	vas = yang_vaGetConfigAttributes(m_va_dpy, h264_profile, VAEntrypointEncSlice,&attrib[0], VAConfigAttribTypeMax);
	CHECK_VASTATUS(vas, "vaGetConfigAttributes");
	if (attrib[VAConfigAttribRateControl].value != VA_ATTRIB_NOT_SUPPORTED) {
		int32_t tmp = attrib[VAConfigAttribRateControl].value;

		printf("Support rate control mode (0x%x):", tmp);

		if (tmp & VA_RC_NONE)
			printf("NONE ");
		if (tmp & VA_RC_CBR)
			printf("CBR ");
		if (tmp & VA_RC_VBR)
			printf("VBR ");
		if (tmp & VA_RC_VCM)
			printf("VCM ");
		if (tmp & VA_RC_CQP)
			printf("CQP ");
		if (tmp & VA_RC_VBR_CONSTRAINED)
			printf("VBR_CONSTRAINED ");

		printf("\n");

		if (m_ymd.rc_mode == -1 || !(m_ymd.rc_mode & tmp)) {
			if (m_ymd.rc_mode != -1) {
				printf(
						"Warning: Don't support the specified RateControl mode: %s!!!, switch to ",
						rc_to_string(m_ymd.rc_mode));
			}

			for (i = 0;
					i < sizeof(rc_default_modes) / sizeof(rc_default_modes[0]);
					i++) {
				if (rc_default_modes[i] & tmp) {
					m_ymd.rc_mode = rc_default_modes[i];
					break;
				}
			}

			printf("RateControl mode: %s\n", rc_to_string(m_ymd.rc_mode));
		}

		config_attrib[config_attrib_num].type = VAConfigAttribRateControl;
		config_attrib[config_attrib_num].value = m_ymd.rc_mode;
		config_attrib_num++;
	}

	if (attrib[VAConfigAttribEncPackedHeaders].value != VA_ATTRIB_NOT_SUPPORTED) {
		int32_t tmp = attrib[VAConfigAttribEncPackedHeaders].value;

		printf("Support VAConfigAttribEncPackedHeaders\n");

		m_ymd.h264_packedheader = 1;
		config_attrib[config_attrib_num].type = VAConfigAttribEncPackedHeaders;
		config_attrib[config_attrib_num].value = VA_ENC_PACKED_HEADER_NONE;

		if (tmp & VA_ENC_PACKED_HEADER_SEQUENCE) {
			printf("Support packed sequence headers\n");
			config_attrib[config_attrib_num].value |=
					VA_ENC_PACKED_HEADER_SEQUENCE;
		}

		if (tmp & VA_ENC_PACKED_HEADER_PICTURE) {
			printf("Support packed picture headers\n");
			config_attrib[config_attrib_num].value |=
					VA_ENC_PACKED_HEADER_PICTURE;
		}

		if (tmp & VA_ENC_PACKED_HEADER_SLICE) {
			printf("Support packed slice headers\n");
			config_attrib[config_attrib_num].value |=
					VA_ENC_PACKED_HEADER_SLICE;
		}

		if (tmp & VA_ENC_PACKED_HEADER_MISC) {
			printf("Support packed misc headers\n");
			config_attrib[config_attrib_num].value |= VA_ENC_PACKED_HEADER_MISC;
		}

		enc_packed_header_idx = config_attrib_num;
		config_attrib_num++;
	}

	if (attrib[VAConfigAttribEncInterlaced].value != VA_ATTRIB_NOT_SUPPORTED) {
		int32_t tmp = attrib[VAConfigAttribEncInterlaced].value;

		printf("Support VAConfigAttribEncInterlaced\n");

		if (tmp & VA_ENC_INTERLACED_FRAME)
			printf("support VA_ENC_INTERLACED_FRAME\n");
		if (tmp & VA_ENC_INTERLACED_FIELD)
			printf("Support VA_ENC_INTERLACED_FIELD\n");
		if (tmp & VA_ENC_INTERLACED_MBAFF)
			printf("Support VA_ENC_INTERLACED_MBAFF\n");
		if (tmp & VA_ENC_INTERLACED_PAFF)
			printf("Support VA_ENC_INTERLACED_PAFF\n");

		config_attrib[config_attrib_num].type = VAConfigAttribEncInterlaced;
		config_attrib[config_attrib_num].value = VA_ENC_PACKED_HEADER_NONE;
		config_attrib_num++;
	}

	if (attrib[VAConfigAttribEncMaxRefFrames].value != VA_ATTRIB_NOT_SUPPORTED) {
		m_ymd.h264_maxref = attrib[VAConfigAttribEncMaxRefFrames].value;

		printf("Support %d RefPicList0 and %d RefPicList1\n",
				m_ymd.h264_maxref & 0xffff, (m_ymd.h264_maxref >> 16) & 0xffff);
	}

	if (attrib[VAConfigAttribEncMaxSlices].value != VA_ATTRIB_NOT_SUPPORTED)
		printf("Support %d slices\n", attrib[VAConfigAttribEncMaxSlices].value);

	if (attrib[VAConfigAttribEncSliceStructure].value != VA_ATTRIB_NOT_SUPPORTED) {
		int32_t tmp = attrib[VAConfigAttribEncSliceStructure].value;

		printf("Support VAConfigAttribEncSliceStructure\n");

		if (tmp & VA_ENC_SLICE_STRUCTURE_ARBITRARY_ROWS)
			printf("Support VA_ENC_SLICE_STRUCTURE_ARBITRARY_ROWS\n");
		if (tmp & VA_ENC_SLICE_STRUCTURE_POWER_OF_TWO_ROWS)
			printf("Support VA_ENC_SLICE_STRUCTURE_POWER_OF_TWO_ROWS\n");
		if (tmp & VA_ENC_SLICE_STRUCTURE_ARBITRARY_MACROBLOCKS)
			printf("Support VA_ENC_SLICE_STRUCTURE_ARBITRARY_MACROBLOCKS\n");
	}
	if (attrib[VAConfigAttribEncMacroblockInfo].value != VA_ATTRIB_NOT_SUPPORTED) {
		printf("Support VAConfigAttribEncMacroblockInfo\n");
	}

	vas = yang_vaCreateConfig(m_va_dpy, h264_profile, VAEntrypointEncSlice,
			&config_attrib[0], config_attrib_num, &m_config_id);
	CHECK_VASTATUS(vas, "vaCreateConfig");


	m_ymd.constraint_set_flag |= (1 << 0 | 1 << 1); /* Annex A.2.2 */
	//m_ymd.ip_period = 1;

	vas = yang_vaCreateSurfaces(m_va_dpy, VA_RT_FORMAT_YUV420, m_width_mbaligned,
			m_height_mbaligned, src_surface, Para_Bufs, NULL, 0);
	CHECK_VASTATUS(vas, "vaCreateSurfaces");

	//vas = vaCreateSurfaces(m_va_dpy, VA_RT_FORMAT_YUV420, m_width_mbaligned,m_height_mbaligned, &ref_surface[0], Para_Bufs, NULL, 0);
	//CHECK_VASTATUS(vas, "vaCreateSurfaces ref");

	//VASurfaceID *tmp_surfaceid = (VASurfaceID *) calloc(2 * Para_Bufs, sizeof(VASurfaceID));
	//memcpy(tmp_surfaceid, src_surface, Para_Bufs * sizeof(VASurfaceID));
	//memcpy(tmp_surfaceid + Para_Bufs, ref_surface,Para_Bufs * sizeof(VASurfaceID));
//	vas = vaCreateContext(m_va_dpy, m_config_id, m_width_mbaligned,
//			m_height_mbaligned, VA_PROGRESSIVE, tmp_surfaceid, 2 * Para_Bufs,
//			&m_vaContext);
	vas = yang_vaCreateContext(m_va_dpy, m_config_id, m_width_mbaligned,
				m_height_mbaligned, VA_PROGRESSIVE, src_surface, Para_Bufs,
				&m_vaContext);
	CHECK_VASTATUS(vas, "vaCreateContext");
	//free(tmp_surfaceid);

	int32_t codedbuf_size = 0;
	codedbuf_size = (m_width_mbaligned * m_height_mbaligned * 400) / (16 * 16);
	for (int32_t i = 0; i < Para_Bufs; i++) {
		/* create coded buffer once for all
		 * other VA buffers which won't be used again after vaRenderPicture.
		 * so APP can always vaCreateBuffer for every frame
		 * but coded buffer need to be mapped and accessed after vaRenderPicture/vaEndPicture
		 * so VA won't maintain the coded buffer
		 */
		vas = yang_vaCreateBuffer(m_va_dpy, m_vaContext, VAEncCodedBufferType,
				codedbuf_size, 1, NULL, &coded_buf[i]);
		CHECK_VASTATUS(vas, "vaCreateBuffer");
	}
	frameIdx = 0;
	posNum = 0;
	m_isInit=1;
//	printSeq(&m_seq_param);
	//printPic(&m_pic_param);

}

void YangH264EncoderIntel::render_packedslice() {
	VAEncPackedHeaderParameterBuffer packedheader_param_buffer;
	VABufferID packedslice_para_bufid, packedslice_data_bufid, render_id[2];
	uint32_t  length_in_bits;
	uint8_t *packedslice_buffer = NULL;
	//VAStatus va_status;

	length_in_bits = build_packed_slice_buffer(&packedslice_buffer);
	packedheader_param_buffer.type = VAEncPackedHeaderSlice;
	packedheader_param_buffer.bit_length = length_in_bits;
	packedheader_param_buffer.has_emulation_bytes = 0;

	vas = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncPackedHeaderParameterBufferType,
			sizeof(packedheader_param_buffer), 1, &packedheader_param_buffer,
			&packedslice_para_bufid);
	CHECK_VASTATUS(vas, "vaCreateBuffer");

	vas = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncPackedHeaderDataBufferType, (length_in_bits + 7) / 8, 1,
			packedslice_buffer, &packedslice_data_bufid);
	CHECK_VASTATUS(vas, "vaCreateBuffer");

	render_id[0] = packedslice_para_bufid;
	render_id[1] = packedslice_data_bufid;
	vas = yang_vaRenderPicture(m_va_dpy, m_vaContext, render_id, 2);
	CHECK_VASTATUS(vas, "vaRenderPicture");

	free(packedslice_buffer);
}
int32_t YangH264EncoderIntel::upload_surface_yuv(VASurfaceID surface_id,
		int32_t src_width, int32_t src_height, uint8_t *src_Y,
		uint8_t *src_U, uint8_t *src_V) {
	VAImage surface_image;
	uint8_t *surface_p = NULL, *Y_start = NULL, *U_start = NULL;
	int32_t Y_pitch = 0, U_pitch = 0, row;

	vas = yang_vaDeriveImage(m_va_dpy, surface_id, &surface_image);
	CHECK_VASTATUS(vas, "vaDeriveImage");

	yang_vaMapBuffer(m_va_dpy, surface_image.buf, (void **) &surface_p);
	// assert(VA_STATUS_SUCCESS == va_status);

	Y_start = surface_p;
	Y_pitch = surface_image.pitches[0];
	U_start = (uint8_t *) surface_p + surface_image.offsets[1];
	U_pitch = surface_image.pitches[1];

	/* copy Y plane */
	for (row = 0; row < src_height; row++) {
		uint8_t *Y_row = Y_start + row * Y_pitch;
		memcpy(Y_row, src_Y + row * src_width, src_width);
	}
	//int32_t j = 0;
	for (row = 0; row < src_height / 2; row++) {
		uint8_t *U_row = U_start + row * U_pitch;
	//	uint8_t *u_ptr = NULL, *v_ptr = NULL;

		memcpy(U_row, src_U + row * src_width, src_width);

	}
	yang_vaUnmapBuffer(m_va_dpy, surface_image.buf);
	yang_vaDestroyImage(m_va_dpy, surface_image.image_id);
	return Yang_Ok;
}
int32_t YangH264EncoderIntel::calc_poc(int32_t pic_order_cnt_lsb) {
	static int32_t PicOrderCntMsb_ref = 0, pic_order_cnt_lsb_ref = 0;
	int32_t prevPicOrderCntMsb, prevPicOrderCntLsb;
	int32_t PicOrderCntMsb, TopFieldOrderCnt;

	if (current_frame_type == FRAME_IDR)
		prevPicOrderCntMsb = prevPicOrderCntLsb = 0;
	else {
		prevPicOrderCntMsb = PicOrderCntMsb_ref;
		prevPicOrderCntLsb = pic_order_cnt_lsb_ref;
	}

	if ((pic_order_cnt_lsb < prevPicOrderCntLsb)
			&& ((prevPicOrderCntLsb - pic_order_cnt_lsb)
					>= (int) (m_ymd.MaxPicOrderCntLsb / 2)))
		PicOrderCntMsb = prevPicOrderCntMsb + m_ymd.MaxPicOrderCntLsb;
	else if ((pic_order_cnt_lsb > prevPicOrderCntLsb)
			&& ((pic_order_cnt_lsb - prevPicOrderCntLsb)
					> (int) (m_ymd.MaxPicOrderCntLsb / 2)))
		PicOrderCntMsb = prevPicOrderCntMsb - m_ymd.MaxPicOrderCntLsb;
	else
		PicOrderCntMsb = prevPicOrderCntMsb;

	TopFieldOrderCnt = PicOrderCntMsb + pic_order_cnt_lsb;

	if (current_frame_type != FRAME_B) {
		PicOrderCntMsb_ref = PicOrderCntMsb;
		pic_order_cnt_lsb_ref = pic_order_cnt_lsb;
	}

	return TopFieldOrderCnt;
}
int32_t YangH264EncoderIntel::render_slice(void) {
	VABufferID slice_param_buf;
	VAStatus va_status;
	int32_t i;

	update_RefPicList();

	/* one frame, one slice */
	m_slice_param.macroblock_address = 0;
	m_slice_param.num_macroblocks = m_width_mbaligned * m_height_mbaligned
			/ (16 * 16); /* Measured by MB */
	m_slice_param.slice_type =
			(current_frame_type == FRAME_IDR) ? 2 : current_frame_type;
	if (current_frame_type == FRAME_IDR) {
		if (current_frame_encoding != 0)
			++m_slice_param.idr_pic_id;
	} else if (current_frame_type == FRAME_P) {
		int32_t refpiclist0_max = m_ymd.h264_maxref & 0xffff;
		memcpy(m_slice_param.RefPicList0, RefPicList0_P,
				refpiclist0_max * sizeof(VAPictureH264));

		for (i = refpiclist0_max; i < 32; i++) {
			m_slice_param.RefPicList0[i].picture_id = VA_INVALID_SURFACE;
			m_slice_param.RefPicList0[i].flags = VA_PICTURE_H264_INVALID;
		}
	} else if (current_frame_type == FRAME_B) {
		int32_t refpiclist0_max = m_ymd.h264_maxref & 0xffff;
		int32_t refpiclist1_max = (m_ymd.h264_maxref >> 16) & 0xffff;

		memcpy(m_slice_param.RefPicList0, RefPicList0_B,
				refpiclist0_max * sizeof(VAPictureH264));
		for (i = refpiclist0_max; i < 32; i++) {
			m_slice_param.RefPicList0[i].picture_id = VA_INVALID_SURFACE;
			m_slice_param.RefPicList0[i].flags = VA_PICTURE_H264_INVALID;
		}

		memcpy(m_slice_param.RefPicList1, RefPicList1_B,
				refpiclist1_max * sizeof(VAPictureH264));
		for (i = refpiclist1_max; i < 32; i++) {
			m_slice_param.RefPicList1[i].picture_id = VA_INVALID_SURFACE;
			m_slice_param.RefPicList1[i].flags = VA_PICTURE_H264_INVALID;
		}
	}

	m_slice_param.slice_alpha_c0_offset_div2 = 0;
	m_slice_param.slice_beta_offset_div2 = 0;
	m_slice_param.direct_spatial_mv_pred_flag = 1;
	m_slice_param.pic_order_cnt_lsb = (current_frame_display
			- current_IDR_display) % m_ymd.MaxPicOrderCntLsb;

	if (m_ymd.h264_packedheader&& (config_attrib[enc_packed_header_idx].value & VA_ENC_PACKED_HEADER_SLICE))
		render_packedslice();

	va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,VAEncSliceParameterBufferType, sizeof(m_slice_param), 1,
			&m_slice_param, &slice_param_buf);
	CHECK_VASTATUS(va_status, "vaCreateBuffer");;

	va_status = yang_vaRenderPicture(m_va_dpy, m_vaContext, &slice_param_buf, 1);
	CHECK_VASTATUS(va_status, "vaRenderPicture");

	return Yang_Ok;
}

int32_t YangH264EncoderIntel::update_RefPicList(void) {
	uint32_t  current_poc = CurrentCurrPic.TopFieldOrderCnt;

	if (current_frame_type == FRAME_P) {
		memcpy(RefPicList0_P, ReferenceFrames,
				m_ymd.numShortTerm * sizeof(VAPictureH264));
		sort_one(RefPicList0_P, 0, m_ymd.numShortTerm - 1, 0, 1);
	}

	if (current_frame_type == FRAME_B) {
		memcpy(RefPicList0_B, ReferenceFrames,
				m_ymd.numShortTerm * sizeof(VAPictureH264));
		sort_two(RefPicList0_B, 0, m_ymd.numShortTerm - 1, current_poc, 0, 1, 0, 1);

		memcpy(RefPicList1_B, ReferenceFrames,
				m_ymd.numShortTerm * sizeof(VAPictureH264));
		sort_two(RefPicList1_B, 0, m_ymd.numShortTerm - 1, current_poc, 0, 0, 1, 0);
	}

	return Yang_Ok;
}

int32_t YangH264EncoderIntel::render_picture(void) {
	VABufferID m_pic_param_buf;
	VAStatus va_status;
	int32_t i = 0;

	m_pic_param.CurrPic.picture_id = src_surface[sid];
	m_pic_param.CurrPic.frame_idx = frameIdx;
	m_pic_param.CurrPic.flags = 0;
	m_pic_param.CurrPic.TopFieldOrderCnt = calc_poc(
			(current_frame_display - current_IDR_display) % m_ymd.MaxPicOrderCntLsb);
	m_pic_param.CurrPic.BottomFieldOrderCnt =
			m_pic_param.CurrPic.TopFieldOrderCnt;
	CurrentCurrPic = m_pic_param.CurrPic;

	/**if (getenv("TO_DEL")) { // set RefPicList into ReferenceFrames

		update_RefPicList(); // calc RefPicList
		memset(m_pic_param.ReferenceFrames, 0xff, 16 * sizeof(VAPictureH264)); // invalid all
		if (current_frame_type == FRAME_P) {
			m_pic_param.ReferenceFrames[0] = RefPicList0_P[0];
		} else if (current_frame_type == FRAME_B) {
			m_pic_param.ReferenceFrames[0] = RefPicList0_B[0];
			m_pic_param.ReferenceFrames[1] = RefPicList1_B[0];
		}
	} else {
		**/
		memcpy(m_pic_param.ReferenceFrames, ReferenceFrames,m_ymd.numShortTerm * sizeof(VAPictureH264));
		for (i = m_ymd.numShortTerm; i < Para_Bufs; i++) {
			m_pic_param.ReferenceFrames[i].picture_id = VA_INVALID_SURFACE;
			m_pic_param.ReferenceFrames[i].flags = VA_PICTURE_H264_INVALID;
		}
	//}

	m_pic_param.pic_fields.bits.idr_pic_flag =(current_frame_type == FRAME_IDR);
	m_pic_param.pic_fields.bits.reference_pic_flag = (current_frame_type!= FRAME_B);

	m_pic_param.frame_num = frameIdx;
	m_pic_param.coded_buf = coded_buf[sid];
	m_pic_param.last_picture = (current_frame_encoding == m_ymd.gop_size);

	//initVaPic(&m_ymd,&m_pic_param);

	va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncPictureParameterBufferType, sizeof(m_pic_param), 1,
			&m_pic_param, &m_pic_param_buf);
	CHECK_VASTATUS(va_status, "vaCreateBuffer");;

	va_status = yang_vaRenderPicture(m_va_dpy, m_vaContext, &m_pic_param_buf, 1);
	CHECK_VASTATUS(va_status, "vaRenderPicture");

	return Yang_Ok;
}
int32_t YangH264EncoderIntel::render_sequence(void) {
	VABufferID seq_param_buf, rc_param_buf, misc_param_tmpbuf, render_id[2];
	VAStatus va_status;
	VAEncMiscParameterBuffer *misc_param, *misc_param_tmp;
	VAEncMiscParameterRateControl *misc_rate_ctrl;
	//initVaSeq(&m_ymd,&m_seq_param);
	va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncSequenceParameterBufferType, sizeof(m_seq_param), 1,
			&m_seq_param, &seq_param_buf);
	CHECK_VASTATUS(va_status, "vaCreateBuffer");

	va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
			VAEncMiscParameterBufferType,
			sizeof(VAEncMiscParameterBuffer)
					+ sizeof(VAEncMiscParameterRateControl), 1, NULL,
			&rc_param_buf);
	CHECK_VASTATUS(va_status, "vaCreateBuffer");

	yang_vaMapBuffer(m_va_dpy, rc_param_buf, (void **) &misc_param);
	misc_param->type = VAEncMiscParameterTypeRateControl;
	misc_rate_ctrl = (VAEncMiscParameterRateControl *) misc_param->data;
	memset(misc_rate_ctrl, 0, sizeof(*misc_rate_ctrl));
	misc_rate_ctrl->bits_per_second = m_ymd.frame_bitrate;
	misc_rate_ctrl->target_percentage = 66;
	misc_rate_ctrl->window_size = 1000;
	misc_rate_ctrl->initial_qp = m_ymd.initial_qp;
	misc_rate_ctrl->min_qp = m_ymd.minimal_qp;
	misc_rate_ctrl->basic_unit_size = 0;
	yang_vaUnmapBuffer(m_va_dpy, rc_param_buf);

	render_id[0] = seq_param_buf;
	render_id[1] = rc_param_buf;

	va_status = yang_vaRenderPicture(m_va_dpy, m_vaContext, &render_id[0], 2);
	CHECK_VASTATUS(va_status, "vaRenderPicture");;

	if (misc_priv_type != 0) {
		va_status = yang_vaCreateBuffer(m_va_dpy, m_vaContext,
				VAEncMiscParameterBufferType, sizeof(VAEncMiscParameterBuffer),
				1, NULL, &misc_param_tmpbuf);
		CHECK_VASTATUS(va_status, "vaCreateBuffer");
		yang_vaMapBuffer(m_va_dpy, misc_param_tmpbuf, (void **) &misc_param_tmp);
		misc_param_tmp->type = (VAEncMiscParameterType) misc_priv_type;
		misc_param_tmp->data[0] = misc_priv_value;
		yang_vaUnmapBuffer(m_va_dpy, misc_param_tmpbuf);

		va_status = yang_vaRenderPicture(m_va_dpy, m_vaContext, &misc_param_tmpbuf,1);
	}

	return Yang_Ok;
}
int32_t YangH264EncoderIntel::update_ReferenceFrames(void) {
	int32_t i;
	if (current_frame_type == FRAME_B)
		return Yang_Ok;
	CurrentCurrPic.flags = VA_PICTURE_H264_SHORT_TERM_REFERENCE;
	m_ymd.numShortTerm++;
	if (m_ymd.numShortTerm > m_ymd.num_ref_frames)
		m_ymd.numShortTerm = m_ymd.num_ref_frames;
	for (i = m_ymd.numShortTerm - 1; i > 0; i--)
		ReferenceFrames[i] = ReferenceFrames[i - 1];
	ReferenceFrames[0] = CurrentCurrPic;

	if (current_frame_type != FRAME_B)
		current_frame_num++;
	if (current_frame_num > m_ymd.MaxFrameNum)
		current_frame_num = 0;

	return Yang_Ok;
}

int32_t YangH264EncoderIntel::encode(uint8_t *buf, int32_t buflen, uint8_t *dest,
		int32_t *destLen, int32_t *frametype) {
	upload_surface_yuv(src_surface[sid], m_yvp.outWidth, m_yvp.outHeight, buf,
			buf + yLen, NULL);
	encoding2display_order(current_frame_encoding, m_ymd.intra_period,
				m_ymd.intra_idr_period, m_ymd.ip_period, &current_frame_display,
				&current_frame_type);
	if (current_frame_type == FRAME_IDR) {
		m_ymd.numShortTerm = 0;
		current_frame_num = 0;
		current_IDR_display = current_frame_display;
	}

	vas = yang_vaBeginPicture(m_va_dpy, m_vaContext, src_surface[sid]);
	CHECK_VASTATUS(vas, "vaBeginPicture");

	if (current_frame_type == FRAME_IDR) {
		render_sequence();
		render_picture();
		if (m_ymd.h264_packedheader) {
			render_packedsequence();
			render_packedpicture();
		}
		*frametype = 1;
		//if (rc_mode == VA_RC_CBR)
		//    render_packedsei();
		//render_hrd();
	} else {
		//render_sequence();
		render_picture();
		*frametype = 0;
		//if (rc_mode == VA_RC_CBR)
		//    render_packedsei();
		//render_hrd();
	}
	render_slice();
	vas = yang_vaEndPicture(m_va_dpy, m_vaContext);
	CHECK_VASTATUS(vas, "vaEndPicture");;

	vas = yang_vaSyncSurface(m_va_dpy, src_surface[sid]);
	CHECK_VASTATUS(vas, "vaSyncSurface");
	VACodedBufferSegment *buf_list = NULL;

	//uint32_t  coded_size = 0;

	vas = yang_vaMapBuffer(m_va_dpy, coded_buf[sid], (void **) (&buf_list));
	CHECK_VASTATUS(vas, "vaMapBuffer");
	*destLen = 0;
	while (buf_list != NULL) {
		memcpy(dest + (*destLen), buf_list->buf, buf_list->size);//fwrite(buf_list->buf, 1, buf_list->size, coded_fp);
		(*destLen) += buf_list->size;
		buf_list = (VACodedBufferSegment *) buf_list->next;

	}
	yang_vaUnmapBuffer(m_va_dpy, coded_buf[sid]);
	update_ReferenceFrames();

	printf("%d-%d,",current_frame_type, *destLen);
	if(current_frame_encoding%15==0) printf("\n");

	current_frame_encoding++;
	if(current_frame_encoding==m_ymd.gop_size) current_frame_encoding=0;
	sid++;
	if (sid == Para_Bufs)
		sid = 0;
	return 1;

}
/**
void YangH264EncoderIntel::startLoop() {
	isConvert = 1;
	int32_t isTrans = (m_inWidth != m_outWidth ? 1 : 0);
	int64_t videoTimestamp = 0;
	int32_t videoDestLen = 0;
	int32_t frameType = 0;
	uint8_t *videoDest = new uint8_t[1024 * 256];

	int32_t m_in_File_Size =
			m_context->videoCaptureFormat>0 ?
					m_inWidth * m_inHeight * 3 / 2 : m_inWidth * m_inHeight * 2;
	//int32_t m_in_File_Size1 = m_inWidth * m_inHeight * 3 / 2;
	int32_t m_out_File_Size = m_outWidth * m_outHeight * 3 / 2;
	uint8_t *videoSrc = new uint8_t[m_inWidth * m_inHeight * 3 / 2];
	uint8_t videoSrc1[m_out_File_Size];	//=new uint8_t[m_in_File_Size1];
	int64_t preVideostamp = 0;
	printf("\n**************************start Zbhd encoding...........");
	YangConvert con;
	//if(isTrans) con.initNv12_resize(inWidth,inHeight,outWidth,outHeight);
	//uint8_t *t_in;
	//uint8_t *t_out;

	if (m_in_videoBuffer != NULL)		m_in_videoBuffer->resetIndex();
	if (m_out_videoBuffer != NULL)		m_out_videoBuffer->resetIndex();
	int32_t is16bit=1;
		if(m_context->videoCaptureFormat>0) is16bit=0;
	while (isConvert == 1) {

		if (m_in_videoBuffer->size() == 0) {
			//printf("***listsize=%d\n",vlist->getSize());
			yang_usleep(20000);
			//printf("**");
			continue;
		}
		//if (isTrans) {
		//	m_in_videoBuffer->getVideo(t_in, m_in_File_Size, &videoTimestamp);
			//util1.getVideoZ(videoSrc,m_in_File_Size,&videoTimestamp);
		//	if (preVideostamp == 0) {
		//		preVideostamp = videoTimestamp;
		//	}
		//	if (videoTimestamp - preVideostamp <= 0)
		//		continue;

			//con.YUY2toNV12(inWidth,inHeight,videoSrc,videoSrc1);
			//con.nv12_nearest_scale(videoSrc1,p264Pic->img.plane[0]);
		//	con.mpu->pu->resize();
		//	memcpy(videoSrc1, t_out, m_out_File_Size);
			//con.YUY2toI420(outWidth,outHeight,con.mpu->pu->out,p264Pic->img.plane[0]);

			//con.resize_NV12(videoSrc1,p264Pic->img.plane[0],inWidth,inHeight,outWidth,outHeight);
			//con.resize_Yuy2_NV12(videoSrc,p264Pic->img.plane[0],inWidth,inHeight,outWidth,outHeight);
			//memcpy(p264Pic->img.plane[0],t_out,m_out_File_Size);
	//	} else {
			//	util1.getVideoZ(p264Pic->img.plane[0],m_in_File_Size,&videoTimestamp);
			//util1.YUY2toI420(inWidth, inHeight,	util1.getVideoIn(&videoTimestamp), p264Pic->img.plane[0]);

		if(is16bit){
				if (isTrans)
					con.resize_Yuy2_NV12(m_in_videoBuffer->getVideoIn(&videoTimestamp),videoSrc1,m_inWidth,m_inHeight,m_outWidth,m_outHeight);
				else
					con.YUY2toNV12(m_inWidth,m_inHeight,m_in_videoBuffer->getVideoIn(&videoTimestamp),	videoSrc1);
			}else{
				if(isTrans)
					con.resize_NV12(m_in_videoBuffer->getVideoIn(&videoTimestamp),videoSrc1,m_inWidth,m_inHeight,m_outWidth,m_outHeight);
				else
					memcpy(videoSrc1,m_in_videoBuffer->getVideoIn(&videoTimestamp),m_in_File_Size);
			}
			if (preVideostamp == 0) {
				preVideostamp = videoTimestamp;
			}
			if (videoTimestamp - preVideostamp <= 0)
				continue;
		//}

		encode(videoSrc1, m_out_File_Size, videoDest, &videoDestLen,
				&frameType);
		if (videoDestLen > 4) {
			//writeFile(videoDest,videoDestLen);
			m_out_videoBuffer->putEVideo(videoDest, videoDestLen, videoTimestamp,
					frameType);
		}
		//printf("%d-%d,",frameType, videoDestLen);

	}
	//util1.close();
	//util_z.close();
	//t_in = NULL;
	//t_out = NULL;
	//con.mpu->pu->close();
	//con.mpu->pu = NULL;
	//util_z.close();
	//	delete[] srcVideoSource;
	yang_deleteA(videoSrc);
	yang_deleteA(videoDest);

}

void YangH264EncoderIntel::stopLoop() {
	isConvert = 0;

}
**/
#endif
#endif
