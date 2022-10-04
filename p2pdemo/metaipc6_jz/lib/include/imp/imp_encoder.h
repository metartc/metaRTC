/*
 * IMP Encoder func header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_ENCODER_H__
#define __IMP_ENCODER_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP视频编码头文件
 */

/**
 * @defgroup IMP_Encoder
 * @ingroup imp
 * @brief 视频编码（JPEG, H264, H265）模块，包含编码通道管理，编码参数设置等功能
 * @section enc_struct 1 模块结构
 * Encoder模块内部结构如下如：
 * @image html encoder_struct.jpg
 * 如上图所示，编码模块由若干个Group组成（在T15上支持两个Group），每个Group由编码Channel组成。
 * 每个编码Channel附带一个输出码流缓冲区，这个缓冲区由多个buffer组成。
 * @section enc_channel 2 编码Channel
 * 一个编码Channel可以完成一种协议的编码, 每个Group可以添加2个编码channel。
 * @section enc_rc 3 码率控制
 * @subsection enc_cbr 3.1 CBR
 * CBR（Constent Bit Rate）恒定比特率，即在码率统计时间内编码码率恒定。
 * 以H.264 编码为例，用户可设置maxQp，minQp，bitrate等。
 * maxQp，minQp 用于控制图像的质量范围， bitrate 用于钳位码率统计时间内的平均编码码率。
 * 当编码码率大于恒定码率时，图像QP 会逐步向maxQp 调整，当编码码率远小于恒定码率时，图像QP 会逐步向minQp 调整。
 * 当图像QP 达到maxQp 时，QP 被钳位到最大值，bitrate 的钳位效果失效，编码码率有可能会超出bitrate。
 * 当图像QP 达到minQp 时，QP 被钳位到最小值，此时编码的码率已经达到最大值，而且图像质量最好。
 * @subsection enc_FixQP 3.2 FixQP
 * Fix Qp 固定Qp 值。在码率统计时间内，编码图像所有宏块Qp 值相同，采用用户设定的图像Qp值。
 * @{
 */

/**
 * 定义H.264码流NALU类型
 */
typedef enum {
	IMP_H264_NAL_UNKNOWN		= 0,	/**< 未指定 */
	IMP_H264_NAL_SLICE		    = 1,	/**< 一个非IDR图像的编码条带  */
	IMP_H264_NAL_SLICE_DPA	    = 2,	/**< 编码条带数据分割块A */
	IMP_H264_NAL_SLICE_DPB	    = 3,	/**< 编码条带数据分割块B */
	IMP_H264_NAL_SLICE_DPC	    = 4,	/**< 编码条带数据分割块C */
	IMP_H264_NAL_SLICE_IDR	    = 5,	/**< IDR图像的编码条带 */
	IMP_H264_NAL_SEI			= 6,	/**< 辅助增强信息 (SEI) */
	IMP_H264_NAL_SPS			= 7,	/**< 序列参数集 */
	IMP_H264_NAL_PPS			= 8,	/**< 图像参数集 */
	IMP_H264_NAL_AUD			= 9,	/**< 访问单元分隔符 */
	IMP_H264_NAL_FILLER		    = 12,	/**< 填充数据 */
} IMPEncoderH264NaluType;

/**
 * 定义H.265码流NALU类型
 */
