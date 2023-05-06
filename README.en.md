## MetaRTC Overview
MetaRTC is a webRTC library for embedded/IoT, providing webRTC capabilities for the third generation Internet Metaverse.  
MetaRTC implements the webrtc protocol, supports webrtc/srt/rtmp, and has two versions of pure C and C++.  
The webrtc support is original, without citing the Google webrtc code, and can interoperate with the Google Webrtc library and browser.  
Support windows/linux/android etc.  
Remark: metaRTC2.0/metaRTC3.0/metaRTC4.0 stops updating  
Remark:metaRTC6.0 is stable    
## metaRTC6.0 stable version download
https://github.com/metartc/metaRTC/releases/tag/v6.0-b2

## The difference between MetaRTC and Google Webrtc  
advantage:  

### MetaRTC compilation is simple  
It is difficult to compile webrtc, and there are dozens of G in the warehouse.  

The metaRTC code is small and easy to compile.  

And metartc has a complete compilation tutorial and video at station B.  

### webrtc is developed using c++ and is bulky and not suitable for embedding.  

metartc is mostly developed in the c language and is inherently suitable for embedding.  

easy to re-develop  

### the amount of meta code is small
Webrtc is developed using c++, which is bulky and not suitable for embedded.  

Metatc is mostly developed in C language, which is naturally suitable for embedded.  

### Easy secondary development

webrtc is developed by Google, the amount of code is large, and the secondary development is difficult.  

The amount of meta code is small, the difficulty of secondary development is small, and there is a complete Chinese community.  

### create a chinese ecology  

metaRTC is seamlessly compatible with SRS and ZLM, and the code integrates its signaling interactions.  

metaRTC has implemented national secret gmssl support.  

metaRTC has submitted a Loongson ecological construction application and will increase support for domestic chips and operating systems.   
### Better for Embedded/IoT
Weak embedded computing power, not suitable for webrtc  

Embedded developers are mostly pure C developers  
### Provide H265 complete solution
H265 can save half of the bandwidth than H264. The H265 ecosystem is relatively mature in China. Many industry chips such as security support H265 encoding.  

metaRTC supports H265 version of webrtc  

