/*
 * IMP ISP header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_ISP_H__
#define __IMP_ISP_H__
#include <stdbool.h>

#include "imp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * ISP模块头文件
 */

/**
 * @defgroup IMP_ISP
 * @ingroup imp
 * @brief 图像信号处理单元。主要包含图像效果设置、模式切换以及Sensor的注册添加删除等操作
 *
 * ISP模块与数据流无关，不需要进行Bind，仅作用于效果参数设置及Sensor控制。
 *
 * ISP模块的使能步骤如下：
 * @code
 * int ret = 0;
 * ret = IMP_ISP_Open(); // step.1 创建ISP模块
 * if(ret < 0){
 *     printf("Failed to ISPInit\n");
 *     return -1;
 * }
 * IMPSensorInfo sensor;
 * sensor.name = "xxx";
 * sensor.cbus_type = SENSOR_CONTROL_INTERFACE_I2C; // OR SENSOR_CONTROL_INTERFACE_SPI
 * sensor.i2c = {
 * 	.type = "xxx", // I2C设备名字，必须和sensor驱动中struct i2c_device_id中的name一致。
 *	.addr = xx,	//I2C地址
 *	.i2c_adapter_id = xx, //sensor所在的I2C控制器ID
 * }
 * OR
 * sensor.spi = {
 *	.modalias = "xx", //SPI设备名字，必须和sensor驱动中struct spi_device_id中的name一致。
 *	.bus_num = xx, //SPI总线地址
 * }
 * ret = IMP_ISP_AddSensor(&sensor); //step.2 添加一个sensor，在此操作之前sensor驱动已经添加到内核。
 * if (ret < 0) {
 *     printf("Failed to Register sensor\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_EnableSensor(void); //step.3 使能sensor，现在sensor开始输出图像。
 * if (ret < 0) {
 *     printf("Failed to EnableSensor\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_EnableTuning(); //step.4 使能ISP tuning, 然后才能调用ISP调试接口。
 * if (ret < 0) {
 *     printf("Failed to EnableTuning\n");
 *     return -1;
 * }
 *
 * 调试接口请参考ISP调试接口文档。 //step.5 效果调试。
 *
 * @endcode
 * ISP模块的卸载步骤如下：
 * @code
 * int ret = 0;
 * IMPSensorInfo sensor;
 * sensor.name = "xxx";
 * ret = IMP_ISP_DisableTuning(); //step.1 关闭ISP tuning
 * if (ret < 0) {
 *     printf("Failed to disable tuning\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_DisableSensor(); //step.2 关闭sensor，现在sensor停止输出图像；在此操作前FrameSource必须全部关闭。
 * if (ret < 0) {
 *     printf("Failed to disable sensor\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_DelSensor(&sensor); //step.3 删除sensor，在此操作前sensor必须关闭。
 * if (ret < 0) {
 *     printf("Failed to disable sensor\n");
 *     return -1;
 * }
 *
 * ret = IMP_ISP_Close(); //step.4 清理ISP模块，在此操作前所有sensor都必须被删除。
 * if (ret < 0) {
 *     printf("Failed to disable sensor\n");
 *     return -1;
 * }
 * @endcode
 * 更多使用方法请参考Samples
 * @{
 */

/**
* 摄像头控制总线类型枚举
*/
typedef enum {
	TX_SENSOR_CONTROL_INTERFACE_I2C = 1,	/**< I2C控制总线 */
	TX_SENSOR_CONTROL_INTERFACE_SPI,	/**< SPI控制总线 */
} IMPSensorControlBusType;

/**
* 摄像头控制总线类型是I2C时，需要配置的参数结构体
*/
typedef struct {
	char type[20];		/**< I2C设备名字，必须与摄像头驱动中struct i2c_device_id中name变量一致 */
	int addr;		/**< I2C地址 */
	int i2c_adapter_id;	/**< I2C控制器 */
} IMPI2CInfo;
/**
* 摄像头控制总线类型是SPI时，需要配置的参数结构体
*/
typedef struct {
	char modalias[32];	/**< SPI设备名字，必须与摄像头驱动中struct spi_device_id中name变量一致 */
	int bus_num;		/**< SPI总线地址 */
} IMPSPIInfo;

/**
* 摄像头注册信息结构体
*/
typedef struct {
	char name[32];					/**< 摄像头名字 */
	IMPSensorControlBusType cbus_type;	/**< 摄像头控制总线类型 */
	union {
		IMPI2CInfo i2c;				/**< I2C总线信息 */
		IMPSPIInfo spi;				/**< SPI总线信息 */
	};
	unsigned short rst_gpio;		/**< 摄像头reset接口链接的GPIO，注意：现在没有启用该参数 */
	unsigned short pwdn_gpio;		/**< 摄像头power down接口链接的GPIO，注意：现在没有启用该参数 */
	unsigned short power_gpio;		/**< 摄像头power 接口链接的GPIO，注意：现在没有启用该参数 */
} IMPSensorInfo;

/**
 * @fn int IMP_ISP_Open(void)
 *
 * 打开ISP模块
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 创建ISP模块，准备向ISP添加sensor，并开启ISP效果调试功能。
 *
 * @attention 这个函数必须在添加sensor之前被调用。
 */
int IMP_ISP_Open(void);

/**
 * @fn int IMP_ISP_Close(void)
 *
 * 关闭ISP模块
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark ISP模块，ISP模块不再工作。
 *
 * @attention 在使用这个函数之前，必须保证所有FrameSource和效果调试功能已经关闭，所有sensor都已被卸载.
 */
int IMP_ISP_Close(void);

/**
 * @fn int IMP_ISP_AddSensor(IMPSensorInfo *pinfo)
 *
 * 添加一个sensor，用于向ISP模块提供数据源
 *
 * @param[in] pinfo 需要添加sensor的信息指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 添加一个摄像头，用于提供图像。
 *
 * @attention 在使用这个函数之前，必须保证摄像头驱动已经注册进内核.
 */
int IMP_ISP_AddSensor(IMPSensorInfo *pinfo);

/**
 * @fn int IMP_ISP_DelSensor(IMPSensorInfo *pinfo)
 *
 * 删除一个sensor
 *
 * @param[in] pinfo 需要删除sensor的信息指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 删除一个摄像头。
 *
 * @attention 在使用这个函数之前，必须保证摄像头已经停止工作，即调用了IMP_ISP_DisableSensor函数.
 */
int IMP_ISP_DelSensor(IMPSensorInfo *pinfo);

/**
 * @fn int IMP_ISP_EnableSensor(void)
 *
 * 使能一个sensor
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 使能一个摄像头，使之开始传输图像, 这样FrameSource才能输出图像，同时ISP才能进行效果调试。
 *
 * @attention 在使用这个函数之前，必须保证摄像头已经被添加进ISP模块.
 */
int IMP_ISP_EnableSensor(void);

/**
 * @fn int IMP_ISP_DisableSensor(void)
 *
 * 不使能一个sensor
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 不使能一个摄像头，使之停止传输图像, 这样FrameSource无法输出图像，同时ISP也不能进行效果调试。
 *
 * @attention 在使用这个函数之前，必须保证所有FrameSource都已停止输出图像，同时效果调试也在不使能态.
 */
int IMP_ISP_DisableSensor(void);