typedef enum {
    IMP_H265_NAL_SLICE_TRAIL_N      = 0,        /**< 尾随图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_TRAIL_R      = 1,        /**< 尾随图像, 带参考信息 */
    IMP_H265_NAL_SLICE_TSA_N        = 2,        /**< 时域子层接入点图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_TSA_R        = 3,        /**< 时域子层接入点图像, 带参考信息 */
    IMP_H265_NAL_SLICE_STSA_N       = 4,        /**< 逐步时域子层接入点图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_STSA_R       = 5,        /**< 逐步时域子层接入点图像, 带参考信息 */
    IMP_H265_NAL_SLICE_RADL_N       = 6,        /**< 可解码随机接入前置图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_RADL_R       = 7,        /**< 可解码随机接入前置图像, 带参考信息 */
    IMP_H265_NAL_SLICE_RASL_N       = 8,        /**< 跳过随机接入的前置图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_RASL_R       = 9,        /**< 跳过随机接入的前置图像, 带参考信息 */
    IMP_H265_NAL_SLICE_BLA_W_LP     = 16,       /**< 断点连接接入, 带前置图像 */
    IMP_H265_NAL_SLICE_BLA_W_RADL   = 17,       /**< 断点连接接入, 带前置图像RADL */
    IMP_H265_NAL_SLICE_BLA_N_LP     = 18,       /**< 断点连接接入, 不带前置图像 */
    IMP_H265_NAL_SLICE_IDR_W_RADL   = 19,       /**< 即时解码刷新, 带前置图像RADL */
    IMP_H265_NAL_SLICE_IDR_N_LP     = 20,       /**< 即时解码刷新, 不带前置图像 */
    IMP_H265_NAL_SLICE_CRA          = 21,       /**< 纯随机接入, 带前置图像*/
    IMP_H265_NAL_VPS                = 32,       /**< 视频参数集 */
    IMP_H265_NAL_SPS                = 33,       /**< 序列参数集 */
    IMP_H265_NAL_PPS                = 34,       /**< 图像参数集 */
    IMP_H265_NAL_AUD                = 35,       /**< 访问单元分隔符 */
    IMP_H265_NAL_EOS                = 36,       /**< 序列结束 */
    IMP_H265_NAL_EOB                = 37,       /**< 比特流结束 */
    IMP_H265_NAL_FILLER_DATA        = 38,       /**< 填充数据 */
    IMP_H265_NAL_PREFIX_SEI         = 39,       /**< 辅助增强信息 (SEI) */
    IMP_H265_NAL_SUFFIX_SEI         = 40,       /**< 辅助增强信息 (SEI) */
    IMP_H265_NAL_INVALID            = 64,       /**< 无效NAL类型 */
} IMPEncoderH265NaluType;

/**
 * 定义H.264和H.265编码Channel码流NAL类型
 */
typedef union {
	IMPEncoderH264NaluType		h264NalType;		/**< H264E NALU 码流包类型 */
	IMPEncoderH265NaluType		h265NalType;		/**< H265E NALU 码流包类型 */
} IMPEncoderNalType;

typedef enum {
  IMP_ENC_SLICE_SI = 4,         /**< AVC SI Slice */
  IMP_ENC_SLICE_SP = 3,         /**< AVC SP Slice */
  IMP_ENC_SLICE_GOLDEN = 3,     /**< Golden Slice */
  IMP_ENC_SLICE_I = 2,          /**< I Slice (can contain I blocks) */
  IMP_ENC_SLICE_P = 1,          /**< P Slice (can contain I and P blocks) */
  IMP_ENC_SLICE_B = 0,          /**< B Slice (can contain I, P and B blocks) */
  IMP_ENC_SLICE_CONCEAL = 6,    /**< Conceal Slice (slice was concealed) */
  IMP_ENC_SLICE_SKIP = 7,       /**< Skip Slice */
  IMP_ENC_SLICE_REPEAT = 8,     /**< Repeat Slice (repeats the content of its reference) */
  IMP_ENC_SLICE_MAX_ENUM,       /**< sentinel */
} IMPEncoderSliceType;

/**
 * 定义编码帧码流包结构体
 */
typedef struct {
	uint32_t	offset;						/**< 码流包地址偏移 */
	uint32_t	length;							/**< 码流包长度 */
	int64_t	timestamp;						/**< 时间戳，单位us */
	bool	frameEnd;						/**< 帧结束标识 */
	IMPEncoderNalType   nalType;				/**< H.264和H.265编码Channel码流NAL类型 */
	IMPEncoderSliceType sliceType;
} IMPEncoderPack;

/**
 * 定义编码帧码流类型结构体
 */
typedef struct {
	uint32_t		  phyAddr;          /**< 帧码流物理地址 */
	uint32_t		  virAddr;          /**< 帧码流包虚拟地址 */
	uint32_t		  streamSize;       /**< virAddr对应分配的地址空间大小 */
	IMPEncoderPack  *pack;				/**< 帧码流包结构 */
	uint32_t        packCount;			/**< 一帧码流的所有包的个数 */
	uint32_t        seq;				/**< 编码帧码流序列号 */
} IMPEncoderStream;

typedef enum {
  IMP_ENC_TYPE_AVC      = 0,
  IMP_ENC_TYPE_HEVC     = 1,
  IMP_ENC_TYPE_JPEG     = 4,
} IMPEncoderEncType;

#define IMP_ENC_AVC_PROFILE_IDC_BASELINE      66
#define IMP_ENC_AVC_PROFILE_IDC_MAIN          77
#define IMP_ENC_AVC_PROFILE_IDC_HIGH          100
#define IMP_ENC_HEVC_PROFILE_IDC_MAIN         1

