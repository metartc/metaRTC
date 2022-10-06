/*
 * IMP IVS header file.
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_IVS_H__
#define __IMP_IVS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include <imp/imp_common.h>

/**
 * @file
 * IMP IVS模块
 */

#define IMP_IVS_DEFAULT_TIMEOUTMS		(-1)

/**
 * @defgroup IMP_IVS
 * @ingroup imp
 * @brief IVS智能分析通用接口API
 *
 * @section concept 1 相关概念
 * IMP IVS 通过IVS通用接口API调用实例化的IMPIVSInterface以将智能分析算法嵌入到SDK中来分析SDK中的frame图像。
 *
 * @subsection IMPIVSInterface 1.1 IMPIVSInterface
 * IMPIVSInterface 为通用算法接口，具体算法通过实现此接口并将其传给IMP IVS达到在SDK中运行具体算法的目的。\n
 * 一个channel有且仅为单个算法实例的载体，必须将具体实现的通用算法接口传给具体的channel才能在SDK中运行算法。	\n
 * IMPIVSInterface 成员param为成员函数init的参数。 \n
 * IMP_IVS会在传给成员函数ProcessAsync参数的frame时对其进行外部加锁，ProcessAsync必须在使用完frame后调用IMP_IVS_ReleaseData释放frame，以免死锁。
 *
 * @section ivs_usage 2 使用方法
 * 以移动侦测算法为例，函数的具体实现见sample-move_c.c文件\n
 * step.1 初始化系统，可以直接调用范例中的sample_system_init()函数。\n
 * 整个应用程序只能初始化系统一次，若之前初始化了，这儿不需要再初始化。\n
 * step.2 初始化framesource \n
 * 若算法所使用的framesource通道已创建，直接使用已经创建好的通道即可。\n
 * 若算法所使用的framesource通道未创建，可以调用范列中的sample_framesource_init(IVS_FS_CHN, &fs_chn_attr)进行创建。\n
 * step.3 创建ivs具体算法通道组。\n
 * 多个算法可以共用一个通道组，也可以分别使用通道组，具体件sample_ivs_move_init() \n
 * @code
 * int sample_ivs_move_init(int grp_num)
 * {
 *  	int ret = 0;
 *
 *		ret = IMP_IVS_CreateGroup(grp_num);
 *		if (ret < 0) {
 *			IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
 *			return -1;
 *		}
 *		return 0;
 * }
 * @endcode
 * step.4 绑定算法通道组和framesource通道组
 * @code
 *	IMPCell framesource_cell = {DEV_ID_FS, IVS_FS_CHN, 0};
 *	IMPCell ivs_cell = {DEV_ID_IVS, 0, 0};
 *	ret = IMP_System_Bind(&framesource_cell, &ivs_cell);
 *	if (ret < 0) {
 *		IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and ivs0 failed\n", IVS_FS_CHN);
 *		return -1;
 *	}
 * @endcode
 * step.5 启动framesource和算法。建议算法通道号和算法编号一致，以便可以直接对应当前操作哪一个算法。
 * @code
 *	IMP_FrameSource_SetFrameDepth(0, 0);
 *	ret = sample_framesource_streamon(IVS_FS_CHN);
 *	if (ret < 0) {
 *		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
 *		return -1;
 *	}
 *	ret = sample_ivs_move_start(0, 0, &inteface);
 *	if (ret < 0) {
 *		IMP_LOG_ERR(TAG, "sample_ivs_move_start(0, 0) failed\n");
 *		return -1;
 *	}
 * @endcode
 *
 * step.6 获取算法结果 \n
 * Polling结果、获取结果和释放结果必须严格对应，不能中间有中断;
 * 只有Polling结果正确返回，获取到的结果才会被更新，否则获取到的结果无法预知。
 * @code
 *	for (i = 0; i < NR_FRAMES_TO_IVS; i++) {
 *		ret = IMP_IVS_PollingResult(0, IMP_IVS_DEFAULT_TIMEOUTMS);
 *		if (ret < 0) {
 *			IMP_LOG_ERR(TAG, "IMP_IVS_PollingResult(%d, %d) failed\n", 0, IMP_IVS_DEFAULT_TIMEOUTMS);
 *			return -1;
 *		}
 *		ret = IMP_IVS_GetResult(0, (void **)&result);
 *		if (ret < 0) {
 *			IMP_LOG_ERR(TAG, "IMP_IVS_GetResult(%d) failed\n", 0);
 *			return -1;
 *		}
 *		IMP_LOG_INFO(TAG, "frame[%d], result->ret=%d\n", i, result->ret);
 *
 *		ret = IMP_IVS_ReleaseResult(0, (void *)result);
 *		if (ret < 0) {
 *			IMP_LOG_ERR(TAG, "IMP_IVS_ReleaseResult(%d) failed\n", 0);
 *			return -1;
 *		}
 *	}
 * @endcode
 * step.7 释放资源
 * @code
 *  sample_ivs_move_stop(0, inteface);
 *  sample_framesource_streamoff(1);
 *  IMP_System_UnBind(&framesource_cell, &ivs_cell);
 *  sample_ivs_move_exit(0);
 *  sample_framesource_exit(IVS_FS_CHN);
 *  sample_system_exit();
 * @endcode
 * @{
 */

