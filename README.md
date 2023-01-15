## MetaRTC Overview
MetaRTC是一个为嵌入式/物联网打造的webRTC库，为第三代互联网 元宇宙提供webRTC能力。  
MetaRTC实现了webrtc协议，提供纯C和C++两种版本接口。   
srt/rtmp支持转入yangwebrtc，metaRTC不再支持  
webrtc支持为原创，没有引用谷歌webrtc代码,可与谷歌Webrtc库和浏览器互通。  
支持windows/linux/android等     

MetaRTC is a webRTC library for embedded/IoT, providing webRTC capabilities for the third generation Internet Metaverse.  
MetaRTC implements the webrtc protocol, supports webrtc/srt/rtmp, and has two versions of pure C and C++.  
The webrtc support is original, without citing the Google webrtc code, and can interoperate with the Google Webrtc library and browser.  
Support windows/linux/android etc.  

Remark:metaRTC3.0/metaRTC4.0等停止更新，推荐使用稳定版metaRTC6.0  
Remark:metaRTC6.0为稳定版本   
Remark:metaRTC6.0 is stable    

## metaRTC服务微信群
可加微信taihang82  

## Learning metaRTC 教程
https://github.com/metartc/metaRTC/wiki/metaRTC-Learning  

### metaRTC6.0稳定版本下载
https://github.com/metartc/metaRTC/releases/tag/v6.0-b0  
### 纯C版IPC第三方库下载
https://github.com/metartc/metaRTC/releases/tag/v5.0-b1   

### metaRTC5.0 API
https://github.com/metartc/metaRTC/wiki/metaRTC5.0-API  
https://github.com/metartc/metaRTC/wiki/metaRTC5.0-API-Sample   
 
 ## module
### libmetartccore
webrtc/rtmp 协议实现和基础应用C类库，可集成到ffmpeg  
如果平台已有采集和编解吗，只需要libmetartccore即可，不需要libmetartc
### libmetartc
实现采集、编码、解码、传输(SRT实现)以及推拉流等实现
### demo
#### demo/libmetaApp和demo/libmetaCApp
sfudemo和p2pdemo的C++和C语言的sdk库  
#### demo/metaipc6_jz和demo/metaipc6_rk1109及metap2p6_html
嵌入式p2p ipc demo，metaipc6_jz为君正demo  
metaipc6_rk1109为瑞芯微rk1109demo  
metap2p6_html为浏览器的直播和互动demo  

#### demo/metapushstream和demo/metapushstream_android
推流和录制 demo metapushstream_android为安卓版
#### demo/metaplayer和demo/metaplayer_android
拉流demo,metaplayer_android为安卓版
#### demo/metap2p和demo/metaipc 
p2p demo，支持一对一和一对多，metaplayer3可直接从metap2p拉流  
和metapushstream与metaplayer不同，metap2p支持全双工，即一个连接同时支持推拉流   

### codec/yangwincodec
nvidia/intel gpu编码   

### vsproject
windows vs2019 project  
 
## MetaRTC和谷歌Webrtc的区别
### MetaRTC编译简单
webrtc编译难，仓库几十个G。  

metaRTC代码量小，编译简单。  

而metartc在[B站](https://www.bilibili.com/video/BV1d3411r742/)有完整的编译教程和视频。  


### 体积小  
webrtc使用c++开发，体积大，不适合嵌入式。  

metartc大多数使用c语言开发，天生适合嵌入式。  

### 容易二次开发  
webrtc是谷歌开发，代码量大，二次开发难度大。  

meta代码量小，二次开发难度小，并且有完整的国人社区。  

### 打造国人生态  
metaRTC无缝兼容SRS和ZLM,代码整合了其信令交互。  

metaRTC已经实现了国密gmssl支持 。  

metaRTC实现了对龙芯的支持，将来将支持更多的国产芯片。  

### 更适合嵌入式/物联网  
嵌入式算力弱，不适合webrtc  

嵌入式开发人员纯C开发人员多  

### 更适合元宇宙RTC  

第三代互联网为元宇宙  

支持元宇宙的穿戴设备算力弱，更适合metaRTC  

### 提供H265全套解决方案  

H265比H264可以节省一半的带宽,H265生态在国内比较成熟，如安防等很多行业芯片都支持H265编码。  

metaRTC支持H265版webrtc  

metaRTC提供支持H265的SRS(http://github/metartc/srs-webrtc265)  

### 内置纯C版信令服务  

metap2p工程内置信令服务，信令处理均为纯C实现。

支持datachannel/websocket/http

### 集成ffmpeg 支持静态编译集成到ffmpeg  

集成ffmpeg使ffmpeg支持webrtc推拉流和p2p  

metaRTC集成FFmpeg新版本支持回声消除等AEC/AGC/ANC功能  

## metaRTC功能

1. video encoding 8bit:x265, vaapi, nvenc, etc。
2. video encoding  10bit:x265、vaapi、nvenc，etc。
3. video decoding：ffmpeg or yangh264decoder。
4. VR:基于抠图实现虚拟视频的互动和录制、直播等。
5. 8bit和10bit网络播放器：yangplayer
6. audio：Opus、Aac、Speex、Mp3 and other audio codecs。
7. audio：AEC、AGC、ANS and  SOUND SYNTHESIS 声音合成等处理。
8. transport：webrtc、rtmp、srt，webrtc is non-google lib。
9. live：rtmp、srt、webrtc、HLS、HTTP-FLV。
10. 8bit recording：hh264, h265 mp4 and flv。
11. 10bit recording：h265 mp4
12. screen sharing and control 实现了屏幕共享与控制。
13. a variety of processing of sound and images is realized 实现了声音和图像多种处理。
14. professional camera gimbal control with multi-lens guide switching 专业摄像头的云台控制与多镜头导播切换。
15. supports both 32-bit and 64-bit programming 支持32位和64位编程。
## 应用范围

音视频应用： 视频会议/录播直播/直播互动 IPC/云游戏/云3D/元宇宙RTC  

行业应用： 远程教育/远程医疗/远程办公 指挥调度/安防监控/金融 视频AI/影视录播/电商直播   

## P2P支持
可内置信令服务  
实现一对多直播  
实现浏览器p2p通信  
实现双向对讲和会议  

## ARM支持

支持ARM-Linux  
支持ARM-Android  
提供ARM编译脚本  
提供第三方ARM编译文档  

## 录制mp4
修改配置文件include/yang_config.h 设置Yang_Enable_Openh264为0  
修改配置文件yang_config.ini enc项 createMeta=1  
或者程序设置m_context->avinfo.enc.createMeta=1  

## demo compile
### linux
sudo apt install libasound2-dev  
sudo apt install libgl1-mesa-dev 