typedef enum {
  IMP_ENC_PROFILE_AVC_BASELINE  = ((IMP_ENC_TYPE_AVC << 24) | (IMP_ENC_AVC_PROFILE_IDC_BASELINE)),
  IMP_ENC_PROFILE_AVC_MAIN      = ((IMP_ENC_TYPE_AVC << 24) | (IMP_ENC_AVC_PROFILE_IDC_MAIN)),
  IMP_ENC_PROFILE_AVC_HIGH      = ((IMP_ENC_TYPE_AVC << 24) | (IMP_ENC_AVC_PROFILE_IDC_HIGH)),
  IMP_ENC_PROFILE_HEVC_MAIN     = ((IMP_ENC_TYPE_HEVC << 24) | (IMP_ENC_HEVC_PROFILE_IDC_MAIN)),
  IMP_ENC_PROFILE_JPEG          = (IMP_ENC_TYPE_JPEG << 24),
} IMPEncoderProfile;

typedef enum {
  IMP_ENC_PIC_FORMAT_400_8BITS = 0x0088,
  IMP_ENC_PIC_FORMAT_420_8BITS = 0x0188,
  IMP_ENC_PIC_FORMAT_422_8BITS = 0x0288,
} IMPEncoderPicFormat;

typedef enum {
  IMP_ENC_OPT_QP_TAB_RELATIVE   = 0x00000001,
  IMP_ENC_OPT_FIX_PREDICTOR     = 0x00000002,
  IMP_ENC_OPT_CUSTOM_LDA        = 0x00000004,
  IMP_ENC_OPT_ENABLE_AUTO_QP    = 0x00000008,
  IMP_ENC_OPT_ADAPT_AUTO_QP     = 0x00000010,
  IMP_ENC_OPT_COMPRESS          = 0x00000020,
  IMP_ENC_OPT_FORCE_REC         = 0x00000040,
  IMP_ENC_OPT_FORCE_MV_OUT      = 0x00000080,
  IMP_ENC_OPT_HIGH_FREQ         = 0x00002000,
  IMP_ENC_OPT_SRD               = 0x00008000,
  IMP_ENC_OPT_FORCE_MV_CLIP     = 0x00020000,
  IMP_ENC_OPT_RDO_COST_MODE     = 0x00040000,
} IMPEncoderEncOptions;

typedef enum {
  IMP_ENC_TOOL_WPP              = 0x00000001,
  IMP_ENC_TOOL_TILE             = 0x00000002,
  IMP_ENC_TOOL_LF               = 0x00000004,
  IMP_ENC_TOOL_LF_X_SLICE       = 0x00000008,
  IMP_ENC_TOOL_LF_X_TILE        = 0x00000010,
  IMP_ENC_TOOL_SCL_LST          = 0x00000020,
  IMP_ENC_TOOL_CONST_INTRA_PRED = 0x00000040,
  IMP_ENC_TOOL_TRANSFO_SKIP     = 0x00000080,
  IMP_ENC_TOOL_PCM              = 0x00000800,
} IMPEncoderEncTools;

/**
 * 定义编码器裁剪属性，针对输入编码器的图像先做裁剪，与编码通道的尺寸进行比较再做缩放
 */
typedef struct {
    bool		enable;		/**< 是否进行裁剪,取值范围:[FALSE, TRUE],TRUE:使能裁剪,FALSE:不使能裁剪 */
    uint32_t	x;			/**< 裁剪的区域,左上角x坐标 */
    uint32_t	y;			/**< 裁剪的区域,左上角y坐标 */
    uint32_t	w;			/**< 裁剪的区域,宽 */
    uint32_t	h;			/**< 裁剪的区域,高 */
} IMPEncoderCropCfg;

/**
 * 定义编码器属性结构体
 */
typedef struct {
  IMPEncoderProfile       eProfile;
  uint8_t                 uLevel;
  uint8_t                 uTier;
  uint16_t                uWidth;
  uint16_t                uHeight;
  IMPEncoderPicFormat     ePicFormat;
  uint32_t                eEncOptions;
  uint32_t                eEncTools;
  IMPEncoderCropCfg       crop;           /**< 编码器裁剪属性 */
} IMPEncoderEncAttr;

typedef enum {
  IMP_ENC_GOP_CTRL_MODE_DEFAULT       = 0x02,
  IMP_ENC_GOP_CTRL_MODE_PYRAMIDAL     = 0x04,
  IMP_ENC_GOP_CTRL_MAX_ENUM           = 0xff,
} IMPEncoderGopCtrlMode;

typedef struct {
  IMPEncoderGopCtrlMode   uGopCtrlMode;
  uint16_t                uGopLength;
  uint8_t                 uNumB;
  uint32_t                uMaxSameSenceCnt;
  bool                    bEnableLT;
  uint32_t                uFreqLT;
  bool                    bLTRC;
} IMPEncoderGopAttr;