metaRTC provides SRS with support for H265 ( http://github/metartc/srs-webrtc265 )  

### Built-in pure C version signaling service
The metap2p project has built-in signaling services, and signaling processing is implemented in pure C.  

Support datachannel/websocket/http  
 
### Integrate ffmpeg to support static compilation and integration into ffmpeg
Integrating ffmpeg enables ffmpeg to support webrtc push-pull streaming and p2p  

The new version of metaRTC integrated with FFmpeg supports AEC/AGC/ANC functions such as echo cancellation  

  

## metaRTC function
1.Video encoding 8bit: x264, x265, vaapi, nvenc, etc., the second phase adds AV1 and a variety of hardware encoding.  
2.video encoding 10bit: x265, vaapi, nvenc, etc.  
3.video decoding: ffmpeg and yangh264decoder.   
4.VR: INTERACTIVE AND RECORDING, LIVE BROADCASTING, ETC. OF VIRTUAL VIDEO BASED ON KEYING.  
5.8bit and 10bit web players: yangplayer  
6.Audio: Opus, Aac, Speex, Mp3 and other audio codecs.  
7.AUDIO: AEC, AGC, ANS AND SOUND SYNTHESIS.  
8.transport: webrtc, rtmp, srt, webrtc for their own implementation, does not use the google lib library.  
9.Live: rtmp, srt, webrtc, HLS, HTTP-FLV.  
10.8bit recording: h264, h265 mp4 and flv.  
11.10bit recording: mp4 of h265  
12.screen sharing and control are implemented.  
13. a variety of processing of sound and images is realized.  
14. professional camera gimbal control with multi-lens guide switching.  
15.supports both 32-bit and 64-bit programming.  

## Scope of application
Audio and video applications: video conferencing/recording and live broadcasting/live interactive IPC/cloud games/cloud 3D/metaverse RTC

Industry application: distance education / telemedicine / remote office command and dispatch / security monitoring / financial video AI / video recording and broadcasting / e-commerce live broadcast
## P2P support
Built-in signaling service   
one-to-many live broadcast  
browser p2p communication  
two-way intercom and conference  

## ARM support
Support ARM-Linux  
Support ARM-Android  
Provide ARM compilation scripts  
Provide third-party ARM compilation documents  
## metaRTC WeChat group
You can add WeChat taihang82

## module
### libmetartccore6
webrtc/rtmp protocol implementation and basic application C library, which can be integrated into ffmpeg  
. If the platform has acquisition and decoding, only libmetartccore5 is needed, libmetartc5 is not required  

### libmetartc6
Realize acquisition, encoding, decoding, transmission (SRT implementation), and push-pull streaming

### metapushstream6/metapushstream6_android
Push stream and record demo metapushstream3_android for Android

### metaplayer6/metaplayer6_android
Pull stream demo, metaplayer5_android is Android version

### metap2p6
p2p demo, supports one-to-one and one-to-many, metaplayer5 can pull streams directly from metap2p6
Unlike metapushstream5 and metaplayer5, metap2p5 supports full duplex, that is, one connection supports both push and pull streams

### yangwincodec
nvidia/intel gpu encoding




The following is an enthusiastic netizen using metaRTC (yangwebrtc) test situation, end-to-end latency of 40ms.  
![](https://img-blog.csdnimg.cn/fbd331e04ad94910804a5786f725a297.png?x-oss-process=image/watermark,type_ZHJvaWRzYW5zZmFsbGJhY2s,shadow_50,text_Q1NETiBAeWFuZ3J0Yw==,size_20,color_FFFFFF,t_70,g_se,x_16)

## compile the tutorial
MetaRTC Embedded Webrtc Programming Guide https://blog.csdn.net/m0_56595685/article/details/122010285  
Video Compilation Tutorial https://www.bilibili.com/video/BV1d3411r742/  
### P2P audio and video intercom demo between metaRTC4.0 and browser webrtc
https://blog.csdn.net/m0_56595685/article/details/124784019

### metaRTC realizes audio and video intercom and video conference of P2P application of webrtc
https://blog.csdn.net/m0_56595685/article/details/124067383

### P2P audio and video intercom demo between metaRTC4.0 and browser webrtc
https://blog.csdn.net/m0_56595685/article/details/124784019

### P2P communication programming guide between metaRTC and browser webrtc
https://blog.csdn.net/m0_56595685/article/details/124015458

### metaRTC implements Android webrtc live streaming
https://blog.csdn.net/m0_56595685/article/details/123657024

### metaRTC (ARM linux64) compilation guide
https://blog.csdn.net/m0_56595685/article/details/123900651

### MetaRTC Embedded Webrtc Programming Guide metaRTC Embedded Webrtc Programming Guide
https://blog.csdn.net/m0_56595685/article/details/122139482
https://blog.csdn.net/m0_56595685/article/details/122010285

### metaRTC builds video conferencing
https://blog.csdn.net/m0_56595685/article/details/122775443

### Build a low-latency H265 webrtc application that saves half the bit rate
https://blog.csdn.net/m0_56595685/article/details/121880362

### Build a linux and embedded HD recording and broadcasting system that supports H264 and H265
https://blog.csdn.net/m0_56595685/article/details/121735106

### Build a dual-teacher classroom that supports embedded intelligent terminals across platforms to realize remote interactive teaching
https://blog.csdn.net/m0_56595685/article/details/121735106

### metaRTC integrates FFmpeg to implement audio functions such as echo cancellation (3A)
https://blog.csdn.net/m0_56595685/article/details/122682713

### metaRTC is integrated into ffmpeg to implement srs's webrtc push-pull stream
https://blog.csdn.net/m0_56595685/article/details/122642347

### metaRTC is integrated into ffmpeg to implement webrtc streaming playback of srs
https://blog.csdn.net/m0_56595685/article/details/122594797

### SRS+metaRTC+FFmpeg helps live broadcast enter a new era of webrtc live broadcast interaction
https://blog.csdn.net/m0_56595685/article/details/122697381

