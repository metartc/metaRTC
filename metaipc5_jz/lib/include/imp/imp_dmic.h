#ifndef __IMP_DMIC_H
#define __IMP_DMIC_H

#include <stdint.h>
#include "imp_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

typedef enum {
	DMIC_SAMPLE_RATE_8000 = 8000, /**8KHz 采样率*/
	DMIC_SAMPLE_RATE_16000 = 16000, /*16KHz 采样率*/
} IMPDmicSampleRate;

typedef enum {
	DMIC_BIT_WIDTH_16 = 16, /**<16 bit 采样精度*/
} IMPDmicBitWidth;

typedef enum {
	DMIC_SOUND_MODE_MONO = 1,       /*单声道*/
	DMIC_SOUND_MODE_STEREO = 2,		/*立体音*/
} IMPDmicSoundMode;

/*DMIC 输入设备属性*/
typedef struct {
	IMPDmicSampleRate samplerate; /**< DMIC采样率 */
	IMPDmicBitWidth bitwidth;    /**<DMIC采样精度 */
	IMPDmicSoundMode soundmode;   /*声音模式*/
	int frmNum;					/**<DMIC录音缓存帧的数目*/
	int numPerFrm;
	int chnCnt;  //支持的最大声道数目;
} IMPDmicAttr;

/**
 DMIC 音频帧结构体.
 */
typedef struct {
	IMPDmicBitWidth bitwidth;
	IMPDmicSoundMode soundmode;
	uint32_t *virAddr;
	uint32_t phyAddr;
	int64_t timeStamp;
	int seq;
	int len;
} IMPDmicFrame;

typedef struct {
	IMPDmicFrame rawFrame;  //原始数据;
	IMPDmicFrame aecFrame;  //选取其中一路AEC处理后的数据;
} IMPDmicChnFrame;

/*DMIC 通道参数结构体*/
typedef struct {
	int usrFrmDepth;  /**<DMIC 音频帧缓存深度*/
	int Rev;		  /*保留*/
} IMPDmicChnParam;

typedef struct {
	int dmic_id;
	int dmic_en;
} DmicXInfo;

/**
@int IMP_DMIC_SetUserInfo(int dmicDevId, int aecDmicId, int need_aec);
 *设置麦克阵列的用户需求相关的信息；
 * @param[in] dmicDevId 麦克阵列设备号.
 * @param[in] aecDmicId 用户需要做回声消除处理的麦克的编号;
 * @param[in] need_aec	用户是否需要做回声消除处理(need_aec: 0:不需要 1：需要)
 * @retval 0 成功.
 * @retval 非0 失败.
 */
int IMP_DMIC_SetUserInfo(int dmicDevId, int aecDmicId, int need_aec);

/**
@fn int IMP_DMIC_SetPubAttr(int dmicDevId, IMPDmicAttr *attr);
 *设置麦克阵列音频输入设备属性;
 * @param[in] dmicDevId 麦克阵列设备号.
 * @param[in] attr 麦克阵列音频设备熟悉指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @attention 需要在IMP_DMIC_Enable前调用;
 */
int IMP_DMIC_SetPubAttr(int dmicDevId, IMPDmicAttr *attr);

/**
@fn int IMP_DMIC_GetPubAttr(int dmicDevId, IMPDmicAttr *attr);
 *
 * 获取麦克阵列音频输入设备属性;
 * @param[in] dmicDevId 麦克阵列设备号.
 * @param[in] attr 麦克阵列音频设备熟悉指针.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_DMIC_GetPubAttr(int dmicDevId, IMPDmicAttr *attr);

/**
@fn int IMP_DMIC_Enable(int dmicDevId);
 *
 * 启用麦克阵列音频输入设备;
 * @param[in] dmicDevId 麦克阵列设备号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_DMIC_Enable(int dmicDevId);

/**
@fn int IMP_DMIC_Disable(int dmicDevId);
 *
 * 禁用麦克阵列音频输入设备;
 * @param[in] dmicDevId 麦克阵列设备号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_DMIC_Disable(int dmicDevId);


/**
@fn int IMP_DMIC_EnableChn(int dmicDevId, int dmicChnId);
 *
 * 启用麦克阵列音频输入通道;
 * @param[in] dmicDevId 麦克阵列设备号.
 * @param[in] dmicChnId 麦克阵列音频输入通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 必须先使能麦克阵列device。
 */
int IMP_DMIC_EnableChn(int dmicDevId, int dmicChnId);

