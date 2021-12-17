/*
 * Yang_Config.h
 *
 *  Created on: 2020年12月19日
 *      Author: yang
 */

#ifndef INCLUDE_YANG_CONFIG_H_
#define INCLUDE_YANG_CONFIG_H_

#define Yang_HaveLibva 0
#define Yang_Have10bit 0
#define Yang_HavePicUtilFfmpeg 0
#define Yang_HaveWebsockets 1
#define Yang_HaveVr 0
#define Yang_GPU_Encoding 0
#define Yang_H265_Encoding 1
#ifdef _WIN32
#define Yang_Ffmpeg_UsingSo 0
#else
#define Yang_Ffmpeg_UsingSo 1
#endif

#endif /* INCLUDE_YANG_CONFIG_H_ */
