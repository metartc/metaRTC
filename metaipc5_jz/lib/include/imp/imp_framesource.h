/*
 * IMP FrameSource header file.
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_FRAMESOURCE_H__
#define __IMP_FRAMESOURCE_H__

#include "imp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * FrameSource模块头文件
 */

/**
 * @defgroup IMP_FrameSource
 * @ingroup imp
 * @brief 视频源，是IMP系统的图像数据源，可设置图像的分辨率、裁减、缩放等属性，以及后端降噪功能
 *
 * FrameSource是一个数据流相关概念，可以设置图像分辨率，格式等，并向后端提供原始图像。
 *
 * FrameSource的结构如下图：
 * @image html FrameSource.png
 * 如上图所示，FrameSource有三路输出，三路输出均可用来编码，其中：
 * * Channel 0一般作为高清视频流
 * * Channel 1一般作为标清视频流，或者IVS只能算法的数据源
 * * Channel 2是拓展通道，在特殊应用下使用，一般不建议使用
 *
 * FrameSource的初始化步骤如下（以两路输出为例）：
 * @code
 * IMPFSChnAttr fs_chn_attr;
 * fs_chn_attr.pixFmt = PIX_FMT_NV12;
 * fs_chn_attr.outFrmRateNum = SENSOR_FRAME_RATE;
 * fs_chn_attr.outFrmRateDen = 1;
 * fs_chn_attr.nrVBs = 3;
 * fs_chn_attr.type = FS_PHY_CHANNEL;
 *
 * fs_chn_attr.crop.enable = 1;
 * fs_chn_attr.crop.top = 0;
 * fs_chn_attr.crop.left = 0;
 * fs_chn_attr.crop.width = 960;
 * fs_chn_attr.crop.height = 640;
 *
 * fs_chn_attr.scaler.enable = 1;
 * fs_chn_attr.scaler.outwidth = 320;
 * fs_chn_attr.scaler.outheight = 240;
 *
 * ret = IMP_FrameSource_CreateChn(0, &fs_chn_attr); //step.1 创建Channel 0
 * if (ret < 0) {
 *     printf("FrameSource_CreateChn(0) error\n");
 *     goto createchn_err;
 * }
 *
 * ret = IMP_FrameSource_EnableChn(0); //step.2 Enable Channel 0，channel 0开始输出图像
 * if (ret < 0) {
 *     printf("EnableChn(0) error\n";
 *     return enablechn_err;
 * }
 *
 * //至此，FrameSource开始向后端Group传递数据
 *
 * ret = IMP_FrameSource_DisableChn(0); //step.3 Disable channel 0，channel 0停止输出图像
 * if (ret < 0) {
 *     printf("FrameSource_DisableChn(0) error\n");
 *     return disablechn_err;
 * }
 *
 * ret = IMP_FrameSource_DestroyChn(0); //step.4 销毁channel 0
 * if (ret < 0) {
 *     printf("FrameSource_DestroyChn error\n");
 *     return destorychn_err;
 * }
 * @endcode
 * 更多使用方法请参考Samples
 * @{
 */

/**
* 通道裁剪结构体
*/
typedef struct {
	int enable;		/**< 使能裁剪功能 */
	int left;		/**< 裁剪左起始点 */
	int top;		/**< 裁剪上起始点 */
	int width;		/**< 图片裁剪宽度 */
	int height;		/**< 图片裁剪高度 */
} IMPFSChnCrop;

/**
* 通道缩放结构体
*/
typedef struct {
	int enable;		/**< 使能缩放功能 */
	int outwidth;	/**< 缩放后图片宽度 */
	int outheight;	/**< 缩放后图片高度 */
} IMPFSChnScaler;

typedef enum {
	FS_PHY_CHANNEL,			/**< 物理通道 */
	FS_EXT_CHANNEL,			/**< 拓展通道 */
} IMPFSChnType;

/**
* 通道FIFO类型
*/
typedef enum {
	FIFO_CACHE_PRIORITY = 0,	/**< FIFO 优先缓存，然后输出数据 */
	FIFO_DATA_PRIORITY,			/**< FIFO 优先输出数据，然后缓存 */
} IMPFSChnFifoType;

/**
* 通道FIFO属性结构体
*/
typedef struct {
	int maxdepth;				/**< FIFO 最大深度 */
	IMPFSChnFifoType type;			/**< 通道FIFO类型 */
} IMPFSChnFifoAttr;