/**
 * @fn int IMP_ISP_SetSensorRegister(uint32_t reg, uint32_t value)
 *
 * 设置sensor一个寄存器的值
 *
 * @param[in] reg 寄存器地址
 *
 * @param[in] value 寄存器值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 可以直接设置一个sensor寄存器的值。
 *
 * @attention 在使用这个函数之前，必须保证摄像头已经被使能.
 */
int IMP_ISP_SetSensorRegister(uint32_t reg, uint32_t value);

/**
 * @fn int IMP_ISP_GetSensorRegister(uint32_t reg, uint32_t *value)
 *
 * 获取sensor一个寄存器的值
 *
 * @param[in] reg 寄存器地址
 *
 * @param[in] value 寄存器值的指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 可以直接获取一个sensor寄存器的值。
 *
 * @attention 在使用这个函数之前，必须保证摄像头已经被使能.
 */
int IMP_ISP_GetSensorRegister(uint32_t reg, uint32_t *value);

/**
 * ISP功能开关
 */
typedef enum {
	IMPISP_TUNING_OPS_MODE_DISABLE,			/**< 不使能该模块功能 */
	IMPISP_TUNING_OPS_MODE_ENABLE,			/**< 使能该模块功能 */
	IMPISP_TUNING_OPS_MODE_BUTT,			/**< 用于判断参数的有效性，参数大小必须小于这个值 */
} IMPISPTuningOpsMode;

/**
 * ISP功能选用开关
 */
typedef enum {
	IMPISP_TUNING_OPS_TYPE_AUTO,			/**< 该模块的操作为自动模式 */
	IMPISP_TUNING_OPS_TYPE_MANUAL,			/**< 该模块的操作为手动模式 */
	IMPISP_TUNING_OPS_TYPE_BUTT,			/**< 用于判断参数的有效性，参数大小必须小于这个值 */
} IMPISPTuningOpsType;

typedef struct {
	unsigned int zone[15][15];    /**< 各区域信息*/
} IMPISPZone;

/**
 * @fn int IMP_ISP_EnableTuning(void)
 *
 * 使能ISP效果调试功能
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证IMP_ISP_EnableSensor被执行且返回成功.
 */
int IMP_ISP_EnableTuning(void);

/**
 * @fn int IMP_ISP_DisableTuning(void)
 *
 * 不使能ISP效果调试功能
 *
 * @param 无
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证在不使能sensor之前，先不使能ISP效果调试（即调用此函数）.
 */
int IMP_ISP_DisableTuning(void);

/**
 * @fn int IMP_ISP_Tuning_SetSensorFPS(uint32_t fps_num, uint32_t fps_den)
 *
 * 设置摄像头输出帧率
 *
 * @param[in] fps_num 设定帧率的分子参数
 * @param[in] fps_den 设定帧率的分母参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证IMP_ISP_EnableSensor 和 IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetSensorFPS(uint32_t fps_num, uint32_t fps_den);

/**
 * @fn int IMP_ISP_Tuning_GetSensorFPS(uint32_t *fps_num, uint32_t *fps_den)
 *
 * 获取摄像头输出帧率
 *
 * @param[in] fps_num 获取帧率分子参数的指针
 * @param[in] fps_den 获取帧率分母参数的指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证IMP_ISP_EnableSensor 和 IMP_ISP_EnableTuning已被调用。
 * @attention 在使能帧通道开始传输数据之前必须先调用此函数获取摄像头默认帧率。
 */
int IMP_ISP_Tuning_GetSensorFPS(uint32_t *fps_num, uint32_t *fps_den);

/**
 * ISP抗闪频属性参数结构体。
 */
typedef enum {
	IMPISP_ANTIFLICKER_DISABLE,	/**< 不使能ISP抗闪频功能 */
	IMPISP_ANTIFLICKER_50HZ,	/**< 使能ISP抗闪频功能, 并设置频率为50HZ */
	IMPISP_ANTIFLICKER_60HZ,	/**< 使能ISP抗闪频功能，并设置频率为60HZ */
	IMPISP_ANTIFLICKER_BUTT,	/**< 用于判断参数的有效性，参数大小必须小于这个值 */
} IMPISPAntiflickerAttr;

/**
 * @fn int IMP_ISP_Tuning_SetAntiFlickerAttr(IMPISPAntiflickerAttr attr)
 *
 * 设置ISP抗闪频属性
 *
 * @param[in] attr 设置参数值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetAntiFlickerAttr(IMPISPAntiflickerAttr attr);

/**
 * @fn int IMP_ISP_Tuning_GetAntiFlickerAttr(IMPISPAntiflickerAttr *pattr)
 *
 * 获得ISP抗闪频属性
 *
 * @param[in] pattr 获取参数值指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetAntiFlickerAttr(IMPISPAntiflickerAttr *pattr);

/**
 * @fn int IMP_ISP_Tuning_SetBrightness(unsigned char bright)
 *
 * 设置ISP 综合效果图片亮度
 *
 * @param[in] bright 图片亮度参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加亮度，小于128降低亮度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetBrightness(unsigned char bright);

/**
 * @fn int IMP_ISP_Tuning_GetBrightness(unsigned char *pbright)
 *
 * 获取ISP 综合效果图片亮度
 *
 * @param[in] bright 图片亮度参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加亮度，小于128降低亮度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetBrightness(unsigned char *pbright);

/**
 * @fn int IMP_ISP_Tuning_SetContrast(unsigned char contrast)
 *
 * 设置ISP 综合效果图片对比度
 *
 * @param[in] contrast 图片对比度参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加对比度，小于128降低对比度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetContrast(unsigned char contrast);

/**
 * @fn int IMP_ISP_Tuning_GetContrast(unsigned char *pcontrast)
 *
 * 获取ISP 综合效果图片对比度
 *
 * @param[in] contrast 图片对比度参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加对比度，小于128降低对比度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetContrast(unsigned char *pcontrast);

 /**
 * @fn int IMP_ISP_Tuning_SetSharpness(unsigned char sharpness)
 *
 * 设置ISP 综合效果图片锐度
 *
 * @param[in] sharpness 图片锐度参数值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加锐度，小于128降低锐度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetSharpness(unsigned char sharpness);

/**
 * @fn int IMP_ISP_Tuning_GetSharpness(unsigned char *psharpness)
 *
 * 获取ISP 综合效果图片锐度
 *
 * @param[in] sharpness 图片锐度参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加锐度，小于128降低锐度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetSharpness(unsigned char *psharpness);

/**
 * @fn int IMP_ISP_Tuning_SetBcshHue(unsigned char hue)
 *
 * 设置图像的色调
 *
 * @param[in] hue 图像的色调参考值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128正向调节色调，小于128反向调节色调，调节范围0~255。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetBcshHue(unsigned char hue);

/**
 * @fn int IMP_ISP_Tuning_GetBcshHue(unsigned char *hue)
 *
 * 获取图像的色调值。
 *
 * @param[out] hue 图像的色调参数指针。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128代表正向调节色调，小于128代表反向调节色调，范围0~255。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetBcshHue(unsigned char *hue);

/**
 * @fn int IMP_ISP_Tuning_SetSaturation(unsigned char sat)
 *
 * 设置ISP 综合效果图片饱和度
 *
 * @param[in] sat 图片饱和度参数值
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加饱和度，小于128降低饱和度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_SetSaturation(unsigned char sat);

/**
 * @fn int IMP_ISP_Tuning_GetSaturation(unsigned char *psat)
 *
 * 获取ISP 综合效果图片饱和度
 *
 * @param[in] sat 图片饱和度参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 默认值为128，大于128增加饱和度，小于128降低饱和度。
 *
 * @attention 在使用这个函数之前，必须保证ISP效果调试功能已使能.
 */