typedef enum {
  IMP_ENC_RC_MODE_FIXQP             = 0x0,
  IMP_ENC_RC_MODE_CBR               = 0x1,
  IMP_ENC_RC_MODE_VBR               = 0x2,
  IMP_ENC_RC_MODE_CAPPED_VBR        = 0x4,
  IMP_ENC_RC_MODE_CAPPED_QUALITY    = 0x8,
  IMP_ENC_RC_MODE_INVALID           = 0xff,
} IMPEncoderRcMode;

typedef enum IMPEncoderRcOptions {
  IMP_ENC_RC_OPT_NONE           = 0x00000000,
  IMP_ENC_RC_SCN_CHG_RES        = 0x00000001,
  IMP_ENC_RC_DELAYED            = 0x00000002,
  IMP_ENC_RC_STATIC_SCENE       = 0x00000004,
  IMP_ENC_RC_ENABLE_SKIP        = 0x00000008,
  IMP_ENC_RC_OPT_SC_PREVENTION  = 0x00000010,
  IMP_ENC_RC_MAX_ENUM,
} IMPEncoderRcOptions;

typedef struct {
  int16_t       iInitialQP;
} IMPEncoderAttrFixQP;

typedef struct {
  uint32_t              uTargetBitRate;
  int16_t               iInitialQP;
  int16_t               iMinQP;
  int16_t               iMaxQP;
  int16_t               iIPDelta;
  int16_t               iPBDelta;
  uint32_t              eRcOptions;
  uint32_t              uMaxPictureSize;
} IMPEncoderAttrCbr;

typedef struct {
  uint32_t              uTargetBitRate;
  uint32_t              uMaxBitRate;
  int16_t               iInitialQP;
  int16_t               iMinQP;
  int16_t               iMaxQP;
  int16_t               iIPDelta;
  int16_t               iPBDelta;
  uint32_t              eRcOptions;
  uint32_t              uMaxPictureSize;
} IMPEncoderAttrVbr;

typedef struct {
  uint32_t              uTargetBitRate;
  uint32_t              uMaxBitRate;
  int16_t               iInitialQP;
  int16_t               iMinQP;
  int16_t               iMaxQP;
  int16_t               iIPDelta;
  int16_t               iPBDelta;
  uint32_t              eRcOptions;
  uint32_t              uMaxPictureSize;
  uint16_t              uMaxPSNR;
} IMPEncoderAttrCappedVbr;

typedef IMPEncoderAttrCappedVbr IMPEncoderAttrCappedQuality;

typedef struct {
  IMPEncoderRcMode              rcMode;
  union {
    IMPEncoderAttrFixQP         attrFixQp;
    IMPEncoderAttrCbr           attrCbr;
    IMPEncoderAttrVbr           attrVbr;
    IMPEncoderAttrCappedVbr     attrCappedVbr;
    IMPEncoderAttrCappedQuality attrCappedQuality;
  };
} IMPEncoderAttrRcMode;

/**
 * 定义编码channel帧率结构体,frmRateNum和frmRateDen经过最大公约数整除后两者之间的最小公倍数不能超过64，最好在设置之前就被最大公约数整除
 */
typedef struct {
	uint32_t	frmRateNum;				/**< 在一秒钟内的时间单元的数量, 以时间单元为单位。即帧率的分子 */
	uint32_t	frmRateDen;				/**< 在一帧内的时间单元的数量, 以时间单元为单位。即帧率的分母 */
} IMPEncoderFrmRate;


typedef struct {
  IMPEncoderAttrRcMode      attrRcMode;
  IMPEncoderFrmRate         outFrmRate;
} IMPEncoderRcAttr;

/**
 * 定义编码Channel属性结构体
 */
typedef struct {
	IMPEncoderEncAttr     encAttr;	/**< 编码器属性结构体 */
	IMPEncoderRcAttr      rcAttr;		/**< 码率控制器属性结构体,只针对H264和h265 */
	IMPEncoderGopAttr     gopAttr;	/**< 编码器属性结构体 */
} IMPEncoderChnAttr;

/**
 * 定义编码Channel的状态结构体
 */
typedef struct {
	bool		registered;			/**< 注册到Group标志，取值范围:{TRUE, FALSE}，TRUE:注册，FALSE:未注册 */
	uint32_t	leftPics;			/**< 待编码的图像数 */
	uint32_t	leftStreamBytes;	/**< 码流buffer剩余的byte数 */
	uint32_t	leftStreamFrames;	/**< 码流buffer剩余的帧数 */
	uint32_t	curPacks;			/**< 当前帧的码流包个数 */
	uint32_t	work_done;			/**< 通道程序运行状态，0：正在运行，1，未运行 */
} IMPEncoderChnStat;

