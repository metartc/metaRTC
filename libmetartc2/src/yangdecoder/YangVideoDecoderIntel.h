#ifndef YangVideoDecoderIntel_H
#define YangVideoDecoderIntel_H
//#include "../decoder/YangAudioDecoderHandle.h"
#include "Yang_Config.h"
#if HaveLibva
#include "yangdecoder/YangVideoDecoder.h"
#include "yangutil/yangavtype.h"
//#include "YangInitVaPicPara.h"
#include "va/va.h"
#include "va/va_drm.h"
#include "va/va_drmcommon.h"

//#include "../yangplayer/YangVideoPlay.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/sys/YangLoadLib.h"
#include "YangH264Header.h"
//#include <va/va_x11.h>
//#include "YangVaDevice.h"

//}

//#define MAX_PARAM_BUFFERS 16
#define Para_Bufs 7
typedef struct YangDecodePicture {
	   	VABufferID picBuf;
	    VABufferID IQMBuf;
	    VABufferID sliceParaBuf;
	    VABufferID sliceDataBuf;

} YangDecodePicture;


class YangVideoDecoderIntel:public YangVideoDecoder
{
public:
    YangVideoDecoderIntel();
     ~YangVideoDecoderIntel();
    void init();
    void init(int32_t iW,int32_t iH);
    void init(YangVideoParam *p_config);
    int32_t decode(YangVideoBuffer *vpl,int32_t isIframe,uint8_t *pData, int32_t nSize, int64_t ptimestamp,uint8_t *dest, int32_t *pnFrameReturned);
    void initWH();
    void parseRtmpHeader(uint8_t *p, int32_t pLen,int32_t *pwid,int32_t *phei,int32_t *pfps);

protected:
   void closeDevice();
private:
   int32_t m_fd;
   VADisplay m_va_dpy;
   VAConfigID m_config_id;
   VAContextID m_vaContext;

   VAStatus vas;

   VAImage m_img;
   VAImageFormat image_format;

   YangH264Header h264header;
    VAPictureH264 m_pic[16];
    int32_t m_ref_count;
    int32_t m_ref_count_m1;

    //VAPictureH264 m_old_pic;
    VAPictureParameterBufferH264 m_picPara;

    VABufferID            bufids[10];
    VAIQMatrixBufferH264 iq_matrix;
    VASliceParameterBufferH264 m_slice_param;

    YangDecodePicture pics[Para_Bufs];
    VASurfaceID surface_id[Para_Bufs];
    int32_t sid;

private:
    void cachePic(VAPictureH264 *p_pic,int32_t p_pocind);
    void initH264Pic();
    void initH264Slice();
    int32_t m_width;
    int32_t m_height;
    int32_t m_frame;
    int32_t yLen;
    int32_t uLen;
    int32_t uvLen;
    int32_t allLen;
    int32_t frameIdx;
    int32_t FieldOrderCnt;
    int32_t posNum;
    void initSlicePara(VASliceParameterBufferH264 *slice264,uint8_t *p_data,int32_t p_len);
	YangLoadLib m_lib,m_lib1;
	void loadLib();
	void unloadLib();
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
#endif // YANGH264DECODER_H
