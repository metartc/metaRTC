//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANG_CONFIG_H_
#define INCLUDE_YANG_CONFIG_H_

#define Yang_Enable_Datachannel 1
#define Yang_Enable_Openh264 1

#if _WIN32
#define Yang_Enable_Openssl 1
#else
#define Yang_Enable_Openssl 0
#endif

#define Yang_Enable_Embbed 1
#define Yang_Enable_10bit 0
#define Yang_Enable_GPU_Encoding 0
#define Yang_Enable_H265_Encoding 1

#ifdef _WIN32
#define Yang_Enable_H264Decoder 0
#define Yang_Enable_H264Decoder_So 1
#define Yang_Enable_FfmpegSo 0
#else
#define Yang_Enable_H264Decoder 1
#ifndef __ANDROID__
#define Yang_Enable_H264Decoder_So 1
#else
#define Yang_Enable_H264Decoder_So 0
#endif
#define Yang_Enable_FfmpegSo 1
#endif



#define Yang_Enable_Vr 0

#define Yang_Enable_Dtls 1 //default:using dtls


#define Yang_Enable_Timer_Phtread 0
#define Yang_Enable_Ffmpeg_Codec 1

//option lib
#define Yang_Enable_PicUtilFfmpeg 0
#define Yang_Enable_Websockets 1 //using libwebsockets
#define Yang_Enable_Curl 0   //using libcurl
#define Yang_Enable_Json 0 //using jsonlib


#define Yang_Enable_TWCC 0
#define Yang_Enable_RtcpXr 0
#define Yang_Rtp_I_Withmeta 0
#define Yang_CTimer_Epoll_Timeout 200

#endif /* INCLUDE_YANG_CONFIG_H_ */