/**
 * @fn int IMP_Encoder_CreateGroup(int encGroup)
 *
 * 创建编码Group
 *
 * @param[in] encGroup Group号,取值范围:[0, @ref NR_MAX_ENC_GROUPS - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 一路Group仅支持一路分辨率，不同分辨率需启动新的Group。一路Group允许最多注册2个编码channel
 *
 * @attention 如果指定的Group已经存在，则返回失败
 */
int IMP_Encoder_CreateGroup(int encGroup);

/**
 * @fn int IMP_Encoder_DestroyGroup(int encGroup)
 *
 * 销毁编码Grouop.
 *
 * @param[in] encGroup Group号,取值范围:[0, @ref NR_MAX_ENC_GROUPS - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 销毁Group时，必须保证Group为空，即没有任何Channel在Group中注册，或注册到Group中
 * 的Channel已经反注册，否则返回失败
 *
 * @attention 销毁并不存在的Group，则返回失败
 */
int IMP_Encoder_DestroyGroup(int encGroup);


int IMP_Encoder_SetDefaultParam(IMPEncoderChnAttr *chnAttr, IMPEncoderProfile profile, IMPEncoderRcMode rcMode, uint16_t uWidth, uint16_t uHeight, uint32_t frmRateNum, uint32_t frmRateDen, uint32_t uGopLength, int uMaxSameSenceCnt, int iInitialQP, uint32_t uTargetBitRate);

/**
 * @fn int IMP_Encoder_CreateChn(int encChn, const IMPEncoderChnAttr *attr)
 *
 * 创建编码Channel
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] attr 编码Channel属性指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 编码Channel属性由两部分组成，编码器属性和码率控制属性
 * @remarks 编码器属性首先需要选择编码协议，然后分别对各种协议对应的属性进行赋值
 */
int IMP_Encoder_CreateChn(int encChn, const IMPEncoderChnAttr *attr);

/**
 * @fn int IMP_Encoder_DestroyChn(int encChn)
 *
 * 销毁编码Channel
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 销毁并不存在的Channel，则返回失败
 * @attention 销毁前必须保证Channel已经从Group反注册，否则返回失败
 */
int IMP_Encoder_DestroyChn(int encChn);

/**
 * @fn int IMP_Encoder_GetChnAttr(int encChn, IMPEncoderChnAttr * const attr)
 *
 * 获取编码Channel的属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] attr 编码Channel属性
 *
 * @retval 0 成功
 * @retval 非0 失败
 */
int IMP_Encoder_GetChnAttr(int encChn, IMPEncoderChnAttr * const attr);

/**
 * @fn int IMP_Encoder_RegisterChn(int encGroup, int encChn)
 *
 * 注册编码Channel到Group
 *
 * @param[in] encGroup 编码Group号,取值范围: [0, @ref NR_MAX_ENC_GROUPS - 1]
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 注册并不存在的Channel，则返回失败
 * @attention 注册Channel到不存在的Group，否则返回失败
 * @attention 同一个编码Channel只能注册到一个Group，如果该Channel已经注册到某个Group，则返回失败
 * @attention 如果一个Group已经被注册，那么这个Group就不能再被其他的Channel注册，除非之前注册关系被解除
 */

int IMP_Encoder_RegisterChn(int encGroup, int encChn);
/**
 * @fn int IMP_Encoder_UnRegisterChn(int encChn)
 *
 * 反注册编码Channel到Group
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks Channel注销之后，编码Channel会被复位，编码Channel里的码流buffer都会被清空，如果用户还在使用
 * 未及时释放的码流buffer，将不能保证buffer数据的正确性，用户可以使用IMP_Encoder_Query接口来查询编
 * 码Channel码流buffer状态，确认码流buffer里的码流取完之后再反注册Channel
 *
 * @attention 注销未创建的Channel，则返回失败
 * @attention 注销未注册的Channel，则返回失败
 * @attention 如果编码Channel未停止接收图像编码，则返回失败
 */
int IMP_Encoder_UnRegisterChn(int encChn);

/**
 * @fn int IMP_Encoder_StartRecvPic(int encChn)
 *
 * 开启编码Channel接收图像
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 开启编码Channel接收图像后才能开始编码
 *
 * @attention 如果Channel未创建，则返回失败
 * @attention 如果Channel没有注册到Group，则返回失败
 */
int IMP_Encoder_StartRecvPic(int encChn);