/**
 * ivs 的通用接口
 */
typedef struct IMPIVSInterface IMPIVSInterface;

struct IMPIVSInterface {
	void  *param;													/**< 输入参数 */
	int   paramSize;												/**< 参数空间大小 */
	IMPPixelFormat pixfmt;											/**< 算法需要数据格式 */
	int  (*init)(IMPIVSInterface *inf);								/**< 初始化函数 */
	void (*exit)(IMPIVSInterface *inf);								/**< 注销函数 */
	int  (*preProcessSync)(IMPIVSInterface *inf, IMPFrameInfo *frame);/**< 预处理函数，不对传入此函数的frame额外加锁，故无需free frame，返回值：>=0 正确，<0：错误 */
	int  (*processAsync)(IMPIVSInterface *inf, IMPFrameInfo *frame);/**< 处理函数, SDK IVS 模块对传入此函数的frame 额外加了锁，故此函数必须在该frame使用完毕后尽快使用free_data函数解锁; 此函数是必须实现的函数,算法结果由此函数产生;返回值:0->实际检测正常返回,1->跳帧检测正常返回,-1->错误 */
	int  (*getResult)(IMPIVSInterface *inf, void **result);			/**< 获取结果资源 */
	int  (*releaseResult)(IMPIVSInterface *inf, void *result);		/**< 释放结果资源 */
	int	 (*getParam)(IMPIVSInterface *inf, void *param);			/**< 获得算法参数 */
	int	 (*setParam)(IMPIVSInterface *inf, void *param);			/**< 设置算法参数 */
	int	 (*flushFrame)(IMPIVSInterface *inf);						/**< 释放由外部通过processAsync输入给算法后被缓存的所有frame */
	void *priv;														/**< 私有变量 */
};

/**
 * 创建通道组
 *
 * @fn int IMP_IVS_CreateGroup(int GrpNum);
 *
 * @param[in] GrpNum IVS功能对应的通道组号
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 无
 *
 * @attention 无
 */
int IMP_IVS_CreateGroup(int GrpNum);

/**
 * 销毁通道组
 *
 * @fn int IMP_IVS_DestroyGroup(int GrpNum);
 *
 * @param[in] GrpNum IVS功能对应的通道组号
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 无
 *
 * @attention 无
 */
int IMP_IVS_DestroyGroup(int GrpNum);
/**
 * 创建IVS功能对应的通道
 *
 * @fn int IMP_IVS_CreateChn(int ChnNum, IMPIVSInterface *handler);
 *
 * @param[in] ChnNum 通道号
 *
 * @param[in] handler IVS功能句柄
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 无.
 *
 * @attention 无
 */
int IMP_IVS_CreateChn(int ChnNum, IMPIVSInterface *handler);

/**
 * 销毁IVS功能句柄对应的通道
 *
 * @fn int IMP_IVS_DestroyChn(int ChnNum);
 *
 * @param[in] ChnNum 通道号
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 无
 *
 * @attention 无
 */
int IMP_IVS_DestroyChn(int ChnNum);

/**
 * 注册通道到通道组
 *
 * @fn int IMP_IVS_RegisterChn(int GrpNum, int ChnNum);
 *
 * @param[in] GrpNum IVS功能对应的通道组号
 *
 * @param[in] ChnNum IVS功能对应的通道号
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 将号为Chnnum的通道注册到号为Grpnum通道组中
 *
 * @attention 无
 */
