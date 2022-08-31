/*
 * IMP Decoder func header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_DECODER_H__
#define __IMP_DECODER_H__

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
 * IMP解码器头文件
 */

/**
 * @defgroup IMP_Decoder
 * @ingroup imp
 * @brief 视频解码模块，当前只支持JPEG解码
 * @{
 */

/**
 * 定义解码器属性
 */
typedef struct {
	IMPPayloadType		decType;		/**< 解码帧原数据协议类型 */
	uint32_t			maxWidth;		/**< 解码帧最大的宽度 */
	uint32_t			maxHeight;		/**< 解码帧最大的高度 */
	IMPPixelFormat		pixelFormat;	/**< 解码帧目标数据协议类型 */
	uint32_t			nrKeepStream;	/**< 解码器缓存帧个数 */
	uint32_t			frmRateNum;		/**< 在一秒钟内的时间单元的数量, 以时间单元为单位。即帧率的分子 */
	uint32_t			frmRateDen;		/**< 在一帧内的时间单元的数量, 以时间单元为单位。即帧率的分母 */
} IMPDecoderAttr;

/**
 * 定义解码Channel属性
 */
typedef struct {
	IMPDecoderAttr		decAttr;		/**< 解码器属性 */
} IMPDecoderCHNAttr;

/**
 * 定义解码帧数据属性
 */
typedef struct {
	int					i_payload;		/**< 解码帧的数据长度 */
	uint8_t				*p_payload;		/**< 解码帧的数据指针 */
	int64_t				timeStamp;		/**< 解码帧的时间戳 */
} IMPDecoderNal;

/**
 * 定义解码器码流属性
 */
typedef struct {
	IMPDecoderNal	decoderNal; /**< 解码帧数据结构体 */
} IMPDecoderStream;

/**
 * @fn int IMP_Decoder_CreateChn(int decChn, const IMPDecoderCHNAttr *attr)
 *
 * 创建解码Channel
 *
 * @param[in] decChn 解码Channel号,取值范围: [0, @ref NR_MAX_DEC_CHN - 1]
 * @param[in] attr 解码Channel属性指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 * @attention 无。
 */
int IMP_Decoder_CreateChn(int decChn, const IMPDecoderCHNAttr *attr);

/**
 * @fn int IMP_Decoder_DestroyChn(int decChn)
 *
 * 销毁解码Channel
 *
 * @param[in] decChn 解码Channel号,取值范围: [0, @ref NR_MAX_DEC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 * @attention 无。
 */
int IMP_Decoder_DestroyChn(int decChn);

/**
 * @fn int IMP_Decoder_StartRecvPic(int decChn)
 *
 * 开启解码Channel接收图像
 *
 * @param[in] decChn 解码Channel号,取值范围: [0, @ref NR_MAX_DEC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 开启解码Channel接收图像后才能开始解码
 *
 * @attention 如果Channel未创建，则返回失败
 */
int IMP_Decoder_StartRecvPic(int decChn);

/**
 * @fn int IMP_Decoder_StopRecvPic(int decChn)
 *
 * 停止解码Channel接收图像
 *
 * @param[in] decChn 解码Channel号,取值范围: [0, @ref NR_MAX_DEC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 停止解码Channel接收图像
 *
 * @attention 如果Channel未创建，则返回失败
 */
int IMP_Decoder_StopRecvPic(int decChn);

/**
 * @fn int IMP_Decoder_SendStreamTimeout(int decChn, IMPDecoderStream *stream, uint32_t timeoutMsec)
 *
 * 发送需解码数据
 *
 * @param[in] decChn 解码Channel号,取值范围: [0, @ref NR_MAX_DEC_CHN - 1]
 * @param[in] stream 需解码的数据流结构体指针
 * @param[in] timeoutMsec 解码超时时间 单位ms
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 如果Channel未创建，则返回失败
 */
int IMP_Decoder_SendStreamTimeout(int decChn, IMPDecoderStream *stream, uint32_t timeoutMsec);

/**
 * @fn int IMP_Decoder_PollingFrame(int decChn, uint32_t timeoutMsec)
 *
 * Polling 解码码流缓存
 *
 * @param[in] decChn 解码Channel号,取值范围: [0, @ref NR_MAX_DEC_CHN - 1]
 * @param[in] timeoutMsec 超时时间 单位ms
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 如果Channel未创建，则返回失败
 */
int IMP_Decoder_PollingFrame(int decChn, uint32_t timeoutMsec);

/**
 * @fn int IMP_Decoder_GetFrame(int decChn, IMPFrameInfo **frame)
 *
 * 获取解码码流
 *
 * @param[in] decChn 解码Channel号,取值范围: [0, @ref NR_MAX_DEC_CHN - 1]
 * @param[out] frame 解码码流结构体指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 解码码流buffer由解码器内部申请，该函数只需要传入结构体指针即可。
 */
int IMP_Decoder_GetFrame(int decChn, IMPFrameInfo **frame);

/**
 * @fn int IMP_Decoder_ReleaseFrame(int decChn, IMPFrameInfo *frame)
 *
 * 释放码流缓存
 *
 * @param[in] decChn 解码Channel号,取值范围: [0, @ref NR_MAX_DEC_CHN - 1]
 * @param[in] frame 解码码流结构体指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 无。
 */
int IMP_Decoder_ReleaseFrame(int decChn, IMPFrameInfo *frame);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_DECODER_H__ */