int IMP_ISP_Tuning_GetSaturation(unsigned char *psat);

/**
 * @fn int IMP_ISP_Tuning_SetISPBypass(IMPISPTuningOpsMode enable)
 *
 * ISP模块是否bypass
 *
 * @param[in] enable 是否bypass输出模式
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @remark 无
 *
 * @attention 在使用这个函数之前，必须保证ISP模块是关闭的.
 */
int IMP_ISP_Tuning_SetISPBypass(IMPISPTuningOpsMode enable);

/**
 * @fn int IMP_ISP_Tuning_GetTotalGain(uint32_t *gain)
 *
 * 获取ISP输出图像的整体增益值
 *
 * @param[in] gain 获取增益值参数的指针,其数据存放格式为[24.8]，高24bit为整数，低8bit为小数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，必须保证IMP_ISP_EnableSensor 和 IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetTotalGain(uint32_t *gain);

/**
 * 设置ISP图像镜面效果功能是否使能
 *
 * @fn int IMP_ISP_Tuning_SetISPHflip(IMPISPTuningOpsMode mode)
 *
 * @param[in] mode 是否使能镜面效果
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetISPHflip(IMPISPTuningOpsMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetISPHflip(IMPISPTuningOpsMode *pmode)
 *
 * 获取ISP图像镜面效果功能的操作状态
 *
 * @param[in] pmode 操作参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetISPHflip(IMPISPTuningOpsMode *pmode);

/**
 * @fn int IMP_ISP_Tuning_SetISPVflip(IMPISPTuningOpsMode mode)
 *
 * 设置ISP图像上下反转效果功能是否使能
 *
 * @param[in] mode 是否使能图像上下反转
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetISPVflip(IMPISPTuningOpsMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetISPVflip(IMPISPTuningOpsMode *pmode)
 *
 * 获取ISP图像上下反转效果功能的操作状态
 *
 * @param[in] pmode 操作参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetISPVflip(IMPISPTuningOpsMode *pmode);

/**
 * ISP 工作模式配置，正常模式或夜视模式。
 */
typedef enum {
	IMPISP_RUNNING_MODE_DAY = 0,				/**< 正常模式 */
	IMPISP_RUNNING_MODE_NIGHT = 1,				/**< 夜视模式 */
	IMPISP_RUNNING_MODE_BUTT,					/**< 最大值 */
} IMPISPRunningMode;

/**
 * @fn int IMP_ISP_Tuning_SetISPRunningMode(IMPISPRunningMode mode)
 *
 * 设置ISP工作模式，正常模式或夜视模式；默认为正常模式。
 *
 * @param[in] mode运行模式参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * 示例：
 * @code
 * IMPISPRunningMode mode;
 *
 *	if( it is during a night now){
		mode = IMPISP_RUNNING_MODE_NIGHT
	}else{
		mode = IMPISP_RUNNING_MODE_DAY;
	}
	ret = IMP_ISP_Tuning_SetISPRunningMode(mode);
	if(ret){
		IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetISPRunningMode error !\n");
		return -1;
	}
 *
 * @endcode
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetISPRunningMode(IMPISPRunningMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetISPRunningMode(IMPISPRunningMode *pmode)
 *
 * 获取ISP工作模式，正常模式或夜视模式。
 *
 * @param[in] pmode操作参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetISPRunningMode(IMPISPRunningMode *pmode);

/**
 * @fn int IMP_ISP_Tuning_SetISPCustomMode(IMPISPTuningOpsMode mode)
 *
 * 使能ISP Custom Mode，加载另外一套效果参数.
 *
 * @param[in] mode Custom 模式，使能或者关闭
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */

int IMP_ISP_Tuning_SetISPCustomMode(IMPISPTuningOpsMode mode);

/**
 * @fn int IMP_ISP_Tuning_GetISPCustomMode(IMPISPTuningOpsMode mode)
 *
 * 获取ISP Custom Mode的状态.
 *
 * @param[out] mode Custom 模式，使能或者关闭
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetISPCustomMode(IMPISPTuningOpsMode *pmode);

/**
 * gamma
 */
typedef struct {
	uint16_t gamma[129];		/**< gamma参数数组，有129个点 */
} IMPISPGamma;

/**
* @fn int IMP_ISP_Tuning_SetGamma(IMPISPGamma *gamma)
*
* 设置GAMMA参数.
* @param[in] gamma gamma参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_SetGamma(IMPISPGamma *gamma);

/**
* @fn int IMP_ISP_Tuning_GetGamma(IMPISPGamma *gamma)
*
* 获取GAMMA参数.
* @param[out] gamma gamma参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_GetGamma(IMPISPGamma *gamma);

/**
* @fn int IMP_ISP_Tuning_SetAeComp(int comp)
*
* 设置AE补偿。AE补偿参数可以调整图像AE target，范围为[0-255].
* @param[in] comp AE补偿参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_SetAeComp(int comp);

/**
* @fn int IMP_ISP_Tuning_GetAeComp(int *comp)
*
* 获取AE补偿。
* @param[out] comp AE补偿参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_GetAeComp(int *comp);

/**
* @fn int IMP_ISP_Tuning_GetAeLuma(int *luma)
*
* 获取画面平均亮度。
*
* @param[out] luma AE亮度参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_GetAeLuma(int *luma);

/**
 * @fn int IMP_ISP_Tuning_SetAeFreeze(IMPISPTuningOpsMode mode)
 *
 * 使能AE Freeze功能.
 *
 * @param[in] mode AE Freeze功能使能参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */

int IMP_ISP_Tuning_SetAeFreeze(IMPISPTuningOpsMode mode);

/**
 * 曝光模式
 */
enum isp_core_expr_mode {
	ISP_CORE_EXPR_MODE_AUTO,			/**< 自动模式 */
	ISP_CORE_EXPR_MODE_MANUAL,			/**< 手动模式 */
};

/**
 * 曝光单位
 */
enum isp_core_expr_unit {
	ISP_CORE_EXPR_UNIT_LINE,			/**< 行 */
	ISP_CORE_EXPR_UNIT_US,				/**< 毫秒 */
};

/**
 * 曝光参数
 */
typedef union isp_core_expr_attr{
	struct {
		enum isp_core_expr_mode mode;		/**< 设置的曝光模式 */
		enum isp_core_expr_unit unit;		/**< 设置的曝光单位 */
		uint16_t time;
	} s_attr;
	struct {
		enum isp_core_expr_mode mode;			/**< 获取的曝光模式 */
		uint16_t integration_time;		/**< 获取的曝光时间，单位为行 */
		uint16_t integration_time_min;	/**< 获取的曝光最小时间，单位为行 */
		uint16_t integration_time_max;	/**< 获取的曝光最大时间，单位为行 */
		uint16_t one_line_expr_in_us;		/**< 获取的一行曝光时间对应的微妙数 */
	} g_attr;
}IMPISPExpr;


