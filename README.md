## MetaRTC Overview
MetaRTC是一个为嵌入式/物联网打造的RTC库，为第三代互联网 元宇宙提供RTC能力。
MetaRTC实现了webrtc协议，支持webrtc/srt/rtmp，可与谷歌webrtc互联互通。
GitHub - metartc/metaRTC: 中国人自己的webrtc,非谷歌lib

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

1. 视频编码 8bit:x264、x265、vaapi、nvenc等，二期增加AV1和多种硬件编码。
2. 视频编码 10bit:x265、vaapi、nvenc等。
3. 视频解码：ffmpeg和yangh264decoder。
4. VR:基于抠图实现虚拟视频的互动和录制、直播等。
5. 8bit和10bit网络播放器：yangplayer
6. 音频：Opus、Aac、Speex、Mp3等音频编解码。
7. 音频：AEC、AGC、ANS及声音合成等处理。
8. 传输：webrtc、rtmp、srt，webrtc为自己实现，没使用谷歌lib库。
9. 直播：rtmp、srt、webrtc、HLS、HTTP-FLV。
10. 8bit录制：h264、h265的mp4和flv。
11. 10bit录制：h265的mp4
12. 实现了屏幕共享与控制。
13. 实现了声音和图像多种处理。
14. 专业摄像头的云台控制与多镜头导播切换。
15. 支持32位和64位编程。

可用于：
视频会议、高清录播直播、直播互动、云游戏、云3D等多种视音频应用。
可用于远程教育、远程医疗、指挥调度、安防监控、影视录播、协同办公、直播互动等多种行业应用。


下面是一热心网友使用metaRTC(yangwebrtc)测试情况，端到端延迟时间为40ms。
![](https://img-blog.csdnimg.cn/fbd331e04ad94910804a5786f725a297.png?x-oss-process=image/watermark,type_ZHJvaWRzYW5zZmFsbGJhY2s,shadow_50,text_Q1NETiBAeWFuZ3J0Yw==,size_20,color_FFFFFF,t_70,g_se,x_16)
