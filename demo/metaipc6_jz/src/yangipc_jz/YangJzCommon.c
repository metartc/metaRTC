//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangipc_jz/YangJzCommon.h>

#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_isp.h>
#include <imp/imp_osd.h>
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



struct chn_conf chn[1] ;
void init_chn(){
	chn[0].index = CH0_INDEX;
	chn[0].enable = CHN0_EN;
	chn[0].payloadType = IMP_ENC_PROFILE_AVC_BASELINE;
	chn[0].fs_chn_attr.pixFmt = PIX_FMT_NV12;
	chn[0].fs_chn_attr.outFrmRateNum = SENSOR_FRAME_RATE_NUM;
	chn[0].fs_chn_attr.outFrmRateDen = SENSOR_FRAME_RATE_DEN;
	chn[0].fs_chn_attr.nrVBs = 2;
	chn[0].fs_chn_attr.type = FS_PHY_CHANNEL;

	chn[0].fs_chn_attr.crop.enable = CROP_EN;
	chn[0].fs_chn_attr.crop.top = 0;
	chn[0].fs_chn_attr.crop.left = 0;
	chn[0].fs_chn_attr.crop.width = SENSOR_WIDTH;
	chn[0].fs_chn_attr.crop.height = SENSOR_HEIGHT;

	chn[0].fs_chn_attr.scaler.enable = 0;

	chn[0].fs_chn_attr.picWidth = SENSOR_WIDTH;
	chn[0].fs_chn_attr.picHeight = SENSOR_HEIGHT;


	chn[0].framesource_chn.deviceID=DEV_ID_FS;
	chn[0].framesource_chn.groupID=CH0_INDEX;
	chn[0].framesource_chn.outputID=0;

	chn[0].imp_encoder.deviceID=DEV_ID_ENC;
	chn[0].imp_encoder.groupID=CH0_INDEX;
	chn[0].imp_encoder.outputID=0;

}


