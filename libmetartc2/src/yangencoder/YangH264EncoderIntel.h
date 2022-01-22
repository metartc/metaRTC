#ifndef YangH264EncoderIntel_H
#define YangH264EncoderIntel_H

#ifndef _WIN32
#include "yangencoder/YangVideoEncoder.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include <yangutil/yangavinfotype.h>
#if YangLibva
#include "va/va.h"
#include "va/va_enc_h264.h"
#include "va/va_drm.h"
#include "va/va_drmcommon.h"

#include "yangutil/sys/YangLoadLib.h"
#include "YangH264EncHeader.h"
#define Para_Bufs 16

struct bitstream {
	uint32_t  *buffer;
	int32_t bit_offset;
	int32_t max_size_in_dword;
};


class YangH264EncoderIntel: public YangVideoEncoder {
public:
	YangH264EncoderIntel();
	 ~YangH264EncoderIntel();
	//void init(YangIni *pcontext);
	void init(YangVideoParam *pvp,YangVideoEncParam *penc);

	int32_t encode(uint8_t *buf, int32_t buflen, uint8_t *dest, int32_t *destLen,int32_t *frametype);
	void setVideoMetaData(YangVideoMeta *pvmd);
    void parseRtmpHeader(uint8_t *p,int32_t pLen,int32_t *pwid,int32_t *phei,int32_t *pfps);

protected:
	//void stopLoop();
	//void startLoop();
private:

	YangVideoMeta *m_vmd;
	YangMeataData m_ymd;
	int32_t m_fd;
	VADisplay m_va_dpy;
	VAConfigID m_config_id;
	VAContextID m_vaContext;

	VAStatus vas;

	VAImage m_img;
	VAImageFormat image_format;

	// YangH264Header h264header;
	VAPictureH264 m_pic[16];
	//int32_t m_ref_count;
	//int32_t m_ref_count_m1;

	VAPictureH264 CurrentCurrPic;
	VAEncPictureParameterBufferH264 m_pic_param;
	VAEncSliceParameterBufferH264 m_slice_param;
	VAEncSequenceParameterBufferH264 m_seq_param;
	VABufferID bufids[10];

	//YangDecodePicture pics[Para_Bufs];
	VASurfaceID src_surface[Para_Bufs];
	//VASurfaceID ref_surface[Para_Bufs];
	VABufferID coded_buf[Para_Bufs];
	int32_t sid;

private:
	//int32_t isConvert;
	//int32_t m_inWidth;
	//int32_t m_inHeight;
	int32_t m_width_mbaligned;
	int32_t m_height_mbaligned;

	int32_t m_frame;
	int32_t yLen;
	int32_t uLen;
	int32_t uvLen;
	int32_t allLen;
	int32_t frameIdx;
	int32_t FieldOrderCnt;
	int32_t posNum;
	int32_t m_vap_size;

	int32_t current_frame_type;
	VAConfigAttrib attrib[VAConfigAttribTypeMax];
	VAConfigAttrib config_attrib[VAConfigAttribTypeMax];
	unsigned int64_t current_frame_encoding;
	unsigned int64_t current_frame_display;
	unsigned int64_t current_IDR_display;
	uint32_t  current_frame_num;
	int32_t misc_priv_type;
	int32_t misc_priv_value;
	VAProfile h264_profile;
	// int32_t current_frame_type;
	int32_t config_attrib_num, enc_packed_header_idx;

//#define current_slot (current_frame_display % Para_Bufs)
	VAPictureH264 ReferenceFrames[16], RefPicList0_P[32], RefPicList0_B[32],RefPicList1_B[32];
	void initSlicePara(VASliceParameterBufferH264 *slice264,uint8_t *p_data, int32_t p_len);
	int32_t render_packedpicture(void);
	int32_t render_packedsequence(void);
	int32_t render_slice(void);
	int32_t render_picture(void);
	int32_t render_sequence(void);
	int32_t update_RefPicList(void);
	void slice_header(bitstream *bs);
	void pps_rbsp(bitstream *bs);
	void sps_rbsp(bitstream *bs);
	int32_t calc_poc(int32_t pic_order_cnt_lsb);
	void render_packedslice();
	int32_t build_packed_slice_buffer(uint8_t **header_buffer);
	int32_t build_packed_seq_buffer(uint8_t **header_buffer);
	int32_t build_packed_pic_buffer(uint8_t **header_buffer);
	int32_t update_ReferenceFrames(void);
	int32_t upload_surface_yuv(VASurfaceID surface_id, int32_t src_width,int32_t src_height, uint8_t *src_Y, uint8_t *src_U,uint8_t *src_V);
	int32_t upload_surface_yuv(VADisplay va_dpy, VASurfaceID surface_id, int32_t src_fourcc,	int32_t src_width, int32_t src_height, uint8_t *src_Y,
			uint8_t *src_U, uint8_t *src_V);
private:

	void closeDevice();
	// int32_t build_packed_seq_buffer(uint8_t **header_buffer)
	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();

	VAStatus (*yang_vaGetConfigAttributes) (VADisplay dpy, VAProfile profile, VAEntrypoint32_t entrypoint, VAConfigAttrib *attrib_list, int32_t num_attribs);
	VAStatus (*yang_vaDeriveImage) (VADisplay dpy,VASurfaceID surface,VAImage *image);
	VAStatus (*yang_vaDestroyImage) (VADisplay dpy,VAImageID image);
    VADisplay (*yang_vaGetDisplayDRM)(int32_t fd);
    VAStatus (*yang_vaInitialize) ( VADisplay dpy, int32_t *major_version,int32_t *minor_version);
    VAStatus (*yang_vaDestroyContext) (VADisplay dpy,VAContextID context);
    VAStatus (*yang_vaDestroySurfaces) (VADisplay dpy,VASurfaceID *surfaces,int32_t num_surfaces);
    VAStatus (*yang_vaDestroyConfig) (VADisplay dpy,VAConfigID config_id);
    VAStatus (*yang_vaTerminate) ( VADisplay dpy);


    VAStatus (*yang_vaCreateConfig) (VADisplay dpy, VAProfile profile, VAEntrypoint32_t entrypoint, VAConfigAttrib *attrib_list, int32_t num_attribs, VAConfigID *config_id );

    VAStatus (*yang_vaCreateSurfaces)(VADisplay  dpy,uint32_t   format,uint32_t    width,
        uint32_t         height,VASurfaceID  *surfaces,uint32_t   num_surfaces,  VASurfaceAttrib    *attrib_list,
        uint32_t         num_attribs);

    VAStatus (*yang_vaCreateContext) (VADisplay dpy,VAConfigID config_id, int32_t picture_width,
        int32_t picture_height, int32_t flag, VASurfaceID *render_targets,
        int32_t num_render_targets,VAContextID *context);
    VAStatus (*yang_vaCreateBuffer) (VADisplay dpy,VAContextID context, VABufferType type,
        uint32_t  size, uint32_t  num_elements, void *data, VABufferID *buf_id
    );
    VAStatus (*yang_vaBeginPicture) (VADisplay dpy,VAContextID context,VASurfaceID render_target);
    VAStatus (*yang_vaRenderPicture) (VADisplay dpy,VAContextID context, VABufferID *buffers,int32_t num_buffers);

    VAStatus (*yang_vaCreateImage) (VADisplay dpy,VAImageFormat *format, int32_t width, int32_t height, VAImage *image);
    VAStatus (*yang_vaEndPicture) (VADisplay dpy,VAContextID context);
    VAStatus (*yang_vaGetImage) (VADisplay dpy,VASurfaceID surface, int32_t x,
        int32_t y,uint32_t  width, uint32_t  height,VAImageID image);
    VAStatus (*yang_vaMapBuffer) (VADisplay dpy,VABufferID buf_id, void **pbuf);
    VAStatus (*yang_vaSyncSurface) (VADisplay dpy,VASurfaceID render_target);
    VAStatus (*yang_vaUnmapBuffer) (VADisplay dpy,VABufferID buf_id);
};
#endif
#endif
#endif // YANGH264DECODER_H