/**
 * @fn int IMP_ISP_Tuning_SetExpr(IMPISPExpr *expr)
 *
 * 设置AE参数。
 *
 * @param[in] expr AE参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetExpr(IMPISPExpr *expr);

/**
 * @fn int IMP_ISP_Tuning_GetExpr(IMPISPExpr *expr)
 *
 * 获取AE参数。
 *
 * @param[out] expr AE参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetExpr(IMPISPExpr *expr);

/**
 * 曝光统计区域选择
 */
typedef union isp_core_ae_roi_select{
	struct {
		unsigned endy :8;                   /**< 结束点y坐标 (0 ~ 255)*/
		unsigned endx :8;                   /**< 结束点x坐标 (0 ~ 255)*/
		unsigned starty :8;                 /**< 起始点y坐标 (0 ~ 255)*/
		unsigned startx :8;                 /**< 起始点x坐标 (0 ~ 255)*/
	};
	uint32_t value;
} IMPISPAERoi;

/**
 * 白平衡模式
 */
enum isp_core_wb_mode {
	ISP_CORE_WB_MODE_AUTO = 0,			/**< 自动模式 */
	ISP_CORE_WB_MODE_MANUAL,			/**< 手动模式 */
	ISP_CORE_WB_MODE_DAY_LIGHT,			/**< 晴天 */
	ISP_CORE_WB_MODE_CLOUDY,			/**< 阴天 */
	ISP_CORE_WB_MODE_INCANDESCENT,		/**< 白炽灯 */
	ISP_CORE_WB_MODE_FLOURESCENT,		/**< 荧光灯 */
	ISP_CORE_WB_MODE_TWILIGHT,			/**< 黄昏 */
	ISP_CORE_WB_MODE_SHADE,				/**< 阴影 */
	ISP_CORE_WB_MODE_WARM_FLOURESCENT,	/**< 暖色荧光灯 */
	ISP_CORE_WB_MODE_CUSTOM,	/**< 自定义模式 */
};

/**
 * 白平衡参数
 */
typedef struct isp_core_wb_attr{
	enum isp_core_wb_mode mode;		/**< 白平衡模式，分为自动与手动模式 */
	uint16_t rgain;			/**< 红色增益，手动模式时有效 */
	uint16_t bgain;			/**< 蓝色增益，手动模式时有效 */
}IMPISPWB;

/**
 * @fn int IMP_ISP_Tuning_SetWB(IMPISPWB *wb)
 *
 * 设置白平衡功能设置。可以设置自动与手动模式，手动模式主要通过设置rgain、bgain实现。
 *
 * @param[in] wb 设置的白平衡参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetWB(IMPISPWB *wb);

/**
 * @fn int IMP_ISP_Tuning_GetWB(IMPISPWB *wb)
 *
 * 获取白平衡功能设置。
 *
 * @param[out] wb 获取的白平衡参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetWB(IMPISPWB *wb);

/**
 * @fn IMP_ISP_Tuning_GetWB_Statis(IMPISPWB *wb)
 *
 * 获取白平衡统计值。
 *
 * @param[out] wb 获取的白平衡统计值。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetWB_Statis(IMPISPWB *wb);

/**
 * @fn IMP_ISP_Tuning_GetWB_GOL_Statis(IMPISPWB *wb)
 *
 * 获取白平衡全局统计值。
 *
 * @param[out] wb 获取的白平衡全局统计值。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetWB_GOL_Statis(IMPISPWB *wb);

/**
 * @fn int IMP_ISP_Tuning_SetMaxAgain(uint32_t gain)
 *
 * 设置sensor可以设置最大Again。
 *
 * @param[in] gain sensor可以设置的最大again.0表示1x，32表示2x，依次类推。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */

typedef struct isp_core_rgb_coefft_wb_attr {
		unsigned short rgb_coefft_wb_r;
		unsigned short rgb_coefft_wb_g;
		unsigned short rgb_coefft_wb_b;

}IMPISPCOEFFTWB;
/**
 * @fn IMP_ISP_Tuning_Awb_GetRgbCoefft(IMPISPCOEFFTWB *isp_core_rgb_coefft_wb_attr)
 *
 * 获取sensor AWB RGB通道偏移参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_Awb_GetRgbCoefft(IMPISPCOEFFTWB *isp_core_rgb_coefft_wb_attr);
/**
 * @fn IMP_ISP_Tuning_Awb_SetRgbCoefft(IMPISPCOEFFTWB *isp_core_rgb_coefft_wb_attr)
 *
 * 设置sensor可以设置AWB RGB通道偏移参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 *
 * 示例：
 * @code
 * IMPISPCOEFFTWB isp_core_rgb_coefft_wb_attr;
 *
 *isp_core_rgb_coefft_wb_attr.rgb_coefft_wb_r=x;
 *isp_core_rgb_coefft_wb_attr.rgb_coefft_wb_g=y;
 *isp_core_rgb_coefft_wb_attr.rgb_coefft_wb_b=z;
 *IMP_ISP_Tuning_Awb_SetRgbCoefft(&isp_core_rgb_coefft_wb_attr);
 if(ret){
 IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_Awb_SetRgbCoefft error !\n");
 return -1;
 }
*/
int IMP_ISP_Tuning_Awb_SetRgbCoefft(IMPISPCOEFFTWB *isp_core_rgb_coefft_wb_attr);

int IMP_ISP_Tuning_SetMaxAgain(uint32_t gain);

/**
 * @fn int IMP_ISP_Tuning_GetMaxAgain(uint32_t *gain)
 *
 * 获取sensor可以设置最大Again。
 *
 * @param[out] gain sensor可以设置的最大again.0表示1x，32表示2x，依次类推。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetMaxAgain(uint32_t *gain);

/**
 * @fn int IMP_ISP_Tuning_SetMaxDgain(uint32_t gain)
 *
 * 设置ISP可以设置的最大Dgain。
 *
 * @param[in] ISP Dgain 可以设置的最大dgain.0表示1x，32表示2x，依次类推。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetMaxDgain(uint32_t gain);

/**
 * @fn int IMP_ISP_Tuning_GetMaxDgain(uint32_t *gain)
 *
 * 获取ISP设置的最大Dgain。
 *
 * @param[out] ISP Dgain 可以得到设置的最大的dgain.0表示1x，32表示2x，依次类推。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetMaxDgain(uint32_t *gain);

/**
 * @fn int IMP_ISP_Tuning_SetVideoDrop(void (*cb)(void))
 *
 * 设置视频丢失功能。当出现sensor与主板的连接线路出现问题时，设置的回调函数会被执行。
 *
 * @param[in] cb 回调函数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetVideoDrop(void (*cb)(void));

/**
 * @fn int IMP_ISP_Tuning_SetHiLightDepress(uint32_t strength)
 *
 * 设置强光抑制强度。
 *
 * @param[in] strength 强光抑制强度参数.取值范围为［0-10], 0表示关闭功能。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetHiLightDepress(uint32_t strength);

/**
 * @fn int IMP_ISP_Tuning_GetHiLightDepress(uint32_t *strength)
 *
 * 获取强光抑制的强度。
 *
 * @param[out] strength 可以得到设置的强光抑制的强度.0表示关闭此功能。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetHiLightDepress(uint32_t *strength);

/**
 * @fn int IMP_ISP_Tuning_SetTemperStrength(uint32_t ratio)
 *
 * 设置3D降噪强度。
 *
 * @param[in] ratio 强度调节比例.默认值为128,如果设置大于128则增加强度，小于128降低强度.取值范围为［0-255]. *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetTemperStrength(uint32_t ratio);

/**
 * @fn int IMP_ISP_Tuning_SetSinterStrength(uint32_t ratio)
 *
 * 设置2D降噪强度。
 *
 * @param[in] ratio 强度调节比例.默认值为128,如果设置大于128则增加强度，小于128降低强度.取值范围为［0-255].
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetSinterStrength(uint32_t ratio);

/**
 * ISP EV 参数。
 */