/**
 * 通道属性结构体
 */
typedef struct {
	int picWidth;				/**< 图片宽度 */
	int picHeight;				/**< 图片高度 */
	IMPPixelFormat pixFmt;		/**< 图片格式 */
	IMPFSChnCrop crop;			/**< 图片裁剪属性 */
	IMPFSChnScaler scaler;		/**< 图片缩放属性 */
	int outFrmRateNum;			/**< 通道的输出帧率分子 */
	int outFrmRateDen;			/**< 通道的输出帧率分母 */
	int nrVBs;					/**< Video buffer数量 */
	IMPFSChnType type;			/**< 通道类型 */
	IMPFSChnCrop fcrop;			/**< 图片裁剪属性 */
} IMPFSChnAttr;

/**
 * @fn int IMP_FrameSource_CreateChn(int chnNum, IMPFSChnAttr *chnAttr)
 *
 * 创建通道
 *
 * @param[in] chnNum 通道号
 * @param[in] chnAttr 通道属性结构体指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 创建通道，给后端模块提供数据源; \n
 * 可以设置通道的相关属性，包括：图片宽度，图片高度，图片格式，通道的输出帧率, 缓存buf数，裁剪和缩放属性。\n
 * 对于T10，通道0、1只能被设置为物理通道，通道2,3只能被设置为拓展通道。
 *
 * @attention 无。
 */
int IMP_FrameSource_CreateChn(int chnNum, IMPFSChnAttr *chn_attr);

/**
 * @fn IMP_FrameSource_DestroyChn(int chnNum)
 *
 * 销毁通道
 *
 * @param[in] chnNum 通道号
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 销毁通道
 *
 * @attention 如果程序调用过IMP_FrameSource_EnableChn，一定要调用IMP_FrameSource_DisableChn之后，再使用此函数。
 */
int IMP_FrameSource_DestroyChn(int chnNum);

/**
 * @fn int IMP_FrameSource_EnableChn(int chnNum)
 *
 * 使能通道
 *
 * @param[in] chnNum 通道号
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无
 *
 * @attention 在使用这个函数之前，必须确保所使能的通道已经创建.
 */
int IMP_FrameSource_EnableChn(int chnNum);

/**
 * @fn int IMP_FrameSource_DisableChn(int chnNum)
 *
 * 关闭通道
 *
 * @param[in] chnNum 通道号
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无
 *
 * @attention 无
 */
int IMP_FrameSource_DisableChn(int chnNum);

/**
 * @fn int IMP_FrameSource_SetSource(int extchnNum, int sourcechnNum)
 *
 * 指定扩展通道源通道接口
 *
 * @param[in] extchnNum 扩展通道号
 *
 * @param[in] sourcechnNum 源通道号
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无
 *
 * @attention 在IMP_FrameSource_CreateChn之后, IMP_FrameSource_EnableChn之前调用
 */
int IMP_FrameSource_SetSource(int extchnNum, int sourcechnNum);

/**
 * @fn int IMP_FrameSource_GetChnAttr(int chnNum, IMPFSChnAttr *chnAttr)
 *
 * 获得通道属性
 *
 * @param[in] chnNum 通道号
 *
 * @param[out] chnAttr 通道属性结构体指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 可以获得通道的相关属性，包括：图片宽度，图片高度，图片格式，通道的输出帧率, 缓存buf数，裁剪和缩放属性.
 *
 * @attention 无
 */
int IMP_FrameSource_GetChnAttr(int chnNum, IMPFSChnAttr *chnAttr);

/**
 * @fn int IMP_FrameSource_SetChnAttr(int chnNum, const IMPFSChnAttr *chnAttr)
 *
 * 设置通道属性
 *
 * @param[in] chnNum 通道号
 *
 * @param[in] chnAttr 通道属性结构体指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 可以设置通道的相关属性，包括：图片宽度，图片高度，图片格式，通道的输出帧率, 缓存buf数，裁剪和缩放属性.
 *
 * @attention 无
 */
int IMP_FrameSource_SetChnAttr(int chnNum, const IMPFSChnAttr *chnAttr);