/**
 * @fn int IMP_Encoder_StopRecvPic(int encChn)
 *
 * 停止编码Channel接收图像
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 此接口并不判断当前是否停止接收，即允许重复停止接收不返回错误
 * @remarks 调用此接口仅停止接收原始数据编码，码流buffer并不会被消除
 *
 * @attention 如果Channel未创建，则返回失败
 * @attention 如果Channel没有注册到Group，则返回失败
 */
int IMP_Encoder_StopRecvPic(int encChn);

/**
 * @fn int IMP_Encoder_Query(int encChn, IMPEncoderChnStat *stat)
 *
 * 查询编码Channel状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] stat 编码Channel状态
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无
 *
 * @attention 无
 */
int IMP_Encoder_Query(int encChn, IMPEncoderChnStat *stat);

/**
 * @fn int IMP_Encoder_GetStream(int encChn, IMPEncoderStream *stream, bool blockFlag)
 *
 * 获取编码的码流
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] stream 码流结构体指针
 * @param[in] blockFlag 是否使用阻塞方式获取，0：非阻塞，1：阻塞
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 每次获取一帧码流的数据
 * @remarks 如果用户长时间不获取码流,码流缓冲区就会满。一个编码Channel如果发生码流缓冲区满,就会把后
 * 面接收的图像丢掉,直到用户获取码流,从而有足够的码流缓冲可以用于编码时,才开始继续编码。建议用户
 * 获取码流接口调用与释放码流的接口调用成对出现,且尽快释放码流,防止出现由于用户态获取码流,释放不
 * 及时而导致的码流 buffer 满,停止编码。
 * @remarks 对于H264和H265类型码流，一次调用成功获取一帧的码流，这帧码流可能包含多个包。
 * @remarks 对于JPEG类型码流，一次调用成功获取一帧的码流，这帧码流只包含一个包，这一帧包含了JPEG图片文件的完整信息。
 *
 * 示例：
 * @code
 * int ret;
 * ret = IMP_Encoder_PollingStream(ENC_H264_CHANNEL, 1000); //Polling码流Buffer，等待可获取状态
 * if (ret < 0) {
 *     printf("Polling stream timeout\n");
 *     return -1;
 * }
 *
 * IMPEncoderStream stream;
 * ret = IMP_Encoder_GetStream(ENC_H264_CHANNEL, &stream, 1); //获取一帧码流，阻塞方式
 * if (ret < 0) {
 *     printf("Get Stream failed\n");
 *     return -1;
 * }
 *
 * int i, nr_pack = stream.packCount;
 * for (i = 0; i < nr_pack; i++) { //保存这一帧码流的每个包
 *     ret = write(stream_fd, (void *)stream.pack[i].virAddr,
 *                stream.pack[i].length);
 *     if (ret != stream.pack[i].length) {
 *         printf("stream write error:%s\n", strerror(errno));
 *         return -1;
 *     }
 * }
 * @endcode
 *
 * @attention 如果pstStream为NULL,则返回失败；
 * @attention 如果Channel未创建，则返回失败；
 */
int IMP_Encoder_GetStream(int encChn, IMPEncoderStream *stream, bool blockFlag);

/**
 * @fn int IMP_Encoder_ReleaseStream(int encChn, IMPEncoderStream *stream)
 *
 * 释放码流缓存
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] stream 码流结构体指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 此接口应当和IMP_Encoder_GetStream配对起来使用，\n
 * 用户获取码流后必须及时释放已经获取的码流缓存，否则可能会导致码流buffer满，影响编码器编码。\n
 * 并且用户必须按先获取先
 * 释放的顺序释放已经获取的码流缓存；
 * @remarks 在编码Channel反注册后，所有未释放的码流包均无效，不能再使用或者释放这部分无效的码流缓存。
 *
 * @attention 如果pstStream为NULL,则返回失败；
 * @attention 如果Channel未创建，则返回失败；
 * @attention 释放无效的码流会返回失败。
 */
int IMP_Encoder_ReleaseStream(int encChn, IMPEncoderStream *stream);

/**
 * @fn int IMP_Encoder_PollingStream(int encChn, uint32_t timeoutMsec)
 *
 * Polling码流缓存
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] timeoutMsec 超时时间，单位：毫秒
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 在获取码流之前可以用过此API进行Polling，当码流缓存不为空时或超时时函数返回。
 *
 * @attention 无
 */
int IMP_Encoder_PollingStream(int encChn, uint32_t timeoutMsec);