//extern int IMP_OSD_SetPoolSize(int size);
const IMPEncoderRcMode S_RC_METHOD = IMP_ENC_RC_MODE_CAPPED_QUALITY;
IMPSensorInfo sensor_info;
int sample_system_init()
{
	int ret = 0;

	//IMP_OSD_SetPoolSize(512*1024);

	memset(&sensor_info, 0, sizeof(IMPSensorInfo));
	memcpy(sensor_info.name, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.cbus_type = SENSOR_CUBS_TYPE;
	memcpy(sensor_info.i2c.type, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.i2c.addr = SENSOR_I2C_ADDR;

	yang_debug( "sample_system_init start\n");

	ret = IMP_ISP_Open();
	if(ret < 0){
		yang_error( "failed to open ISP\n");
		return -1;
	}

	ret = IMP_ISP_AddSensor(&sensor_info);
	if(ret < 0){
		yang_error( "failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_EnableSensor();
	if(ret < 0){
		yang_error( "failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_System_Init();
	if(ret < 0){
		yang_error( "IMP_System_Init failed\n");
		return -1;
	}

	/* enable turning, to debug graphics */
	ret = IMP_ISP_EnableTuning();
	if(ret < 0){
		yang_error( "IMP_ISP_EnableTuning failed\n");
		return -1;
	}
	IMP_ISP_Tuning_SetContrast(128);
	IMP_ISP_Tuning_SetSharpness(128);
	IMP_ISP_Tuning_SetSaturation(128);
	IMP_ISP_Tuning_SetBrightness(128);
#if 1
	ret = IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_DAY);
	if (ret < 0){
		yang_error( "failed to set running mode\n");
		return -1;
	}
#endif
#if 0
	ret = IMP_ISP_Tuning_SetSensorFPS(SENSOR_FRAME_RATE_NUM, SENSOR_FRAME_RATE_DEN);
	if (ret < 0){
		yang_error( "failed to set sensor fps\n");
		return -1;
	}
#endif
	yang_debug( "ImpSystemInit success\n");

	return 0;
}

int sample_framesource_init()
{
	int i=0, ret;

	//for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr);
			if(ret < 0){
				yang_error( "IMP_FrameSource_CreateChn(chn%d) error !\n", chn[i].index);
				return -1;
			}

			ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr);
			if (ret < 0) {
				yang_error( "IMP_FrameSource_SetChnAttr(chn%d) error !\n",  chn[i].index);
				return -1;
			}
		}
//	}

	return 0;
}


int sample_encoder_init()
{
	int i, ret, chnNum = 0;
	IMPFSChnAttr *imp_chn_attr_tmp;
	IMPEncoderChnAttr channel_attr;

	for (i = 0; i <  FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			imp_chn_attr_tmp = &chn[i].fs_chn_attr;
			chnNum = chn[i].index;

			memset(&channel_attr, 0, sizeof(IMPEncoderChnAttr));

			float ratio = 1;
			if (((uint64_t)imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight) > (1280 * 720)) {
				ratio = log10f(((uint64_t)imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight) / (1280 * 720.0)) + 1;
			} else {
				ratio = 1.0 / (log10f((1280 * 720.0) / ((uint64_t)imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight)) + 1);
			}
			ratio = ratio > 0.1 ? ratio : 0.1;
			unsigned int uTargetBitRate = BITRATE_720P_Kbs * ratio;

			ret = IMP_Encoder_SetDefaultParam(&channel_attr, chn[i].payloadType, S_RC_METHOD,
					imp_chn_attr_tmp->picWidth, imp_chn_attr_tmp->picHeight,
					imp_chn_attr_tmp->outFrmRateNum, imp_chn_attr_tmp->outFrmRateDen,
					imp_chn_attr_tmp->outFrmRateNum * 2 / imp_chn_attr_tmp->outFrmRateDen, 2,
					(S_RC_METHOD == IMP_ENC_RC_MODE_FIXQP) ? 35 : -1,
							uTargetBitRate);
			if (ret < 0) {
				yang_error( "IMP_Encoder_SetDefaultParam(%d) error !\n", chnNum);
				return -1;
			}
#ifdef LOW_BITSTREAM
IMPEncoderRcAttr *rcAttr = &channel_attr.rcAttr;
uTargetBitRate /= 2;

switch (rcAttr->attrRcMode.rcMode) {
case IMP_ENC_RC_MODE_FIXQP:
	rcAttr->attrRcMode.attrFixQp.iInitialQP = 38;
	break;
case IMP_ENC_RC_MODE_CBR:
	rcAttr->attrRcMode.attrCbr.uTargetBitRate = uTargetBitRate;
	rcAttr->attrRcMode.attrCbr.iInitialQP = -1;
	rcAttr->attrRcMode.attrCbr.iMinQP = 34;
	rcAttr->attrRcMode.attrCbr.iMaxQP = 51;
	rcAttr->attrRcMode.attrCbr.iIPDelta = -1;
	rcAttr->attrRcMode.attrCbr.iPBDelta = -1;
	rcAttr->attrRcMode.attrCbr.eRcOptions = IMP_ENC_RC_SCN_CHG_RES | IMP_ENC_RC_OPT_SC_PREVENTION;
	rcAttr->attrRcMode.attrCbr.uMaxPictureSize = uTargetBitRate * 4 / 3;
	break;
case IMP_ENC_RC_MODE_VBR:
	rcAttr->attrRcMode.attrVbr.uTargetBitRate = uTargetBitRate;
	rcAttr->attrRcMode.attrVbr.uMaxBitRate = uTargetBitRate * 4 / 3;
	rcAttr->attrRcMode.attrVbr.iInitialQP = -1;
	rcAttr->attrRcMode.attrVbr.iMinQP = 34;
	rcAttr->attrRcMode.attrVbr.iMaxQP = 51;
	rcAttr->attrRcMode.attrVbr.iIPDelta = -1;
	rcAttr->attrRcMode.attrVbr.iPBDelta = -1;
	rcAttr->attrRcMode.attrVbr.eRcOptions = IMP_ENC_RC_SCN_CHG_RES | IMP_ENC_RC_OPT_SC_PREVENTION;
	rcAttr->attrRcMode.attrVbr.uMaxPictureSize = uTargetBitRate * 4 / 3;
	break;
case IMP_ENC_RC_MODE_CAPPED_VBR:
	rcAttr->attrRcMode.attrCappedVbr.uTargetBitRate = uTargetBitRate;
	rcAttr->attrRcMode.attrCappedVbr.uMaxBitRate = uTargetBitRate * 4 / 3;
	rcAttr->attrRcMode.attrCappedVbr.iInitialQP = -1;
	rcAttr->attrRcMode.attrCappedVbr.iMinQP = 34;
	rcAttr->attrRcMode.attrCappedVbr.iMaxQP = 51;
	rcAttr->attrRcMode.attrCappedVbr.iIPDelta = -1;
	rcAttr->attrRcMode.attrCappedVbr.iPBDelta = -1;
	rcAttr->attrRcMode.attrCappedVbr.eRcOptions = IMP_ENC_RC_SCN_CHG_RES | IMP_ENC_RC_OPT_SC_PREVENTION;
	rcAttr->attrRcMode.attrCappedVbr.uMaxPictureSize = uTargetBitRate * 4 / 3;
	rcAttr->attrRcMode.attrCappedVbr.uMaxPSNR = 42;
	break;
case IMP_ENC_RC_MODE_CAPPED_QUALITY:
	rcAttr->attrRcMode.attrCappedQuality.uTargetBitRate = uTargetBitRate;
	rcAttr->attrRcMode.attrCappedQuality.uMaxBitRate = uTargetBitRate * 4 / 3;
	rcAttr->attrRcMode.attrCappedQuality.iInitialQP = -1;
	rcAttr->attrRcMode.attrCappedQuality.iMinQP = 34;
	rcAttr->attrRcMode.attrCappedQuality.iMaxQP = 51;
	rcAttr->attrRcMode.attrCappedQuality.iIPDelta = -1;
	rcAttr->attrRcMode.attrCappedQuality.iPBDelta = -1;
	rcAttr->attrRcMode.attrCappedQuality.eRcOptions = IMP_ENC_RC_SCN_CHG_RES | IMP_ENC_RC_OPT_SC_PREVENTION;
	rcAttr->attrRcMode.attrCappedQuality.uMaxPictureSize = uTargetBitRate * 4 / 3;
	rcAttr->attrRcMode.attrCappedQuality.uMaxPSNR = 42;
	break;
case IMP_ENC_RC_MODE_INVALID:
	yang_error( "unsupported rcmode:%d, we only support fixqp, cbr vbr and capped vbr\n", rcAttr->attrRcMode.rcMode);
	return -1;
}
#endif

ret = IMP_Encoder_CreateChn(chnNum, &channel_attr);
if (ret < 0) {
	yang_error( "IMP_Encoder_CreateChn(%d) error !\n", chnNum);
	return -1;
}

ret = IMP_Encoder_RegisterChn(chn[i].index, chnNum);
if (ret < 0) {
	yang_error( "IMP_Encoder_RegisterChn(%d, %d) error: %d\n", chn[i].index, chnNum, ret);
	return -1;
}
		}
	}

	return 0;
}



int sample_encoder_exit(void)
{
	int ret = 0, i = 0, chnNum = 0;
	IMPEncoderChnStat chn_stat;

	for (i = 0; i <  FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			chnNum = chn[i].index;
			memset(&chn_stat, 0, sizeof(IMPEncoderChnStat));
			ret = IMP_Encoder_Query(chnNum, &chn_stat);
			if (ret < 0) {
				yang_error( "IMP_Encoder_Query(%d) error: %d\n", chnNum, ret);
				return -1;
			}

			if (chn_stat.registered) {
				ret = IMP_Encoder_UnRegisterChn(chnNum);
				if (ret < 0) {
					yang_error( "IMP_Encoder_UnRegisterChn(%d) error: %d\n", chnNum, ret);
					return -1;
				}

				ret = IMP_Encoder_DestroyChn(chnNum);
				if (ret < 0) {
					yang_error( "IMP_Encoder_DestroyChn(%d) error: %d\n", chnNum, ret);
					return -1;
				}
			}
		}
	}

	return 0;
}

int sample_framesource_exit()
{
	int ret,i;

	for (i = 0; i <  FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			/*Destroy channel */
			ret = IMP_FrameSource_DestroyChn(chn[i].index);
			if (ret < 0) {
				yang_error( "IMP_FrameSource_DestroyChn(%d) error: %d\n", chn[i].index, ret);
				return -1;
			}
		}
	}
	return 0;
}

