## MetaRTC Overview
MetaRTC是一个为嵌入式/物联网打造的webRTC库，为第三代互联网 元宇宙提供webRTC能力。  
MetaRTC实现了webrtc协议，支持webrtc/srt/rtmp，可与谷歌webrtc互联互通。   
GitHub - metartc/metaRTC: 中国人自己的webrtc,非谷歌lib    
metaRTC为嵌入式版本   
yangwebrtc为PC版本  https://github.com/metartc/yangwebrtc    

MetaRTC is an webRTC library built for embedded/IoT that provides webRTC capabilities for the third-generation Internet metaverse.  
MetaRTC implements the webrtc protocol, supports webrtc/srt/rtmp, and can be interconnected with Google webrtc.  
non-Google lib metaRTC for the embedded version  
yangwebrtc for the PC version https://github.com/metartc/yangwebrtc  
 
## MetaRTC和谷歌Webrtc的区别
优势:
1. MetaRTC编译简单
- webrtc编译难，需要科学上网，仓库几十个G。
而metartc在[B站](https://www.bilibili.com/video/BV1d3411r742/)有完整的编译教程和视频


2. 体积小
- webrtc使用c++开发，体积大，不适合嵌入式。
metartc大多数使用c语言开发，天生适合嵌入式。

3. 容易二次开发
- webrtc是谷歌开发，代码量大，二次开发难度大。
meta代码量小，二次开发难度小，并且有完整的国人社区。

4. 打造国人生态
- webrtc是p2p的，没有服务端，而开源的服务端，五花八门，学习成本高，开发者经常纠结使用那个webrtc开源服务。metartc推荐使用[srs](https://github.com/ossrs/srs) 杨成立大佬开源的服务端（国人写的）,当然如果你有自己的流媒体服务器也支持对接。

5. 更开放
- metartc更本土化，拥有自主的开发权，需要的功能和建议都可以提issue，会根据开发者的建议来更新迭代metartc

6. 提供全套解决方案 
- metartc 提供全套的解决方案，比如开发者想使用H265，而srs不支持265，我们就在srs上扩展了[H265的支持]( http://github/metartc/srs-webrtc265),提供客户端到服务端的完整解决方案

7.更可控
- 近期Java log4j的安全漏洞，刷屏了整个互联网，log4j捅破了Java的大半片天，对于RTC来说，更需要一个自主可控的RTC

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

可用于：
视频会议、高清录播直播、直播互动、云游戏、云3D等多种视音频应用。
可用于远程教育、远程医疗、指挥调度、安防监控、影视录播、协同办公、直播互动等多种行业应用。
## libmetartc2 compile
mkdir build  
cd build   
cmake .. -DCMAKE_BUILD_TYPE=Release  
make  

下面是一热心网友使用metaRTC(yangwebrtc)测试情况，端到端延迟时间为40ms。  
The following is an enthusiastic netizen using metaRTC (yangwebrtc) test situation, end-to-end latency of 40ms.   

![](https://img-blog.csdnimg.cn/fbd331e04ad94910804a5786f725a297.png?x-oss-process=image/watermark,type_ZHJvaWRzYW5zZmFsbGJhY2s,shadow_50,text_Q1NETiBAeWFuZ3J0Yw==,size_20,color_FFFFFF,t_70,g_se,x_16)

## compile the tutorial 编译教程
### MetaRTC Embedded Webrtc Programming Guide  metaRTC嵌入式webrtc编程指南 
https://blog.csdn.net/m0_56595685/article/details/122139482   
https://blog.csdn.net/m0_56595685/article/details/122010285  
搭建低延迟并节省一半码率的H265的webrtc应用 https://blog.csdn.net/m0_56595685/article/details/121880362  
搭建支持H264和H265的linux和嵌入式的高清录播直播系统 https://blog.csdn.net/m0_56595685/article/details/121735106  
搭建跨平台支持嵌入式智能终端的双师课堂实现远程互动教学 https://blog.csdn.net/m0_56595685/article/details/121735106  
### Video Compilation Tutorial 视频编译教程
https://www.bilibili.com/video/BV1d3411r742/  


