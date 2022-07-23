/*
 * IMP System header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_SYSTEM_H__
#define __IMP_SYSTEM_H__

#include "imp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP系统模块头文件
 */

/**
 * @defgroup imp IMP(Ingenic Media Platform)
 */

/**
 * @defgroup IMP_System
 * @ingroup imp
 * @brief 系统控制模块，包括IMP的基础功能以及模块间绑定的相关功能
 * @section concept 1 相关概念
 * 系统控制主要实现连接各模块，定义数据流的功能。以下是一些重要概念：
 *
 * @subsection device 1.1 Device
 * Device是完成某一（类）功能的集合。如FrameSource完成视频源数据的输出，Encoder完成视频编码或者图像编码的功能。这里的FrameSource和Encoder就是Device的概念。\n
 * Device只是一个集合的概念，并不是具体的数据流节点。
 *
 * @subsection group 1.2 Group
 * Group是一路数据输入的最小单位。一个Device可以有多个Group，每个Group只能接收一路数据输入。Group可以有多路输出（@ref output ）。\n
 * Group也是具体”功能“的容器，可以详见@ref channel 部分的解释。
 *
 * @subsection output 1.3 Output
 * Output是一个Group的一路数据输出的最小单位。一个Group可以有多个Output，每个Output只能输出一路数据。
 *
 * @subsection cell 1.4 Cell
 * Cell指包含了Device、Group、Output信息的集合。以@ref IMPCell的数据结构呈现。\n
 * Cell主要用来Bind（@ref bind）。根据Device、Group、Output的定义，Output作为数据输出的节点，而Group作为数据输入的节点。\n
 * 在Bind时数据输出节点的Cell索引到输出的Output，数据输入节点的Cell索引到输入的Group（因此作为数据输入的Cell，Output是一个无意义值）。
 *
 * @subsection channel 1.5 Channel
 * Channel通常指一个单一功能的单元，Channel在Create时(实例化)时被指定具体功能。\n
 * 例如：\n
 * * 对于Encoder，一个Channel完成一路H264编码或者JPEG编码的功能，具体的编码功能(类型，参数)在通道创建时指定
 * * 对于IVS，一个Channel完成一个具体的算法的功能，具体的算法类型参数在通道创建时指定
 * * 对于OSD，有一个同Channel类似的概念Region，一个Region是一个具体的叠加区域，可以是PIC(图像)，COVER(遮挡)等
 * * 对于FrameSource，一个Channel输出一路原始图像，FrameSource的Channel实际上就是Group
 *
 * Channel作为功能单元，通常需要Register到Group中(FrameSource除外)，才能接收到数据。Channel注册到Group中后，会得到Group输入的数据。\n
 * 不同Device的Group可Register的Channel数也不同。
 *
 * @section bind 2 模块绑定（Bind）
 * 两个Group经过Bind连接后，源Group的数据会自动发到目的Group。\n
 * 由于Group是数据输入的最小单元，Output是数据输出的最小单元，因此IMP_System_Bind(IMPCell *srcCell, IMPCell *dstCell)的两个参数中srcCell的deviceID, groupID, outputID是有效的
 * 而dstCell仅deviceID和groupID有效，outputID作为数据输入是无意义的。\n
 * 下图是一个简单Bind的例子。
 * @image html system_bind0.jpg
 * 在上图中，实现了FrameSource的一路输出Bind到Encoder的一个Group。
 * 在Encoder Group中Register了两个Channel，因此Encoder Group有H264和JPEG两路输出。
 * 参考代码：
 * @code
 * IMPCell fs_chn0 = {DEV_ID_FS, 0, 0};			//FrameSource deviceID:DEV_ID_FS groupID:0 outputID:0
 * IMPCell enc_grp0 = {DEV_ID_ENC, 0, 0};		//ENC deviceID:DEV_ID_ENC groupID:0 outputID:0, 这里enc_grp0的第三个参数无意义。
 * int ret = IMP_System_Bind(&fs_chn0, &enc_grp0);
 * if (ret < 0)
 *     printf("Bind FrameSource Channel0 and Encoder Group0 failed\n");
 * @endcode
 * Bind将系统的数据流串接起来，根据不同的产品功能需求，Bind的策略也可能不同。
 *
 * 以下是典型双路码流产品应用Bind的示意图：
 *
 * @image html typical_application.png
 *
 * 上图中，FrameSource有两路输出，分别是Channel0主码流(1280x720)和Channel1从码流(640x360)。\n
 * * 主码流：FrameSource的Channel0 Bind OSD Group.0，OSD Group.0 Bind Encoder Group.0。其中：
 *   * OSD Group.0 注册了两个Region，分别用来显示时间戳和字符串信息
 *   * Encoder Group.0 注册了两个Channel，分别进行H264编码和JPEG编码。其中JPEG编码通道的图像大小如果不等于输入设置（FrameSource的Channel0），那么就会进行缩放(Software at T10)，达到任意分辨率抓拍的目的
 * * 从码流：FrameSource的Channel1 Bind IVS Group.0，IVS Group.0 Bind OSD Group.1，OSD Group.1 Bind Encoder Group.1。其中：
 *   * IVS Group.0 注册了一个Channel，用来进行移动侦测
 *   * OSD Group.1 注册了两个Region，分别用来显示时间戳和字符串信息
 *   * Encoder Group.1 注册了一个Channel，进行H264编码
 *   * 这里值得注意的一点是，IVS Bind 在 OSD之前，是因为OSD的时间戳可能造成IVS移动侦测的误判
 *
 * 参考代码：\n
 * 主码流数据流Bind：
 * @code
 * IMPCell fs_chn0 = {DEV_ID_FS, 0, 0};
 * IMPCell osd_grp0 = {DEV_ID_OSD, 0, 0};
 * IMPCell enc_grp0 = {DEV_ID_ENC, 0, 0};
 * int ret = IMP_System_Bind(&fs_chn0, &osd_grp0);
 * if (ret < 0)
 *     printf("Bind FrameSource Channel0 and OSD Group0 failed\n");
 *
 * int ret = IMP_System_Bind(&osd_grp0, &enc_grp0);
 * if (ret < 0)
 *     printf("Bind OSD Group0 and Encoder Group0 failed\n");
 * @endcode
 * 从码流数据流Bind：
 * @code
 * IMPCell fs_chn1_output0 = {DEV_ID_FS, 1, 0};
 * IMPCell ivs_grp0 = {DEV_ID_IVS, 0, 0};
 * IMPCell osd_grp1 = {DEV_ID_OSD, 1, 0};
 * IMPCell enc_grp1 = {DEV_ID_ENC, 1, 0};
 *
 * int ret = IMP_System_Bind(&fs_chn1_output0, &ivs_grp0);
 * if (ret < 0)
 *     printf("Bind FrameSource Channel1 and IVS Group0 failed\n");
 *
 * int ret = IMP_System_Bind(&ivs_grp0, &osd_grp1);
 * if (ret < 0)
 *     printf("Bind IVS Group0 and OSD Group1 failed\n");
 *
 * int ret = IMP_System_Bind(&osd_grp1, &enc_grp1);
 * if (ret < 0)
 *     printf("Bind OSD Group1 and Encoder Group1 failed\n");
 *
 * @endcode
 *
 * @attention 建议所有的Bind的操作在系统初始化时进行。
 * @attention 在FrameSource使能后Bind和UnBind操作不能动态调用，需要Disable FrameSource后才可进行UnBind。
 * @attention DestroyGroup要在UnBind之后才能进行。
 *
 * Bind可以呈树状结构，下图是一个例子：
 * @image html different_output.png
 * 上图中，FrameSource的Channel 1(Group.1)后端分别Bind了两个Group，分别从Output.0和 Output.1输出数据。本例中这样Bind的好处是，IVS Group可以与OSD Group.1并行工作。
 * @attention 此例中的Bind方式可能对普通移动侦测造成影响，因此普通移动侦测不建议采用这种方式。
 *
 * @{
 */