int IMP_IVS_RegisterChn(int GrpNum, int ChnNum);

/**
 * 从通道组注消通道
 *
 * @fn int IMP_IVS_UnRegisterChn(int ChnNum);
 *
 * @param[in] ChnNum IVS功能对应的通道号
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 从号为Grpnum的通道组中注销号为Chnnum通道
 *
 * @attention 无
 */
int IMP_IVS_UnRegisterChn(int ChnNum);

/**
 * 通道开始接收图像
 *
 * @fn int IMP_IVS_StartRecvPic(int ChnNum);
 *
 * @param[in] ChnNum 通道号
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 通道号为Chnnum的IVS功能通道开始接收图像做智能分析
 *
 * @attention 无
 */
int IMP_IVS_StartRecvPic(int ChnNum);

/**
 * 通道停止接收图像
 *
 * @fn int IMP_IVS_StopRecvPic(int ChnNum);
 *
 * @param[in] ChnNum 通道号
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 通道号为Chnnum的IVS功能通道停止接收图像，暂停智能分析
 *
 * @attention 无
 */
int IMP_IVS_StopRecvPic(int ChnNum);

/**
 * 阻塞判断是否可以获得IVS功能已计算出的智能分析结果
 *
 * @fn int IMP_IVS_PollingResult(int ChnNum, int timeoutMs);
 *
 * @param[in] ChnNum IVS功能对应的通道号
 *
 * @param[in] timeout 最大等待时间，单位ms; IMP_IVS_DEFAULT_TIMEOUTMS:库内部默认的等待时间,0:不等待,>0:用户设定的等待时间
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 只有该通道创建时参数IMPIVSInterface结构体中ProcessAsync函数成员返回0时，即实际检测正常返回时，此Polling函数才返回成功
 *
 * @attention 无
 */
int IMP_IVS_PollingResult(int ChnNum, int timeoutMs);

/**
 * 获得IVS功能计算出的智能分析结果
 *
 * @fn int IMP_IVS_GetResult(int ChnNum, void **result);
 *
 * @param[in] ChnNum IVS功能对应的通道号
 *
 * @param[in] result IVS功能对应的通道号输出的结果，返回此通道对应的智能分析算法的结果指针，外部客户无需分配空间。
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 根据不同IVS功能绑定的通道,输出其对应的结果.
 *
 * @attention 无
 */
int IMP_IVS_GetResult(int ChnNum, void **result);

/**
 * 释放IVS功能计算出的结果资源
 *
 * @fn int IMP_IVS_ReleaseResult(int ChnNum, void *result);
 *
 * @param[in] GrpNum 通道组号
 *
 * @param[in] ChnNum IVS功能对应的通道号
 *
 * @param[in] result IVS功能对应的通道号输出的结果
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 根据不同IVS功能绑定的通道,释放其输出的结果资源.
 *
 * @attention 无
 */
int IMP_IVS_ReleaseResult(int ChnNum, void *result);

/**
 * 释放传给Datacallback的参数frame
 *
 * @fn int IMP_IVS_ReleaseData(void *vaddr);
 *
 * @param[in] vaddr 释放的空间虚拟地址
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @remark 必须使用此函数释放传给Datacallback的frame参数，否则肯定造成死锁。
 * @remark 此接口仅供算法提供商使用，算法使用客户无须关注。
 *
 * @attention 无
 */
int IMP_IVS_ReleaseData(void *vaddr);

/**
 * 获取通道算法参数
 *
 * @fn int IMP_IVS_GetParam(int chnNum, void *param);
 *
 * @param[in] ChnNum IVS功能对应的通道号
 * @param[in] param 算法参数虚拟地址指针
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @attention 无
 */
int IMP_IVS_GetParam(int chnNum, void *param);

/**
 * 设置通道算法参数
 *
 * @fn int IMP_IVS_SetParam(int chnNum, void *param);
 *
 * @param[in] ChnNum IVS功能对应的通道号
 * @param[in] param 算法参数虚拟地址指针
 *
 * @retval 0 成功
 * @retval -1 失败
 *
 * @attention 无
 */
int IMP_IVS_SetParam(int chnNum, void *param);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_IVS_H__ */