int sample_system_exit()
{
	int ret = 0;

	yang_debug( "sample_system_exit start\n");


	IMP_System_Exit();

	ret = IMP_ISP_DisableSensor();
	if(ret < 0){
		yang_error( "failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_ISP_DelSensor(&sensor_info);
	if(ret < 0){
		yang_error( "failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_DisableTuning();
	if(ret < 0){
		yang_error( "IMP_ISP_DisableTuning failed\n");
		return -1;
	}

	if(IMP_ISP_Close()){
		yang_error( "failed to open ISP\n");
		return -1;
	}

	yang_debug( " sample_system_exit success\n");

	return 0;
}

int sample_framesource_streamon()
{
	int ret = 0, i = 0;
	/* Enable channels */

	if (chn[0].enable) {
		ret = IMP_FrameSource_EnableChn(chn[0].index);
		if (ret < 0) {
			yang_error("IMP_FrameSource_EnableChn(%d) error: %d\n", ret, chn[0].index);
			return -1;
		}
	}

	return 0;
}

int sample_framesource_streamoff()
{
	int ret = 0, i = 0;
	/* Enable channels */

	if (chn[0].enable){
		ret = IMP_FrameSource_DisableChn(chn[0].index);
		if (ret < 0) {
			yang_error("IMP_FrameSource_DisableChn(%d) error: %d\n", ret, chn[0].index);
			return -1;
		}
	}

	return 0;
}

/**
int sample_get_video_stream()
{
	unsigned int i;
	int ret;
	pthread_t tid[FS_CHN_NUM];

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
            int arg = 0;
            if (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) {
                arg = (((chn[i].payloadType >> 24) << 16) | (4 + chn[i].index));
            } else {
                arg = (((chn[i].payloadType >> 24) << 16) | chn[i].index);
            }
			ret = pthread_create(&tid[i], NULL, get_video_stream, (void *)arg);
			if (ret < 0) {
				yang_error( "Create ChnNum%d get_video_stream failed\n", (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) ? (4 + chn[i].index) : chn[i].index);
			}
		}
	}

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			pthread_join(tid[i],NULL);
		}
	}

	return 0;
}
 **/