typedef struct {
	uint32_t ev;			/**< 曝光值 */
	uint32_t expr_us;		/**< 曝光时间 */
	uint32_t ev_log2;		/**<log格式曝光时间 */
	uint32_t again;			/**< 模拟增益 */
	uint32_t dgain;			/**< 数字增益 */
	uint32_t gain_log2;		/**< log格式增益 */
}IMPISPEVAttr;

/**
* @fn int IMP_ISP_Tuning_GetEVAttr(IMPISPEVAttr *attr)
*
* 获取EV属性。
* @param[out] attr EV属性参数
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_GetEVAttr(IMPISPEVAttr *attr);

/**
* @fn int IMP_ISP_Tuning_EnableMovestate(void)
*
* 当sensor在运动时，设置ISP进入运动态。
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_EnableMovestate(void);

/**
* @fn int IMP_ISP_Tuning_DisableMovestate(void)
*
* 当sensor从运动态恢复为静止态，设置ISP不使能运动态。
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_Tuning_EnableMovestate已被调用。
*/
int IMP_ISP_Tuning_DisableMovestate(void);

/**
* 模式选择选项
*/
typedef enum {
	IMPISP_TUNING_MODE_AUTO,    /**< 该模块的操作为自动模式 */
	IMPISP_TUNING_MODE_MANUAL,    /**< 该模块的操作为手动模式 */
	IMPISP_TUNING_MODE_RANGE,    /**< 该模块的操作为设置范围模式 */
	IMPISP_TUNING_MODE_BUTT,    /**< 用于判断参数的有效性，参数大小必须小于这个值 */
} IMPISPTuningMode;

/**
* 权重信息
*/
typedef struct {
	unsigned char weight[15][15];    /**< 各区域权重信息 [0 ~ 8]*/
} IMPISPWeight;

/**
 * @fn int IMP_ISP_Tuning_SetAeWeight(IMPISPWeight *ae_weight)
 *
 * 设置AE统计区域的权重。
 *
 * @param[in] ae_weight 各区域权重信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAeWeight(IMPISPWeight *ae_weight);

/**
 * @fn int IMP_ISP_Tuning_GetAeWeight(IMPISPWeight *ae_weight)
 *
 * 获取AE统计区域的权重。
 *
 * @param[out] ae_weight 各区域权重信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAeWeight(IMPISPWeight *ae_weight);

/**
 * @fn int IMP_ISP_Tuning_AE_GetROI(IMPISPWeight *roi_weight)
 *
 * 获取AE感兴趣区域，用于场景判断。
 *
 * @param[out] roi_weight AE感兴趣区域权重。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_AE_GetROI(IMPISPWeight *roi_weight);

/**
 * @fn int IMP_ISP_Tuning_AE_SetROI(IMPISPWeight *roi_weight)
 *
 * 获取AE感兴趣区域，用于场景判断。
 *
 * @param[in] roi_weight AE感兴趣区域权重。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_AE_SetROI(IMPISPWeight *roi_weight);

/**
 * @fn int IMP_ISP_Tuning_SetAwbWeight(IMPISPWeight *awb_weight)
 *
 * 设置AWB统计区域的权重。
 *
 * @param[in] awb_weight 各区域权重信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAwbWeight(IMPISPWeight *awb_weight);

/**
 * @fn int IMP_ISP_Tuning_GetAwbWeight(IMPISPWeight *awb_weight)
 *
 * 获取AWB统计区域的权重。
 *
 * @param[out] awb_weight 各区域权重信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAwbWeight(IMPISPWeight *awb_weight);

/**
* AWB统计值
*/
typedef struct {
	unsigned char zone_r[225];    /**< 15*15块，RGB三个通道在每个块的亮度统计平均值*/
	unsigned char zone_g[225];    /**< 15*15块，RGB三个通道在每个块的亮度统计平均值*/
	unsigned char zone_b[225];    /**< 15*15块，RGB三个通道在每个块的亮度统计平均值*/
} IMPISPAWBZone;
/**
 * @fn int IMP_ISP_Tuning_GetAwbZone(IMPISPAWBZONE *awb_zone)
 *
 * 获取WB在每个块，不同通道的统计平均值。
 *
 * @param[out] awb_zone 白平衡统计信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAwbZone(IMPISPAWBZone *awb_zone);

/**
 * AWB algorithm
 */
typedef enum {
	IMPISP_AWB_ALGO_NORMAL = 0, /*常规模式，用有效点来做统计*/
	IMPISP_AWB_ALGO_GRAYWORLD, /*灰色世界模式，所有像素点都用来做统计*/
	IMPISP_AWB_ALGO_REWEIGHT, /*偏向模式，不同色温重新设置权重*/
} IMPISPAWBAlgo;

/**
 * @fn int IMP_ISP_Tuning_SetWB_ALGO(IMPISPAWBALGO wb_algo)
 *
 * 设置AWB统计的模式。
 *
 * @param[in] wb_algo AWB统计的不同模式。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */

int IMP_ISP_Tuning_SetWB_ALGO(IMPISPAWBAlgo wb_algo);

/**
* AE统计值参数
*/
typedef struct {
	unsigned char ae_histhresh[4];    /**< AE统计直方图bin边界 [0 ~ 255]*/
	unsigned short ae_hist[5];    /**< AE统计直方图bin值 [0 ~ 65535]*/
	unsigned char ae_stat_nodeh;    /**< 水平方向有效统计区域个数 [0 ~ 15]*/
	unsigned char ae_stat_nodev;    /**< 垂直方向有效统计区域个数 [0 ~ 15]*/
} IMPISPAEHist;

/**
 * AE统计值参数
 */
typedef struct {
	unsigned int ae_hist[256];    /**< AE统计直方图256 bin值*/
} IMPISPAEHistOrigin;