/**
 * IMP系统版本号定义.
 */
typedef struct {
	char aVersion[64];	/**< IMP系统版本号 */
} IMPVersion;

/**
 * @fn int IMP_System_Init(void)
 *
 * IMP系统初始化.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 此API调用后会初始化基础的数据结构，但不会初始化硬件单元.
 *
 * @attention 在IMP的任何操作之前必须先调用此接口进行初始化.
 */
int IMP_System_Init(void);

/**
 * @fn int IMP_System_Exit(void)
 *
 * IMP系统去初始化.
 *
 * @param 无.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 此函数调用后会释放IMP所有的内存以及句柄，并关闭硬件单元.
 *
 * @attention 在调用此API后，若要再次使用IMP则需重新进行IMP系统初始化.
 */
int IMP_System_Exit(void);

/**
 * @fn int64_t IMP_System_GetTimeStamp(void)
 *
 * 获得IMP系统的时间戳，单位为微秒。
 *
 * @param 无。
 *
 * @retval 时间(usec)
 *
 * @remarks 系统初始化后时间戳自动被初始化。系统去初始化后时间戳失效。
 *
 * @attention 无。
 */
int64_t IMP_System_GetTimeStamp(void);

/**
 * @fn int IMP_System_RebaseTimeStamp(int64_t basets)
 *
 * 设置IMP系统的时间戳，单位为微秒。
 *
 * @param[in] basets 基础时间。
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无。
 *
 * @attention 无。
 */
