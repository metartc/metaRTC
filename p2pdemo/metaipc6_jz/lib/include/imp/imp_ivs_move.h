/*
 * IMP IVS Move func header file.
 *
 * Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_IVS_MOVE_H__
#define __IMP_IVS_MOVE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define IMP_IVS_MOVE_MAX_ROI_CNT		52

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
 * 移动侦测算法的输入结构体
 */
typedef struct {
	int             sense[IMP_IVS_MOVE_MAX_ROI_CNT];   /**< 移动侦测的灵敏度, 对正常摄像机范围是0-4，对全景摄像机范围是0-8 */
	int             skipFrameCnt;                      /*< 隔帧检测的个数 */
	IMPFrameInfo    frameInfo;                         /**< 帧尺寸信息,只需要配置width和height */
	IMPRect         roiRect[IMP_IVS_MOVE_MAX_ROI_CNT]; /*< 需要检测的roi区域坐标信息 */
	int             roiRectCnt;                        /*< 需要检测的roi区域数量，范围为0-51，若为0：则不检测，1：检测roiRect 0
														 区域，2、检测roiRect 0,1区域，3、检测roiRect 0,1,2区域，依次类推 */
} IMP_IVS_MoveParam;

/*
 * 移动侦测算法的输出结构体
 */
typedef struct {
	int retRoi[IMP_IVS_MOVE_MAX_ROI_CNT];				/*< 区域检测移动结果，与roiRect坐标信息严格对应,0:表示未检测到运动，1：表示检测到运动 */
} IMP_IVS_MoveOutput;

/**
 * 创建移动侦测接口资源
 *
 * @fn IMPIVSInterface *IMP_IVS_CreateMoveInterface(IMP_IVS_MoveParam *param);
 *
 * @param[in] param 移动侦测算法的输入结构体参数
 *
 * @retval 非NULL 成功,返回移动侦测算法接口指针句柄
 * @retval NULL 失败
 *
 * @attention 无
 */
IMPIVSInterface *IMP_IVS_CreateMoveInterface(IMP_IVS_MoveParam *param);

/**
 * 销毁移动侦测接口资源
 *
 * @fn void IMP_IVS_DestroyMoveInterface(IMPIVSInterface *moveInterface);
 *
 * @param[in] moveInterface 移动侦测算法接口指针句柄
 *
 * @retval 无返回值
 *
 * @attention 无
 */
void IMP_IVS_DestroyMoveInterface(IMPIVSInterface *moveInterface);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __IMP_IVS_MOVE_H__ */