/**
 * @fn int IMP_Encoder_PollingModuleStream(uint32_t *encChnBitmap, uint32_t timeoutMsec)
 *
 * Polling整个编码模组各个已编码channel的码流
 *
 * @param[out] encChnBitmap 每一位的位数代表对应的channel号，若有已编码好的码流，则对应的位置1，否则置0
 * @param[in] timeoutMsec 超时时间，单位：毫秒
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 在获取码流之前可以用过此API进行Polling，当码流缓存不为空时或超时时函数返回。
 * @remarks *encChnBitmap 对应置1的位只有在调用IMP_Encoder_ReleaseStream时当检测到该位对应的channel码流缓存不为空时时才会被置零
 *
 * @attention 无
 */
int IMP_Encoder_PollingModuleStream(uint32_t *encChnBitmap, uint32_t timeoutMsec);

/**
 * @fn int IMP_Encoder_GetFd(int encChn)
 *
 * 获取编码Channel对应的设备文件句柄
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval >=0 成功, 返回设备文件描述符
 * @retval < 0 失败
 *
 * @remarks 在使用IMP_Encoder_PollingStream不合适的场合，比如在同一个地方Polling多个编码channel的编码完成情况时,
 * 可以使用此文件句柄调用select, poll等类似函数来阻塞等待编码完成事件
 * @remarks 调用此API需要通道已经存在
 *
 * @attention 无
 */
int IMP_Encoder_GetFd(int encChn);

/**
 * @fn int IMP_Encoder_SetbufshareChn(int encChn, int shareChn)
 *
 * 设置jpeg通道共享265/264编码通道内存
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] sharechn 被共享内存的264/265编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 调用此API之前被共享内存编码通道已创建
 * @remarks 此API需要在通道创建之前调用
 *
 * @attention 无
 */
int IMP_Encoder_SetbufshareChn(int encChn, int shareChn);

/**
 * @fn int IMP_Encoder_SetChnResizeMode(int encChn, int en);
 *
 * 设置编码缩放是否需要申请额外rmem内存
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] en 使能不需要申请rmem，1：不需要申请rmem，0：需要申请rmem
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 此API只在编码缩放的分辨率小于原始分辨率的情况下调用，编码缩放的分辨率大于原始分辨率无需调用
 *
 * @attention 无
 */
int IMP_Encoder_SetChnResizeMode(int encChn, int en);

/**
 * @fn int IMP_Encoder_SetMaxStreamCnt(int encChn, int nrMaxStream)
 *
 * 设置码流缓存Buffer个数
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] nrMaxStream 码流Buffer数,取值范围: [1, @ref NR_MAX_ENC_CHN_STREAM]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 由于码流缓存Buffer个数在通道创建时就已经固定，因此次API需要在通道创建之前调用。
 * @remarks 若通道创建之前不调用此API设置码流缓存Buffer个数，则使用SDK默认的buffer个数。
 *
 * @attention 无
 */
int IMP_Encoder_SetMaxStreamCnt(int encChn, int nrMaxStream);

/**
 * @fn int IMP_Encoder_GetMaxStreamCnt(int encChn, int *nrMaxStream)
 *
 * 获取码流Buffer数
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] nrMaxStream 码流Buffer数变量指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无
 *
 * @attention 无
 */
int IMP_Encoder_GetMaxStreamCnt(int encChn, int *nrMaxStream);

/**
 * @fn int IMP_Encoder_RequestIDR(int encChn)
 *
 * 请求IDR帧
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 在调用此API后，会在最近的编码帧申请IDR帧编码。
 *
 * @attention 此API只适用于H264和h265编码channel
 */
int IMP_Encoder_RequestIDR(int encChn);

/**
 * @fn int IMP_Encoder_FlushStream(int encChn)
 *
 * 刷掉编码器里残留的旧码流，并以IDR帧开始编码
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 在调用此API后，会在最近的编码帧申请IDR帧编码。
 *
 * @attention 无
 */
int IMP_Encoder_FlushStream(int encChn);

/**
 * @fn int IMP_Encoder_GetChnFrmRate(int encChn, IMPEncoderFrmRate *pstFps)
 *
 * 获取帧率控制属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstFpsCfg 帧率控制属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 调用此API会获取通道的帧率控制属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264和h265编码channel
 */
int IMP_Encoder_GetChnFrmRate(int encChn, IMPEncoderFrmRate *pstFps);

/**
 * @fn int IMP_Encoder_SetChnFrmRate(int encChn, const IMPEncoderFrmRate *pstFps)
 *
 * 动态设置帧率控制属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstFpsCfg 帧率控制属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 调用此API会重新设置编码器帧率属性，帧率属性在下一个GOP生效，最大延时1秒钟生效，调用此API需要通道已经存在。
 * @remarks 如果调用IMP_FrameSource_SetChnFPS()函数动态改变系统帧率，那么需要调用该函数修改编码器帧率，完成正确参数配置。
 *
 * @attention 此API只适用于H264和h265编码channel
 */
