//
// Copyright (c) 2019-2022 JerryQiu
//

#include <yangipc_rk/YangRkCommon.h>

#include <yangutil/sys/YangLog.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#define ENABLE_AEC_ANR

#define WEBRTC_V_DEVICE "rkispp_scale0"
#define LOCAL_V_DEVICE  "rkispp_m_bypass"
#define A_DEVICE        "default:CARD=card 0"

RK_CHAR *pIqfilesPath ;

//video
CODEC_TYPE_E enCodecType = RK_CODEC_TYPE_H264;
RK_S32 s32CamId = 0;
RK_U32 u32BufCnt = 4;
RK_U32 u32Width = 1920;
RK_U32 u32Height = 1080;
int mWebrtcVInChn = 2;
int mWebrtcVEncChn = 2;

static RK_U32 u32WebrtcWidth = 1280;
static RK_U32 u32WebrtcHeight = 720;

int video_get_vencChan(void)
{
    return mWebrtcVEncChn;
}

CODEC_TYPE_E video_get_enc_type(void)
{
    return enCodecType;
}

int video_init(char *resolution)
{
     int ret = 1;
    RK_MPI_SYS_Init();

    printf("video_init start\n");
    if (strcmp(resolution,"720") == 0) {
         u32WebrtcWidth = 1280;
         u32WebrtcHeight = 720;
     } else if (strcmp(resolution,"480") == 0) {
         u32WebrtcWidth = 960;
         u32WebrtcHeight = 540;
     } else if (strcmp(resolution,"360") == 0) {
         u32WebrtcWidth = 640;
         u32WebrtcHeight = 360;
     } else {
         u32WebrtcWidth = 1920;
         u32WebrtcHeight = 1080;
     }

    printf(" resolution:%d x %d\n",u32WebrtcWidth,u32WebrtcHeight);

    MPP_CHN_S stWebrtcSrcChn;
    stWebrtcSrcChn.enModId = RK_ID_VI;
    stWebrtcSrcChn.s32DevId = s32CamId;
    stWebrtcSrcChn.s32ChnId = mWebrtcVInChn;

    MPP_CHN_S stEncChn;
    stEncChn.enModId = RK_ID_VENC;
    stEncChn.s32DevId = s32CamId;
    stEncChn.s32ChnId = mWebrtcVEncChn;

    VI_CHN_ATTR_S vi_chn_attr;
    vi_chn_attr.pcVideoNode = "rkispp_scale0";
    vi_chn_attr.u32BufCnt = u32BufCnt;
    vi_chn_attr.u32Width = u32WebrtcWidth;
    vi_chn_attr.u32Height = u32WebrtcHeight;
    vi_chn_attr.enPixFmt = IMAGE_TYPE_NV12;
    // vi_chn_attr.enBufType = VI_CHN_BUF_TYPE_MMAP;
    vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    ret = RK_MPI_VI_SetChnAttr(stWebrtcSrcChn.s32DevId, stWebrtcSrcChn.s32ChnId, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(stWebrtcSrcChn.s32DevId, stWebrtcSrcChn.s32ChnId);
    VENC_CHN_ATTR_S venc_chn_attr;
    memset(&venc_chn_attr, 0, sizeof(venc_chn_attr));
    switch (enCodecType) {
        case RK_CODEC_TYPE_H265:
        venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H265;
        venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
        venc_chn_attr.stRcAttr.stH265Cbr.u32Gop = 25;
        venc_chn_attr.stRcAttr.stH265Cbr.u32BitRate = u32WebrtcWidth * u32WebrtcHeight * 3 / 4;
        // frame rate: in 30/1, out 30/1.
        venc_chn_attr.stRcAttr.stH265Cbr.fr32DstFrameRateDen = 1;
        venc_chn_attr.stRcAttr.stH265Cbr.fr32DstFrameRateNum = 25;
        venc_chn_attr.stRcAttr.stH265Cbr.u32SrcFrameRateDen = 1;
        venc_chn_attr.stRcAttr.stH265Cbr.u32SrcFrameRateNum = 25;
        break;
        case RK_CODEC_TYPE_H264:
        default:
        venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
        venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
        venc_chn_attr.stRcAttr.stH264Cbr.u32Gop = 25;
        venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = u32WebrtcWidth * u32WebrtcHeight;
        // frame rate: in 30/1, out 30/1.
        venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;
        venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = 25;
        venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;
        venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = 25;
        break;
    }
    venc_chn_attr.stVencAttr.imageType = IMAGE_TYPE_NV12;
    venc_chn_attr.stVencAttr.u32PicWidth = u32WebrtcWidth;
    venc_chn_attr.stVencAttr.u32PicHeight = u32WebrtcHeight;
    venc_chn_attr.stVencAttr.u32VirWidth = u32WebrtcWidth;
    venc_chn_attr.stVencAttr.u32VirHeight = u32WebrtcHeight;
    // Only supprot: 66:Baseline, 77:Main Profile, 100: High Profile
    venc_chn_attr.stVencAttr.u32Profile = 66; // metartc目前h264支持Baseline

    RK_MPI_VENC_CreateChn(stEncChn.s32ChnId, &venc_chn_attr);
    RK_MPI_SYS_SetMediaBufferDepth(RK_ID_VENC, stEncChn.s32ChnId, 6);
    RK_MPI_VENC_RGN_Init(stEncChn.s32ChnId, NULL);
    ret = RK_MPI_SYS_Bind(&stWebrtcSrcChn, &stEncChn);

    printf("video_init End\n");
    return ret;
}

int video_stop(void)
{
    int ret=0;

    printf("video_stop start");

    MPP_CHN_S stWebrtcSrcChn;
    stWebrtcSrcChn.enModId = RK_ID_VI;
    stWebrtcSrcChn.s32DevId = s32CamId;
    stWebrtcSrcChn.s32ChnId = mWebrtcVInChn;

    MPP_CHN_S stEncChn;
    stEncChn.enModId = RK_ID_VENC;
    stEncChn.s32DevId = s32CamId;
    stEncChn.s32ChnId = mWebrtcVEncChn;

    ret = RK_MPI_SYS_UnBind(&stWebrtcSrcChn, &stEncChn);

    // destroy venc before vi
     ret = RK_MPI_VENC_DestroyChn((stEncChn.s32ChnId));

    // destroy vi
    ret = RK_MPI_VI_DisableChn(s32CamId, stWebrtcSrcChn.s32ChnId);

    printf("deinit");
exit:
    printf("video_stop end");
    return ret;
}