/**
@fn int IMP_DMIC_DisableChn(int dmicDevId, int dmicChnId);
 *
 * 禁用麦克阵列音频输入通道;
 * @param[in] dmicDevId 麦克阵列设备号.
 * @param[in] dmicChnId 麦克阵列音频输入通道号.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 与IMP_DMIC_EnableChn 配套使用;
 */
int IMP_DMIC_DisableChn(int dmicDevId, int dmicChnId);

/**
@fn int IMP_DMIC_SetChnParam(int dmicDevId, int dmicChnId, IMPDmicChnParam *chnParam);
 *
 * 设置麦克阵列音频输入通道参数.
 * @param[in] dmicDevId 麦克阵列设备号.
 * @param[in] dmicChnId 麦克阵列音频输入通道号.
 * @param[in] chnParam 麦克阵列音频通道参数.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 在IMP_DMIC_EnableChn 前调用.
 */
int IMP_DMIC_SetChnParam(int dmicDevId, int dmicChnId, IMPDmicChnParam *chnParam);

/**
@fn int IMP_DMIC_GetChnParam(int dmicDevId, int dmicChnId, IMPDmicChnParam *chnParam);
 *
 * 获取麦克阵列音频输入通道参数.
 * @param[in] dmicDevId 麦克阵列设备号.
 * @param[in] dmicChnId 麦克阵列音频输入通道号.
 * @param[in] chnParam 麦克阵列音频通道参数.
 *
 * @retval 0 成功.
 * @retval 非0 失败.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
int IMP_DMIC_GetChnParam(int dmicDevId, int dmicChnId, IMPDmicChnParam *chnParam);

/**
* @fn int IMP_DMIC_GetFrame(int dmicDevId, int dmicChnId, IMPDmicChnFrame *chnFrm, IMPBlock block);
*
* 获取麦克阵列音频帧.
*
* @param[in] dmicDevId 麦克阵列音频设备号.
* @param[in] dmicChnId 麦克阵列音频输入通道号.
* @param[out] chnFrm   麦克阵列音频通道音频帧结构体指针.
* @param[in] block 阻塞/非阻塞标识.
*
* @retval 0 成功.
* @retval 非0 失败.
*
* @remarks 无.
*/
int IMP_DMIC_GetFrame(int dmicDevId, int dmicChnId, IMPDmicChnFrame *chnFrm, IMPBlock block);

/**
* @fn int IMP_DMIC_ReleaseFrame(int dmicDevId, int dmicChnId, IMPDmicChnFrame *chnFrm);
*
* 释放麦克阵列音频通道音频帧.
*
* @param[in] dmicDevId 麦克阵列音频设备号.
* @param[in] dmicChnId 麦克阵列音频输入通道号.
* @param[out] chnFrm   麦克阵列音频通道音频帧结构体指针.
*
* @retval 0 成功.
* @retval 非0 失败.
*
* @remarks 无.
*
* @attention 与IMP_DMIC_GetFrame 配套使用.
*/
int IMP_DMIC_ReleaseFrame(int dmicDevId, int dmicChnId, IMPDmicChnFrame *chnFrm);


/**
* @fn int IMP_DMIC_EnableAecRefFrame(int dmicDevId, int dmicChnId, int audioAoDevId, int aoChn);
*
* 打开获取参考帧.
*
* @param[in] dmicDevId 麦克阵列音频设备号.
* @param[in] dmicChnId 麦克阵列音频输入通道号.
* @param[out] audioAoDevId 音频输出设备号.
*
* @retval 0 成功.
* @retval 非0 失败.
*
* @remarks 调用IMP_DMIC_GetFrameAndRef 之前调用.
*
* @attention 无.
*/
int IMP_DMIC_EnableAecRefFrame(int dmicDevId, int dmicChnId, int audioAoDevId, int aoChn);

/**
* @fn int IMP_AI_GetFrameAndRef(int audioDevId, int aiChn, IMPAudioFrame *frm, IMPAudioFrame *ref, IMPBlock block)
*
* 获取音频帧和输出参考帧.
*
* @param[in]  dmicDevId 麦克阵列音频设备号.
* @param[in]  dmicChnId 麦克阵列音频输入通道号.
* @param[out] chnFrm    麦克阵列音频通道音频帧结构体指针.
* @param[out] ref		参考帧结构体指针.
* @param[in] block 阻塞/非阻塞标识.
*
* @retval 0 成功.
* @retval 非0 失败.
**/
int IMP_DMIC_GetFrameAndRef(int dmicDevId, int dmicChnId, IMPDmicChnFrame *chnFrm, IMPDmicFrame *ref, IMPBlock block);