/**
 * @fn IMP_FrameSource_SetFrameDepth(int chnNum, int depth)
 *
 * 设置可获取的图像最大深度
 *
 * @param[in] chnNum 通道的编号
 * @param[in] depth 设置可获取的图像最大深度值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark
 *
 * 1.此接口用于设置某一通道缓存的视频图像帧数。当用户设置缓存多帧视频图像时，用户可以获取到一定数目的连续图像数据。
 *
 * 2.若指定depth为0，表示不需要系统为该通道缓存图像，故用户获取不到该通道图像数据。系统默认不为通道缓存图像，即depth默认为0。
 *
 * 3.系统将自动更新最旧的图像数据，保证用户一旦开始获取，就可获取到最近最新的图像。
 *
 * 4.系统因获取不到图像而自动停止缓存新的图像，用户也不能获取新的图像。因此建议用户保证获取和释放接口配对使用。
 *
 * 5.系统将自动更新用户仍未获取的最旧的图像数据，保证缓存的图像队列为最近最新的图像。由于用户不能保证获取速度，导致获取的可能不是连续的图像。
 *
 * 6.此函数可以调用位置，没有要求。但是只能使用一次。
 *
 * @attention 无.
 */
int IMP_FrameSource_SetFrameDepth(int chnNum, int depth);

/**
 * @fn IMP_FrameSource_GetFrameDepth(int chnNum, int *depth);
 *
 * 获取的图像最大深度
 *
 * @param[in] chnNum 通道的编号
 * @param[out] depth 获取的图像最大深度值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无.
 *
 * @attention 无.
 */
int IMP_FrameSource_GetFrameDepth(int chnNum, int *depth);

/**
 * @fn IMP_FrameSource_GetFrame(int chnNum, IMPFrameInfo **frame);
 *
 * 获取的图像
 *
 * @param[in] chnNum 通道的编号
 * @param[out] frame 获取的图像
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark
 *
 * 1.此接口可以获取指定通道的视频图像信息。图像信息主要包括：图像的宽度、高度、像素格式以及图片数据起始地址。
 *
 * 2.此接口需在通道已启用后才有效。
 *
 * 3.支持多次获取后再释放，但建议获取和释放接口配对使用。
 *
 * 4.该接口默认超时时间为 2s，即2s 内仍未获取到图像，则超时返回。
 *
 * @attention 无.
 */
int IMP_FrameSource_GetFrame(int chnNum, IMPFrameInfo **frame);

/**
 * @fn IMP_FrameSource_GetTimedFrame(int chnNum, IMPFrameTimestamp *framets, int block, void *framedata, IMPFrameInfo *frame);
 *
 * 获取指定时间的图像
 *
 * @param[in] chnNum 通道的编号
 * @param[in] framets 时间信息
 * @param[in] block 阻塞属性
 * @param[in] framedata 拷贝图像的内存指针
 * @param[in] frame 获取到图像信息
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark
 *
 * 1.此接口可以获取指定通道指定时间的视频图像信息。图像信息主要包括：图像的宽度、高度、像素格式以及图片数据。
 *
 * 2.此接口需在通道已启用后才有效。
 *
 * 3.此接口需要先设置IMP_FrameSource_SetMaxDelay和IMP_FrameSource_SetDelay。
 *
 * @attention 无.
 */
int IMP_FrameSource_GetTimedFrame(int chnNum, IMPFrameTimestamp *framets, int block, void *framedata, IMPFrameInfo *frame);

/**
 * @fn IMP_FrameSource_ReleaseFrame(int chnNum, IMPFrameInfo *frame);
 *
 * 释放获取的图像
 *
 * @param[in] chnNum 通道的编号
 * @param[in] frame 释放获取的图像
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无.
 *
 * @attention 无.
 */
int IMP_FrameSource_ReleaseFrame(int chnNum, IMPFrameInfo *frame);

/**
 * @fn IMP_FrameSource_SnapFrame(int chnNum, IMPPixelFormat fmt, int width, int height, void *framedata, IMPFrameInfo *frame);
 *
 * 获取图像
 *
 * @param[in] chnNum 通道的编号
 * @param[in] fmt    图像格式
 * @param[in] width  图像宽度
 * @param[in] height 图像高度
 * @param[in] framedata 拷贝图像的内存指针
 * @param[in] frame 获取到图像信息
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark
 *
 * 1.此接口可以获取一帧指定格式和大小的图像；目前格式支持NV12，YUYV422；大小和通道分辨率一致；不需要调用IMP_FrameSource_SetFrameDepth接口.
 *
 * 2.此接口需在通道已启用后才有效。
 *
 *
 * @attention 无.
 */
