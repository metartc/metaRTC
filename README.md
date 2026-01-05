## MetaRTC Overview
* The easiest WebRTC stack for embedded / IoT / robotics devices
* Code once run everywhere, A pure C version of webRTC
* Linux/RTOS/Windows/Mac/Android/IOS   
## MetaRTC implements a *complete* WebRTC stack

- ICE / ICE-Lite
- STUN and TURN (configurable)
- DTLS / SRTP
- Full RTP/RTCP support
- Adaptive bitrate & congestion control
- Flexible media pipelines
- P2P connection logic
- Customizable session/signaling logic
- UDP and optional TCP fallback
- Multi-stream, multi-track support

In short, MetaRTC provides **full control**, can run in **fully offline environments**, and is suitable for **large-scale product integration**.

## Key Features

+ Audio/Video Support  
  
  + H264/H265(HEVC) 
  + OPUS/G711A/G711U/AAC/MP3/SPEEX  
  + AEC/ANS/AGC/VAD/CNG 

+ DataChannels

+ NACK/PLI/FIR/FEC/TWCC

+ STUN/TURN Support

+ IPv4/IPv6

+ Whip/Whep  

+ Linux/RTOS/Windows/Mac/Android/IOS

+ ARM/MIPS/Loongson/RISC-V/X86

+ MP4/FLV Recording  
  
  + AAC/H264  
  + AAC/H265(HEVC)

+ Flutter/React Native/UniApp/QT/Lvgl/Native

## module

### libmetartccore7(pure C)

Implementation of webRTC protocol stack    
AEC/ANS/AGC and other audio and video processing libraries

### libmetartc7(C++)

Realize audio and video collection, encoding, decoding, transmission, rendering, and push-pull streaming  

## Dependencies

To compile libmetartccore7, you'll need to satisfy the following dependencies:  
[OpenSSL](https://www.openssl.org/) or [Mbedtls](https://github.com/Mbed-TLS/mbedtls)  
[libsrtp](https://github.com/cisco/libsrtp)  
[usrsctp](https://github.com/sctplab/usrsctp)  
[Third-party library download](https://github.com/metartc/metaRTC/wiki/Third%E2%80%90party-library-download) 

## metaRTC API
[metaRTC7 API](https://github.com/metartc/metaRTC/wiki/metaRTC7--API)  
[metaRTC 7.0 API Programming Guide](https://github.com/metartc/metaRTC/wiki/metaRTC-7.0-API-Programming-Guide)

## Compile
[metaRTC Compilation](https://github.com/metartc/metaRTC/wiki/metaRTC-Compilation)   
[metaRTC(ARM linux64) Compilation](https://github.com/metartc/metaRTC/wiki/metaRTC(ARM-linux64)-Compilation)  
[metaRTC mbedtls(PolarSSL) Compilation](https://github.com/metartc/metaRTC/wiki/metaRTC-mbedtls(PolarSSL)-Compilation)


## metaRTC Programming guide
[metaRTC H265 HEVC Programming Guide](https://github.com/metartc/metaRTC/wiki/metaRTC-H265-HEVC-Programming-Guide)   
[metaRTC Mac iOS Programming Guide](https://github.com/metartc/metaRTC/wiki/metaRTC-Mac-iOS-Programming-Guide)   
[metaRTC datachannel programming guide](https://github.com/metartc/metaRTC/wiki/metaRTC-datachannel-programming-guide)  
[metaRTC7 demo mac ios compilation guide](https://github.com/metartc/metaRTC/wiki/metaRTC7-demo-mac-ios-compilation-guide)  

## metaRTC streams to janus via the whip protocol
https://github.com/metartc/metaRTC/wiki/metaRTC-streams-to-janus-via-the-whip-protocol
