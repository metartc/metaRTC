//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANG_CONFIG_H_
#define INCLUDE_YANG_CONFIG_H_

#define Yang_HaveLibva 0
#define Yang_Have10bit 0

#define Yang_HaveVr 0
#define Yang_GPU_Encoding 0
#define Yang_H265_Encoding 1
#define Yang_Using_Openh264 1

#define Yang_Using_Ffmpeg_Codec 1


#ifdef _WIN32
#define Yang_Using_H264Decoder 0
#define Yang_Using_H264Decoder_So 1
#define Yang_Ffmpeg_UsingSo 0
#else
#define Yang_Using_H264Decoder 1
#ifndef __ANDROID__
#define Yang_Using_H264Decoder_So 1
#else
#define Yang_Using_H264Decoder_So 0
#endif
#define Yang_Ffmpeg_UsingSo 1
#endif

//option lib
#define Yang_HavePicUtilFfmpeg 0
#define Yang_HaveWebsockets 1 //using libwebsockets
#define Yang_HaveCurl 0   //using libcurl
#define Yang_HaveJson 0 //using jsonlib
#define Yang_HaveDtls 1 //default:using dtls
#define Yang_HaveDatachannel 1

#define Yang_Using_TWCC 1
#define Yang_Rtp_I_Withmeta 0
#define Yang_CTimer_Epoll_Timeout 200
#endif /* INCLUDE_YANG_CONFIG_H_ */
