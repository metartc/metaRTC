//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANG_CONFIG_H_
#define INCLUDE_YANG_CONFIG_H_

#include "yang_config_os.h"

#define Yang_Enable_Datachannel 1
#define Yang_Enable_Openh264    1
#define Yang_Enable_Json        1

#if Yang_OS_WIN
#define Yang_Enable_Openssl 1
#define Yang_Enable_GPU_Encoding 0
#define Yang_Enable_Phtread 0
#else
#define Yang_Enable_Openssl 1
#define Yang_Enable_GPU_Encoding 0
#define Yang_Enable_Phtread 1
#endif

#define Yang_StreamName_Lenght 128

#define Yang_Enable_H265_Encoding 1
#define Yang_Enable_MJpeg_Encoding 0
#define Yang_Enable_AV1_Encoding 0

#define Yang_Enable_RTC_Audio 1
#define Yang_Enable_RTC_Video 1

#define Yang_Enable_Tcp_Srs 1

#define Yang_Enable_Embbed 0
#define Yang_Enable_10bit 0


#define Yang_Enable_Dtls 1 //default:using dtls
#define Yang_Enable_TWCC 0

#if Yang_OS_WIN
#define Yang_Enable_H264Decoder 0
#define Yang_Enable_H264Decoder_So 1
#define Yang_Enable_FfmpegSo 0
#else
#define Yang_Enable_H264Decoder 1
#if !Yang_OS_ANDROID
#define Yang_Enable_H264Decoder_So 0
#else
#define Yang_Enable_H264Decoder_So 0
#endif
#define Yang_Enable_FfmpegSo 1
#endif


#if __BYTE_ORDER == __BIG_ENDIAN
#define Yang_Little_Endian 0
#else
#define Yang_Little_Endian 1
#endif

#define Yang_Enable_Timer_Phtread 0
#define Yang_Enable_Ffmpeg_Codec 1

#define Yang_Enable_RtcpXr 0
#define Yang_Rtp_I_Withmeta 0
#define Yang_CTimer_Epoll_Timeout 200

#endif /* INCLUDE_YANG_CONFIG_H_ */
