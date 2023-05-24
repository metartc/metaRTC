## MetaRTC Overview
MetaRTC是一个跨平台的webRTC SDK,更适合嵌入式/物联网的webRTC应用  
MetaRTC is a cross-platform webRTC SDK, more suitable for embedded/Internet of Things webRTC applications.    
**Remark:metaRTC6.0 is stable**  
**metaRTC6.0 API及相关文档在metaRTC问答星球上持续更新**  
## Key Features
+ Audio/Video Support  
H264/H265  
Opus/G711A/G711U/AAC/MP3/SPEEX  
AEC/ANS/AGC/VAD  
+ DataChannels
+ NACK/PLI/FIR/FEC/TWCC
+ STUN/TURN Support
+ IPv4/IPv6
+ Whip/Whep  
+ Linux/Android/Windows
+ MP4/FLV Recording  
AAC/H264  
AAC/H265(HEVC)
## module
### libmetartccore6(pure C)
Implementation of webRTC protocol stack    
AEC/ANS/AGC and other audio and video processing libraries
### libmetartc6(C++)
Realize audio and video collection, encoding, decoding, transmission, rendering, and push-pull streaming  

## Dependencies
To compile libmetartccore6, you'll need to satisfy the following dependencies:  
[OpenSSL](https://www.openssl.org/) or [Mbedtls](https://github.com/Mbed-TLS/mbedtls)  
[libsrtp](https://github.com/cisco/libsrtp)  
[usrsctp](https://github.com/sctplab/usrsctp)  

## metaRTC Version
metaRTC version:**社区版/标准版/企业版**  
当前开源版本为社区版  星球用户享受标准版及技术支持 
### Vesion Feature
社区版：webrtc协议栈的基础实现  
标准版：webrtc协议栈优化实现+实用类库(如mqtt/json/静态编解码库等)+demo  
企业版：webrtc协议栈深度优化实现+定制化的低代码产品sdk  
 
## metaRTC问答星球      
  ![xingqiu2](https://user-images.githubusercontent.com/87118023/227077884-0163fcb6-ab0d-4933-88c9-0164b80f4d02.jpg)  
https://t.zsxq.com/0cfpXQYoX

## metaRTC服务支持
微信号: taihang82  
email: yangrtc@aliyun.com yangrtc@outlook.com   
twitter: metaRTC@metartc_sdk

## Compile

### linux/android

#### cmake

`./cmake_lib_x64.sh `

`./cmake_lib_android.sh `

##### module

./cmake_mips32.sh  

#arm64

./cmake_arm.sh  

./cmake_x64.sh 

#arm64-v8a armeabi x86_64

./cmake_android.sh  

#### Qt

libmetartccore6/libmetartccore6.pro

libmetartc6/libmetartc6.pro

### windows

#### visual studio

vsproject/metaRTC6.sln

#### Qt(msvc)

libmetartccore6/libmetartccore6.pro

libmetartc6/libmetartc6.pro

## demo compile
### Qt demo
demo/metapushstream6/metapushstream6.pro  
demo/metaplayer6/metaplayer6.pro
### android demo
android studio ide(api:29)  
demo/metapushstream6_android  
demo/metaplayer6_android  

## metaRTC streams to janus via the whip protocol  
https://github.com/metartc/metaRTC/wiki/metaRTC-streams-to-janus-via-the-whip-protocol

## Learning metaRTC 
https://github.com/metartc/metaRTC/wiki/metaRTC-Learning   
## MetaRTC vs webrtc
https://github.com/metartc/metaRTC/wiki/metaRTC-vs-webrtc
## metaRTC features
https://github.com/metartc/metaRTC/wiki/metaRTC-Features