int IMP_FrameSource_SnapFrame(int chnNum, IMPPixelFormat fmt, int width, int height, void *framedata, IMPFrameInfo *frame);

/**
 * @fn IMP_FrameSource_SetMaxDelay(int chnNum, int maxcnt);
 *
 * 设置最大延迟帧数
 *
 * @param[in] chnNum 通道的编号
 * @param[in] maxcnt 最大延迟，单位帧
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无.
 *
 * @attention 使用时需要在函数IMP_FrameSource_CreateChn与IMP_FrameSource_EnableChn之间调用.
 */
int IMP_FrameSource_SetMaxDelay(int chnNum, int maxcnt);

/**
 * @fn IMP_FrameSource_GetMaxDelay(int chnNum, int *maxcnt);
 *
 * 获取最大延迟帧数
 *
 * @param[in] chnNum 通道的编号
 * @param[out] maxcnt 最大延迟，单位帧
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无.
 *
 * @attention 使用时需要在函数IMP_FrameSource_CreateChn之后.
 */
int IMP_FrameSource_GetMaxDelay(int chnNum, int *maxcnt);

/**
 * @fn IMP_FrameSource_SetDelay(int chnNum, int cnt);
 *
 * 设置延迟帧数
 *
 * @param[in] chnNum 通道的编号
 * @param[in] cnt 延迟，单位帧
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无.
 *
 * @attention 使用时需要在函数IMP_FrameSource_SetMaxDelay之后调用.
 */
int IMP_FrameSource_SetDelay(int chnNum, int cnt);

/**
 * @fn IMP_FrameSource_GetDelay(int chnNum, int *cnt);
 *
 * 获取延迟帧数
 *
 * @param[in] chnNum 通道的编号
 * @param[out] cnt 延迟，单位帧
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无.
 *
 * @attention 使用时需要在函数IMP_FrameSource_CreateChn之后.
 */
int IMP_FrameSource_GetDelay(int chnNum, int *cnt);

/**
 * @fn IMP_FrameSource_SetChnFifoAttr(int chnNum, IMPFSChnFifoAttr *attr);
 *
 * 设置通道最大缓存FIFO属性
 *
 * @param[in] chnNum 通道的编号
 * @param[in] attr	FIFO属性，包括 FIFO最大深度，单位帧；FIFO 类型.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无.
 *
 * @attention 使用时需要在函数IMP_FrameSource_CreateChn与IMP_FrameSource_EnableChn之间调用.
 */
int IMP_FrameSource_SetChnFifoAttr(int chnNum, IMPFSChnFifoAttr *attr);

/**
 * @fn IMP_FrameSource_GetChnFifoAttr(int chnNum, IMPFSChnFifoAttr *attr);
 *
 * 获取通道最大缓存FIFO属性
 *
 * @param[in] chnNum 通道的编号
 * @param[out] attr	FIFO属性.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无.
 *
 * @attention 使用时需要在函数IMP_FrameSource_CreateChn之后.
 */
int IMP_FrameSource_GetChnFifoAttr(int chnNum, IMPFSChnFifoAttr *attr);

/**
 * @brief IMP_FrameSource_SetPool(int chnNum, int poolID);
 *
 * 绑定chnnel 到内存池中，即FrameSource申请mem从pool申请.
 *
 * @param[in] chnNum		通道编号.
 * @param[in] poolID		内存池编号.
 *
 * @retval 0				成功.
 * @retval 非0				失败.
 *
 * @remarks  为了解决rmem碎片化，将该channel FrameSource 绑定到对应的mempool
 * 中, FramSource 申请mem就在mempool中申请，若不调用，FramSource会在rmem中申请
 * 此时对于rmem来说会存在碎片的可能
 *
 * @attention ChannelId 必须大于等于0 且小于32.
 */
int IMP_FrameSource_SetPool(int chnNum, int poolID);

/**
 * @brief IMP_FrameSource_GetPool(int chnNum);
 *
 * 通过channel ID 获取poolID.
 *
 * @param[in] chnNum       通道编号.
 *
 * @retval  >=0 && < 32    成功.
 * @retval  <0			   失败.
 *
 * @remarks 通过ChannelId 获取poolId, 客户暂时用不到
 *
 * @attention 无.
 */
int IMP_FrameSource_GetPool(int chnNum);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/**
 * @}
 */

#endif /* __IMP_FRAMESOURCE_H__ */