/**
 ** @fn int IMP_AI_EnableAec(int aiDevId, int aiChn, int aoDevId, int aoChn)
 *
 * 启用指定音频输入和音频输出的回声抵消功能.
 *
 * @param[in] dmicDevId 需要进行回声抵消的麦克阵列音频输入设备号.
 * @param[in] dmicChnId 需要进行回声抵消的麦克阵列音频输入通道号.
 * @param[in] aoDevId 需要进行回声抵消的音频输出设备号.
 * @param[in] aoChn 需要进行回声抵消的音频输出通道号.
 * @retval 0 成功.
 * @retval 非0 失败.
 */
int IMP_DMIC_EnableAec(int dmicDevId, int dmicChnId, int aoDevId, int aoChId);

/**
  * @fn int IMP_DMIC_PollingFrame(int dmicDevId, int dmicChnId, unsigned int timeout_ms);
  * Polling音频流缓存.
  * @param[in] dmicDevId 麦克阵列音频设备号.
  * @param[in] dmicChnId 麦克阵列音频输入通道号.
  * @param[in] timeout_ms Polling超时时间.
  *
  * @retval 0 成功.
  * @retval 非0 失败.
  *
  * @remarks 无.
  *
  * @attention 在使用IMP_DMIC_GetChnParam之前使用该接口，当该接口调用成功之后表示音频
  *数据已经准备完毕，可以使用IMP_DMIC_GetFrame获取音频数据.
  */
int IMP_DMIC_DisableAec(int dmicDevId, int dmicChnId);

/**
  * @fn int IMP_DMIC_DisableAec(int dmicDevId, int dmicChnId);
  * 关闭DMIC的AEC功能.
  * @param[in] dmicDevId 麦克阵列音频设备号.
  * @param[in] dmicChnId 麦克阵列音频输入通道号.
  *
  * @retval 0 成功.
  * @retval 非0 失败.
  *
  * @remarks 无.
  *
  * @attention IMP_DMIC_DisableAec要和IMP_DMIC_EnableAec配对使用
  */
int IMP_DMIC_PollingFrame(int dmicDevId, int dmicChnId, unsigned int timeout_ms);

/**
*   @fn int IMP_DMIC_SetVol(int dmicDevId, int dmicChnId, int dmicVol);
*   Set 麦克阵列的音量大小.
*
*	@param[in] dmicDevId 麦克阵列音频设备号.
*	@param[in] dmicChnId 麦克阵列音频输入通道号.
*	@param[in] dmicVol   麦克阵列音频输入音量大小.

*   @retval 0 成功.
*   @retval 非 0 失败.
*
*   @remarks 音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.
*   @remarks 其中60是音量设置的一个临界点，在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.                                    5dB；当音量值大于60时，上增加1，音量增加0.5dB。
*/
int IMP_DMIC_SetVol(int dmicDevId, int dmicChnId, int dmicVol);

/**
*   @fn int IMP_DMIC_GetVol(int dmicDevId, int dmicChnId, int *dmicVol);
*   获取麦克阵列的音量大小
*
*	@param[in] dmicDevId 麦克阵列音频设备号.
*	@param[in] dmicChnId 麦克阵列音频输入通道号.
*	@param[out] dmicVol   麦克阵列音频输入音量大小.

*   @retval 0 success.
*   @retval non-0 failure.

*   @remarks no.
*   @attention no.
*/
int IMP_DMIC_GetVol(int dmicDevId, int dmicChnId, int *dmicVol);

/**
*  @fn int IMP_DMIC_SetGain(int dmicDevId, int dmicChnId, int dmicGain);
*  设置麦克阵列的输入增益.
*
*	@param[in] dmicDevId 麦克阵列音频设备号.
*	@param[in] dmicChnId 麦克阵列音频输入通道号.
*	@param[out] dmicGain  麦克阵列音频输入增益.
*
*   @retval 0 success.
*   @retval non-0 failure.

*   @remarks no.
*   @attention no.
*/
int IMP_DMIC_SetGain(int dmicDevId, int dmicChnId, int dmicGain);

/**
*  @fn int IMP_DMIC_GetGain(int dmicDevId, int dmicChnId, int *dmicGain);
*   获取麦克阵列的输入增益.

 *	@param[in]  dmicDevId 麦克阵列音频设备号.
*	@param[in]  dmicChnId 麦克阵列音频输入通道号.
*	@param[out] dmicGain  麦克阵列音频输入增益.
*
*   @retval 0 success.
*   @retval non-0 failure.
*
*   @remarks no.
*   @attention no.
*/
int IMP_DMIC_GetGain(int dmicDevId, int dmicChnId, int *dmicGain);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