/**
 * @fn int IMP_ISP_Tuning_SetAeHist(IMPISPAEHist *ae_hist)
 *
 * 设置AE统计相关参数。
 *
 * @param[in] ae_hist AE统计相关参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAeHist(IMPISPAEHist *ae_hist);

/**
 * @fn int IMP_ISP_Tuning_GetAeHist(IMPISPAEHist *ae_hist)
 *
 * 获取AE统计值。
 *
 * @param[out] ae_hist AE统计值信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAeHist(IMPISPAEHist *ae_hist);

/**
 * @fn int IMP_ISP_Tuning_GetAeHist_Origin(IMPISPAEHistOrigin *ae_hist)
 *
 * 获取AE 256 bin统计值。
 *
 * @param[out] ae_hist AE统计值信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAeHist_Origin(IMPISPAEHistOrigin *ae_hist);

/**
* AWB统计值
*/
struct isp_core_awb_sta_info{
	unsigned short r_gain;    /**< AWB加权r/g平均值 [0 ~ 4095]*/
	unsigned short b_gain;    /**< AWB加权b/g平均值 [0 ~ 4095]*/
	unsigned int awb_sum;    /**< 用于AWB统计的像素数 [0 ~ 4294967295]*/
};
/**
* AWB统计模式
*/
enum isp_core_awb_stats_mode{
	IMPISP_AWB_STATS_LEGACY_MODE = 0,    /**< 延迟模式 */
	IMPISP_AWB_STATS_CURRENT_MODE = 1,    /**< 当前模式 */
	IMPISP_AWB_STATS_MODE_BUTT,
};
/**
* AWB统计值参数
*/
typedef struct {
	struct isp_core_awb_sta_info awb_stat;    /**< AWB统计值 */
	enum isp_core_awb_stats_mode awb_stats_mode;    /**< AWB统计模式 */
	unsigned short awb_whitelevel;    /**< AWB统计数值上限 [0 ~ 1023]*/
	unsigned short awb_blacklevel;    /**< AWB统计数值下限 [0 ~ 1023]*/
	unsigned short cr_ref_max;    /**< AWB统计白点区域r/g最大值 [0 ~ 4095]*/
	unsigned short cr_ref_min;    /**< AWB统计白点区域r/g最小值 [0 ~ 4095]*/
	unsigned short cb_ref_max;    /**< AWB统计白点区域b/g最大值  [0 ~ 4095]*/
	unsigned short cb_ref_min;    /**< AWB统计白点区域b/g最大值  [0 ~ 4095]*/
	unsigned char awb_stat_nodeh;    /**< 水平方向有效统计区域个数 [0 ~ 15]*/
	unsigned char awb_stat_nodev;    /**< 垂直方向有效统计区域个数 [0 ~ 15]*/
} IMPISPAWBHist;

/**
 * @fn int IMP_ISP_Tuning_GetAwbHist(IMPISPAWBHist *awb_hist)
 *
 * 获取AWB统计值。
 *
 * @param[out] awb_hist AWB统计值信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAwbHist(IMPISPAWBHist *awb_hist);

/**
 * @fn int IMP_ISP_Tuning_SetAwbHist(IMPISPAWBHist *awb_hist)
 *
 * 设置AWB统计相关参数。
 *
 * @param[in] awb_hist AWB统计相关参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAwbHist(IMPISPAWBHist *awb_hist);

/**
* AF统计值
*/
struct isp_core_af_sta_info{
	unsigned int af_metrics;/**< AF主统计值*/
	unsigned int af_metrics_alt;/**< AF次统计值*/
};
/**
* AF统计值参数
*/
typedef struct {
	struct isp_core_af_sta_info af_stat;    /**< AF统计值信息 */
	unsigned char af_enable;    /**< AF功能开关*/
	unsigned char af_metrics_shift;    /**< AF统计值缩小参数 默认是0，1代表缩小2倍*/
	unsigned short af_delta;    /**< AF统计低通滤波器的权重 [0 ~ 64]*/
	unsigned short af_theta;    /**< AF统计高通滤波器的权重 [0 ~ 64]*/
	unsigned short af_hilight_th;    /**< AF高亮点统计阈值 [0 ~ 255]*/
	unsigned short af_alpha_alt;    /**< AF统计低通滤波器的水平与垂直方向的权重 [0 ~ 64]*/
	unsigned char  af_hstart;    /**< AF统计值横向起始点：[1-width]，且取奇数*/
	unsigned char  af_vstart;    /**< AF统计值垂直起始点 ：[3-height]，且取奇数
	unsigned char  af_stat_nodeh;    /**< 水平方向统计区域个数 [1 ~ 15]，整个画幅的统计窗口H数目 */
	unsigned char  af_stat_nodev;    /**< 垂直方向统计区域个数 [1 ~ 15]，整个画幅的统计窗口V数目 */
} IMPISPAFHist;

/**
 * @fn IMP_ISP_Tuning_GetAFMetrices(unsigned int *metric);
 *
 * 获取AF统计值。
 *
 * @param[out] metric AF统计值信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAFMetrices(unsigned int *metric);

/**
 * @fn int IMP_ISP_Tuning_GetAfHist(IMPISPAFHist *af_hist);
 *
 * 获取AF统计值。
 *
 * @param[out] af_hist AF统计值信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAfHist(IMPISPAFHist *af_hist);

/**
 * @fn int IMP_ISP_Tuning_SetAfHist(IMPISPAFHist *af_hist)
 *
 * 设置AF统计相关参数。
 *
 * @param[in] af_hist AF统计相关参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAfHist(IMPISPAFHist *af_hist);
/**
 * @fn int IMP_ISP_Tuning_SetAfWeight(IMPISPWeight *af_weight)
 *
 * 设置AF统计区域的权重。
 *
 * @param[in] af_weight 各区域权重信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAfWeight(IMPISPWeight *af_weigh);
/**
 * @fn int IMP_ISP_Tuning_GetAfWeight(IMPISPWeight *af_weight)
 *
 * 获取AF统计区域的权重。
 *
 * @param[out] af_weight 各区域权重信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAfWeight(IMPISPWeight *af_weight);

/**
 * @fn int IMP_ISP_Tuning_GetAfZone(IMPISPZone *af_zone)
 *
 * 获取AF各个zone的统计值。
 *
 * @param[out] af_zone AF各个区域的统计值。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAfZone(IMPISPZone *af_zone);

/**
 * ISP Wait Frame 参数。
 */
typedef struct {
	uint32_t timeout;		/**< 超时时间，单位ms */
	uint64_t cnt;			/**< Frame统计 */
}IMPISPWaitFrameAttr;

/**
* @fn int IMP_ISP_Tuning_WaitFrame(IMPISPWaitFrameAttr *attr)
* 等待帧结束
*
* @param[out] attr 等待帧结束属性
*
* @retval 0 成功
* @retval 非0 失败，返回错误码
*
* @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
*/
int IMP_ISP_Tuning_WaitFrame(IMPISPWaitFrameAttr *attr);

/**
 * AE Min
 */
typedef struct {
	unsigned int min_it;  /**< AE最小曝光 */
	unsigned int min_again;	 /**< AE 最小模拟增益 */
	unsigned int min_it_short; /**< AE短帧的最小曝光 */
	unsigned int min_again_short; /**< AE 短帧的最小模拟增益 */
} IMPISPAEMin;

/**
 * @fn int IMP_ISP_Tuning_SetAeMin(IMPISPAEMin *ae_min)
 *
 * 设置AE最小值参数。
 *
 * @param[in] ae_min AE最小值参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAeMin(IMPISPAEMin *ae_min);

/**
 * @fn int IMP_ISP_Tuning_GetAeMin(IMPISPAEMin *ae_min)
 *
 * 获取AE最小值参数。
 *
 * @param[out] ae_min AE最小值信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAeMin(IMPISPAEMin *ae_min);

/**
 * @fn int IMP_ISP_Tuning_SetAe_IT_MAX(unsigned int it_max)
 *
 * 设置AE最大值参数。
 *
 * @param[in] it_max AE最大值参数。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAe_IT_MAX(unsigned int it_max);

/**
 * @fn int IMP_ISP_Tuning_GetAE_IT_MAX(unsigned int *it_max)
 *
 * 获取AE最大值参数。
 *
 * @param[out] it_max AE最大值信息。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAE_IT_MAX(unsigned int *it_max);

/**
 * @fn int IMP_ISP_Tuning_GetAeZone(IMPISPZone *ae_zone)
 *
 * 获取AE各个zone的Y值。
 *
 * @param[out] ae_zone AE各个区域的Y值。
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAeZone(IMPISPZone *ae_zone);

/**
 * ISP Module Control
 */
