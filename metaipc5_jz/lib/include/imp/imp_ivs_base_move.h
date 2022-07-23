/*
 * IMP IVS Move func header file.
 *
 * Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_IVS_BASE_MOVE_H__
#define __IMP_IVS_BASE_MOVE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define IMP_IVS_MOVE_MAX_ROI_CNT	52

#include <imp/imp_ivs.h>

/**
 * @file
 * IMP IVS 移动侦测模块
 */

/**
 * @defgroup MoveDetection
 * @ingroup IMP_IVS
 * @brief 移动侦测接口
 * @{
 */

/**
 * 基本移动侦测算法的输入结构体
 */
typedef struct {
	int             skipFrameCnt;                       /*< 隔帧检测的个数 */
	int             referenceNum;                      /*<指定相对于当前帧的第-referenceNum帧为参考帧*/
	int             sadMode;                           /*< SAD模式,0表示8*8*/
	int             sense;                             /*<灵敏度,0-3,值越大灵敏度越大*/
	IMPFrameInfo    frameInfo;                         /**< 帧尺寸信息,只需要配置width和height */

} IMP_IVS_BaseMoveParam;

/*
 * 基本移动侦测算法的输出结构体
 */
typedef struct {
	int ret;
	uint8_t* data;
	int datalen;
	int64_t timeStamp;	/**< 帧的时间戳 */
} IMP_IVS_BaseMoveOutput;

/**
 * 创建移动侦测接口资源
 *
 * @fn IMPIVSInterface *IMP_IVS_CreateBaseMoveInterface(IMP_IVS_BaseMoveParam *param);
 *
 * @param[in] param 移动侦测算法的输入结构体参数
 *
 * @retval 非NULL 成功,返回移动侦测算法接口指针句柄
 * @retval NULL 失败
 *
 * @attention 无
 */
IMPIVSInterface *IMP_IVS_CreateBaseMoveInterface(IMP_IVS_BaseMoveParam *param);

/**
 * 销毁移动侦测接口资源
 *
 * @fn void IMP_IVS_DestroyBaseMoveInterface(IMPIVSInterface *moveInterface);
 *
 * @param[in] moveInterface 移动侦测算法接口指针句柄
 *
 * @retval 无返回值
 *
 * @attention 无
 */
void IMP_IVS_DestroyBaseMoveInterface(IMPIVSInterface *moveInterface);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __IMP_IVS_MOVE_H__ */