int IMP_System_RebaseTimeStamp(int64_t basets);

/**
 * @fn uint32_t IMP_System_ReadReg32(uint32_t u32Addr)
 *
 * 读取32位寄存器的值。
 *
 * @param[in] regAddr 寄存器的物理地址。
 *
 * @retval 寄存器的值（32位）
 *
 * @remarks 无。
 *
 * @attention 无。
 */
uint32_t IMP_System_ReadReg32(uint32_t regAddr);

/**
 * @fn void IMP_System_WriteReg32(uint32_t regAddr, uint32_t value)
 *
 * 向32位寄存器中写值。
 *
 * @param[in] regAddr 寄存器的物理地址。
 * @param[in] value 要写入的值。
 *
 * @retval 无
 *
 * @remarks 无。
 *
 * @attention 在不明确寄存器的含义之前请谨慎调用此API，否则可能会导致系统错误。
 */
void IMP_System_WriteReg32(uint32_t regAddr, uint32_t value);

/**
 * @fn int IMP_System_GetVersion(IMPVersion *pstVersion)
 *
 * 获取IMP系统版本号.
 *
 * @param[out] pstVersion IMP系统版本号结构体指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_System_GetVersion(IMPVersion *pstVersion);

/**
 * @fn const char* IMP_System_GetCPUInfo(void)
 *
 * 获取CPU型号信息.
 *
 * @param 无.
 *
 * @retval CPU型号字符串.
 *
 * @remarks 返回值是CPU型号类型的字符串,例如对于T10来说,有"T10"及"T10-Lite".
 *
 * @attention 无.
 */
const char* IMP_System_GetCPUInfo(void);

/**
 * @fn int IMP_System_Bind(IMPCell *srcCell, IMPCell *dstCell)
 *
 * 绑定源Cell和目的Cell.
 *
 * @param[in] srcCell 源Cell指针.
 * @param[in] dstCell 目的Cell指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 根据Device、Group和Output的概念，每个Device可能有多个Group，每个Group可能有多个Output，
 * Group作为Device的输入接口，而Output作为Device的输出接口.因此绑定实际上是将输出Device的某
 * 个Output连接到输入Device的某个Group上.
 * @remarks 绑定关系成功后，源Cell(Output)产生的数据会自动传送到目的Cell(Group).
 *
 * @attention 无。
 */
int IMP_System_Bind(IMPCell *srcCell, IMPCell *dstCell);

/**
 * @fn int IMP_System_UnBind(IMPCell *srcCell, IMPCell *dstCell)
 *
 * 解除源Cell和目的Cell的绑定.
 *
 * @param[in] srcCell 源Cell指针.
 * @param[in] dstCell 目的Cell指针.
 *
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无。
 */
int IMP_System_UnBind(IMPCell *srcCell, IMPCell *dstCell);

/**
 * @fn int IMP_System_GetBindbyDest(IMPCell *dstCell, IMPCell *srcCell)
 *
 * 获取绑定在目的Cell的源Cell信息.
 *
 * @param[in] dstCell 目的Cell指针.
 * @param[out] srcCell 源Cell指针.
 *
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无。
 */
int IMP_System_GetBindbyDest(IMPCell *dstCell, IMPCell *srcCell);

/**
 * @brief IMP_System_MemPoolRequest(int poolId, size_t size, const char *name);
 *
 * 在Rmem上申请mempool
 *
 * @param[in] poolId     申请的poolID.
 * @param[in] size		 申请大小.
 * @param[in] name		 pool名称.
 *
 * @retval    0			 成功.
 * @retval	  非0		 失败.
 *
 * @remarks
 * 视频内存池主要向媒体业务提供大块物理连续内存管理功能,负责内存的分配和回收,
 * 充分发挥内存缓存池的作用,让物理内存资源在各个媒体处理模块中合理使用.
 * 视频内存池建立在原保留内存RMEM（不由内核建立页表）的基础上进行大块内存管理,
 * 每申请一个内存池都是物理连续的内存，之后在该内存池上申请内存同样是申请物理连续的内存.
 * 若使用内存池，必须在系统初始化之前配置内存池大小，根据业务的不同,
 * 内存池申请的大小和数量不同。
 *
 * @attention 无.
 */
int IMP_System_MemPoolRequest(int poolId, size_t size, const char *name);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_SYSTEM_H__ */