typedef union {
	unsigned int key;
	struct {
		unsigned int bitBypassBLC : 1; /* [0]  */
		unsigned int bitBypassGIB : 1; /* [1]  */
		unsigned int bitBypassAG : 1; /* [2]  */
		unsigned int bitBypassWDR : 1; /* [3]  */
		unsigned int bitBypassDPC : 1; /* [4]  */
		unsigned int bitBypassRDNS : 1; /* [5]	*/
		unsigned int bitBypassLSC : 1; /* [6]  */
		unsigned int bitBypassADR : 1; /* [7]	 */
		unsigned int bitBypassDMSC : 1; /* [8]	 */
		unsigned int bitBypassCCM : 1; /* [9]  */
		unsigned int bitBypassGAMMA : 1; /* [10]  */
		unsigned int bitBypassDEFOG : 1; /* [11]	 */
		unsigned int bitBypassCSC : 1; /* [12]	 */
		unsigned int bitBypassCLM : 1; /* [13]	 */
		unsigned int bitBypassSP : 1; /* [14]  */
		unsigned int bitBypassYDNS : 1; /* [15]	 */
		unsigned int bitBypassBCSH : 1; /* [16]	 */
		unsigned int bitBypassSDNS : 1; /* [17]	 */
		unsigned int bitBypassHLDC : 1; /* [18]	 */
		unsigned int bitRsv : 12; /* [19 ~ 30]	*/
		unsigned int bitBypassMDNS : 1; /* [31]  */
	};
} IMPISPModuleCtl;

/**
 * @fn int IMP_ISP_Tuning_SetModuleControl(IMPISPModuleCtl *ispmodule)
 *
 * 设置ISP各个模块bypass功能
 *
 * @param[in] ispmodule ISP各个模块bypass功能.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetModuleControl(IMPISPModuleCtl *ispmodule);

/**
 * @fn int IMP_ISP_Tuning_GetModuleControl(IMPISPModuleCtl *ispmodule)
 *
 * 获取ISP各个模块bypass功能.
 *
 * @param[out] ispmodule ISP各个模块bypass功能
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetModuleControl(IMPISPModuleCtl *ispmodule);

/**
 * ISP Front Crop Attribution
 */
typedef struct {
	bool fcrop_enable;
	unsigned int fcrop_top;
	unsigned int fcrop_left;
	unsigned int fcrop_width;
	unsigned int fcrop_height;
} IMPISPFrontCrop;

/**
 * @fn int IMP_ISP_Tuning_SetFrontCrop(IMPISPFrontCrop *ispfrontcrop)
 *
 * 设置ISP前Crop的位置
 *
 * @param[in] ispfrontcrop 前Crop参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetFrontCrop(IMPISPFrontCrop *ispfrontcrop);

/**
 * @fn int IMP_ISP_Tuning_GetFrontCrop(IMPISPFrontCrop *ispfrontcrop)
 *
 * 获取前Crop参数.
 *
 * @param[out] ispfrontcrop 前Crop参数
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetFrontCrop(IMPISPFrontCrop *ispfrontcrop);

/**
 * ISP AutoZoom Attribution
 */
typedef struct {
	int chan;
	int scaler_enable;
	int scaler_outwidth;
	int scaler_outheight;
	int crop_enable;
	int crop_left;
	int crop_top;
	int crop_width;
	int crop_height;
} IMPISPAutoZoom;

/**
 * @fn int IMP_ISP_WDR_ENABLE(IMPISPTuningOpsMode mode)
 *
 * 使能ISP WDR.
 *
 * @param[in] mode ISP WDR 模式
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_WDR_ENABLE(IMPISPTuningOpsMode mode);

/**
 * @fn IMP_ISP_WDR_ENABLE_Get(IMPISPTuningOpsMode* mode)
 *
 * 获取ISP WDR 模式.
 *
 * @param[in] mode ISP WDR 模式
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_WDR_ENABLE_Get(IMPISPTuningOpsMode* mode);

/**
 * @fn int IMP_ISP_Tuning_SetDPC_Strength(unsigned int strength)
 *
 * 设置DPC强度.
 *
 * @param[in] strength 强度调节比例.默认值为128,如果设置大于128则增加强度，小于128降低强度.取值范围为［0-255]
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetDPC_Strength(unsigned int ratio);

/**
 * @fn int IMP_ISP_Tuning_GetDPC_Strength(unsigned int *strength)
 *
 * 获取DPC强度.
 *
 * @param[out] strength 强度调节比例.默认值为128,如果设置大于128则增加强度，小于128降低强度.取值范围为［0-255]
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetDPC_Strength(unsigned int *ratio);

/**
 * @fn int IMP_ISP_Tuning_SetDRC_Strength(unsigned int ratio)
 *
 * 设置DRC强度值.
 *
 * @param[in] strength 强度调节比例.默认值为128,如果设置大于128则增加强度，小于128降低强度.取值范围为［0-255]
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetDRC_Strength(unsigned int ratio);

/**
 * @fn int IMP_ISP_Tuning_GetDRC_Strength(unsigned int *ratio)
 *
 * 获取DRC强度值.
 *
 * @param[out] ratio 强度调节比例.默认值为128,如果设置大于128则增加强度，小于128降低强度.取值范围为［0-255]
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetDRC_Strength(unsigned int *ratio);

/**
 * HV Flip 模式
 */
typedef enum {
	IMPISP_FLIP_NORMAL_MODE = 0,	/**< 正常模式 */
	IMPISP_FLIP_H_MODE = 1,	   /**< 镜像模式 */
	IMPISP_FLIP_V_MODE = 2,		/**< 翻转模式 */
	IMPISP_FLIP_HV_MODE = 3,	/**< 镜像并翻转模式 */
	IMPISP_FLIP_MODE_BUTT,
} IMPISPHVFLIP;

/**
 * @fn int IMP_ISP_Tuning_SetHVFLIP(IMPISPHVFLIP hvflip)
 *
 * 设置HV Flip的模式.
 *
 * @param[in] hvflip HV Flip模式.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetHVFLIP(IMPISPHVFLIP hvflip);

/**
 * @fn int IMP_ISP_Tuning_GetHVFlip(IMPISPHVFLIP *hvflip)
 *
 * 获取HV Flip的模式.
 *
 * @param[out] hvflip HV Flip模式.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetHVFlip(IMPISPHVFLIP *hvflip);

/**
 * 填充数据类型
 */
typedef enum {
	IMPISP_MASK_TYPE_RGB = 0, /**< RGB */
	IMPISP_MASK_TYPE_YUV = 1, /**< YUV */
} IMPISP_MASK_TYPE;

/**
 * 填充数据
 */
typedef union mask_value {
	struct {
		unsigned char Red; /**< R 值 */
		unsigned char Green; /**< G 值 */
		unsigned char Blue; /**< B 值 */
	} mask_rgb; /**< RGB*/
	struct {
		unsigned char y_value; /**< Y 值 */
		unsigned char u_value; /**< U 值 */
		unsigned char v_value; /**< V 值 */
	} mask_ayuv; /**< YUV*/
} IMP_ISP_MASK_VALUE;