int IMP_Encoder_SetChnFrmRate(int encChn, const IMPEncoderFrmRate *pstFps);

int IMP_Encoder_SetChnBitRate(int encChn, int iTargetBitRate, int iMaxBitRate);

int IMP_Encoder_SetChnGopLength(int encChn, int iGopLength);

/**
 * @fn int IMP_Encoder_GetChnAttrRcMode(int encChn, IMPEncoderAttrRcMode *pstRcModeCfg).
 *
 * 获取码率控制模式属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstRcCfg 码率控制模式属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 调用此API会获取通道的码率控制模式属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264和h265编码channel
 */
int IMP_Encoder_GetChnAttrRcMode(int encChn, IMPEncoderAttrRcMode *pstRcModeCfg);

/**
 * @fn int IMP_Encoder_SetChnAttrRcMode(int encChn, const IMPEncoderAttrRcMode *pstRcModeCfg).
 *
 * 设置码率控制模式属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] pstRcCfg 码率控制模式属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 调用此API会设置通道的码率控制模式属性，下一个IDR生效,调用此API需要通道已经存在。
 *
 * @attention 目前，码率控制模式支持ENC_RC_MODE_FIXQP, ENC_RC_MODE_CBR, ENC_RC_MODE_VBR 与 ENC_RC_MODE_SMART
 * @attention 此API只适用于H264和h265编码channel
 */
int IMP_Encoder_SetChnAttrRcMode(int encChn, const IMPEncoderAttrRcMode *pstRcModeCfg);

int IMP_Encoder_GetChnGopAttr(int encChn, IMPEncoderGopAttr *pGopAttr);
int IMP_Encoder_SetChnGopAttr(int encChn, const IMPEncoderGopAttr *pGopAttr);

/**
 * @fn int IMP_Encoder_SetFisheyeEnableStatus(int encChn, int enable)
 *
 * 设置Ingenic提供的鱼眼矫正算法的使能状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] enable 0:不使能(默认),1:使能
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 由于鱼眼矫正算法的使能状态在通道创建时就已经固定，因此次API需要在通道创建之前调用。
 * @remarks 若通道创建之前不调用此API设置Ingenic提供的鱼眼矫正算法的使能状态,则默认不使能，即不能使用君正提供的鱼眼矫正算法。
 *
 * @attention 此API只适用于H264和h265编码channel
 */
int IMP_Encoder_SetFisheyeEnableStatus(int encChn, int enable);

/**
 * @fn int IMP_Encoder_GetFisheyeEnableStatus(int encChn, int *enable)
 *
 * 获取Ingenic提供的鱼眼矫正算法的使能状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] enable 返回设置的Ingenic提供的鱼眼矫正算法的使能状态,0:未使能,1:已使能
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 此API只适用于H264和h265编码channel
 */
int IMP_Encoder_GetFisheyeEnableStatus(int encChn, int *enable);

/**
 * @fn int IMP_Encoder_GetChnEncType(int encChn, IMPEncoderEncType *encType)
 *
 * 获取图像编码协议类型
 *
 * @param[in] encChn 编码Channel号, 取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] encType 返回获取图像编码协议类型
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 如果通道未创建, 则返回失败
 *
 * @attention 无
 */
int IMP_Encoder_GetChnEncType(int encChn, IMPEncoderEncType *encType);

/**
 * @brief int IMP_Encoder_SetPool(int chnNum, int poolID);
 *
 * 绑定chnnel 到内存池中，即Encoder申请mem从pool申请.
 *
 * @param[in] chnNum		通道编号.
 * @param[in] poolID		内存池编号.
 *
 * @retval 0				成功.
 * @retval 非0				失败.
 *
 * @remarks	  为了解决rmem碎片化，将该channel Encoder绑定到对应的mempool
 * 中, Encoder 申请mem就在mempool中申请，若不调用，Encoder会在rmem中申请
 * 此时对于rmem来说会存在碎片的可能
 *
 * @attention ChannelId 必须大于等于0 且小于32
 */
int IMP_Encoder_SetPool(int chnNum, int poolID);

/**
 * @brief IMP_Encoder_GetPool(int chnNum);
 *
 * 通过channel ID 获取poolID.
 *
 * @param[in] chnNum       通道编号.
 *
 * @retval  >=0 && < 32    成功.
 * @retval  <0			   失败.
 *
 * @remarks   通过ChannelId 获取PoolId，客户暂时使用不到
 *
 * @attention 无.
 */
int IMP_Encoder_GetPool(int chnNum);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_ENCODER_H__ */
