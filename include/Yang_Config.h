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

#define Yang_HaveVr 0
#define Yang_GPU_Encoding 0
#define Yang_H265_Encoding 1

#ifdef _WIN32
#define Yang_Using_H264Decoder 0
#define Yang_Using_H264Decoder_So 0
#define Yang_Ffmpeg_UsingSo 0
#else
#define Yang_Using_H264Decoder 1
#define Yang_Using_H264Decoder_So 1
#define Yang_Ffmpeg_UsingSo 1
#endif

//option lib
#define Yang_HavePicUtilFfmpeg 0
#define Yang_HaveWebsockets 0 //using libwebsockets
#define Yang_HaveCurl 0   //using libcurl
#define Yang_HaveJson 0 //using jsonlib

#define Yang_Using_TWCC 0

#endif /* INCLUDE_YANG_CONFIG_H_ */