/**
 * 每个通道的填充属性
 */
typedef struct isp_mask_block_par {
	unsigned char mask_en;/**< 填充使能 */
	unsigned short mask_pos_top;/**< 填充位置y坐标*/
	unsigned short mask_pos_left;/**< 填充位置x坐标  */
	unsigned short mask_width;/**< 填充数据宽度 */
	unsigned short mask_height;/**< 填充数据高度 */
	IMP_ISP_MASK_VALUE mask_value;/**< 填充数据值 */
} IMPISP_MASK_BLOCK_PAR;

/**
 * 填充参数
 */
typedef struct {
	IMPISP_MASK_BLOCK_PAR chn0[4];/**< 通道0填充参数 */
	IMPISP_MASK_BLOCK_PAR chn1[4];/**< 通道1填充参数 */
	IMPISP_MASK_BLOCK_PAR chn2[4];/**< 通道3填充参数 */
	IMPISP_MASK_TYPE mask_type;/**< 填充数据类型 */
} IMPISPMASKAttr;

/**
 * @fn int IMP_ISP_Tuning_SetMask(IMPISPMASKAttr *mask)
 *
 * 设置填充参数.
 *
 * @param[in] mask 填充参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetMask(IMPISPMASKAttr *mask);

/**
 * @fn int IMP_ISP_Tuning_GetMask(IMPISPMASKAttr *mask)
 *
 * 获取填充参数.
 *
 * @param[out] mask 填充参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetMask(IMPISPMASKAttr *mask);

/**
 * Sensor属性参数
 */
typedef struct {
	unsigned int hts;/**< sensor hts */
	unsigned int vts;/**< sensor vts */
	unsigned int fps;/**< sensor 帧率 */
	unsigned int width;/**< sensor输出宽度 */
	unsigned int height;/**< sensor输出的高度 */
} IMPISPSENSORAttr;
/**
 * @fn int IMP_ISP_Tuning_GetSensorAttr(IMPISPSENSORAttr *attr)
 *
 * 获取填充参数.
 *
 * @param[out] attr sensor属性参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetSensorAttr(IMPISPSENSORAttr *attr);

/**
 * @fn int IMP_ISP_Tuning_EnableDRC(IMPISPTuningOpsMode mode)
 *
 * 使能DRC功能.
 *
 * @param[out] mode DRC功能使能参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_EnableDRC(IMPISPTuningOpsMode mode);

/**
 * @fn int IMP_ISP_Tuning_EnableDefog(IMPISPTuningOpsMode mode)
 *
 * 使能Defog功能.
 *
 * @param[out] mode Defog功能使能参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_EnableDefog(IMPISPTuningOpsMode mode);

/**
 * @fn int IMP_ISP_Tuning_SetAwbCt(unsigned int *ct)
 *
 * 设置AWB色温值.
 *
 * @param[in] ct AWB色温值.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAwbCt(unsigned int *ct);

/**
 * @fn int IMP_ISP_Tuning_GetAWBCt(unsigned int *ct)
 *
 * 获取AWB色温值.
 *
 * @param[out] ct AWB色温值.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetAWBCt(unsigned int *ct);

/**
 * ISP 颜色矩阵属性
 */
typedef struct {
	IMPISPTuningOpsMode ManualEn;   /* 手动CCM使能 */
	IMPISPTuningOpsMode SatEn;      /* 手动模式下饱和度使能 */
	float ColorMatrix[9];              /* 颜色矩阵 */
} IMPISPCCMAttr;
/**
 * @fn int IMP_ISP_Tuning_SetCCMAttr(IMPISPCCMAttr *ccm)
 *
 * 设置CCM属性.
 *
 * @param[in] ccm CCM属性参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetCCMAttr(IMPISPCCMAttr *ccm);

/**
 * @fn int IMP_ISP_Tuning_GetCCMAttr(IMPISPCCMAttr *ccm)
 *
 * 获取CCM属性.
 *
 * @param[out] ccm CCM属性参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_GetCCMAttr(IMPISPCCMAttr *ccm);

/**
 * ISP AE 手动模式属性
 */
typedef struct {
	/* 线性模式和WDR模式下长帧的AE 手动模式属性*/
	IMPISPTuningOpsMode AeFreezenEn;    /* AE Freezen使能 */
	IMPISPTuningOpsMode AeItManualEn;	/* AE曝光手动模式使能 */
	unsigned int AeIt;			   /* AE手动模式下的曝光值，单位是曝光行 */
	IMPISPTuningOpsMode AeAGainManualEn;	   /* AE Sensor 模拟增益手动模式使能 */
	unsigned int AeAGain;			      /* AE Sensor 模拟增益值，单位是倍数 x 1024 */
	IMPISPTuningOpsMode AeDGainManualEn;	   /* AE Sensor数字增益手动模式使能 */
	unsigned int AeDGain;			      /* AE Sensor数字增益值，单位是倍数 x 1024 */
	IMPISPTuningOpsMode AeIspDGainManualEn;	      /* AE ISP 数字增益手动模式使能 */
	unsigned int AeIspDGain;			 /* AE ISP 数字增益值，单位倍数 x 1024*/

	/* WDR模式下短帧的AE 手动模式属性*/
	IMPISPTuningOpsMode AeWdrShortFreezenEn;    /* AE Freezen使能 */
	IMPISPTuningOpsMode AeWdrShortItManualEn;	/* AE曝光手动模式使能 */
	unsigned int AeWdrShortIt;			   /* AE手动模式下的曝光值，单位是曝光行 */
	IMPISPTuningOpsMode AeWdrShortAGainManualEn;	    /* AE Sensor 模拟增益手动模式使能 */
	unsigned int AeWdrShortAGain;			      /* AE Sensor 模拟增益值，单位是倍数 x 1024 */
	IMPISPTuningOpsMode AeWdrShortDGainManualEn;	   /* AE Sensor数字增益手动模式使能 */
	unsigned int AeWdrShortDGain;			      /* AE Sensor数字增益值，单位是倍数 x 1024 */
	IMPISPTuningOpsMode AeWdrShortIspDGainManualEn;	      /* AE ISP 数字增益手动模式使能 */
	unsigned int AeWdrShortIspDGain;			 /* AE ISP 数字增益值，单位倍数 x 1024*/
} IMPISPAEAttr;
/**
 * @fn int IMP_ISP_Tuning_SetAeAttr(IMPISPAEAttr *ae)
 *
 * 设置AE手动模式属性.
 *
 * @param[in] ae AE手动模式属性参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 */
int IMP_ISP_Tuning_SetAeAttr(IMPISPAEAttr *ae);

/**
 * @fn int IMP_ISP_Tuning_GetAeAttr(IMPISPAEAttr *ae)
 *
 * 获取AE手动模式属性.
 *
 * @param[out] ae AE手动模式属性参数.
 *
 * @retval 0 成功
 * @retval 非0 失败，返回错误码
 *
 * @attention 在使用这个函数之前，IMP_ISP_EnableTuning已被调用。
 * @attention 在使用这个函数之前，需要先将IMPISPAEAttr结构体初始化为0，然后配置相应的属性。
 */
int IMP_ISP_Tuning_GetAeAttr(IMPISPAEAttr *ae);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/**
 * @}
 */

#endif /* __IMP_ISP_H__ */
