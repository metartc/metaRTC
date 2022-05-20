## MetaRTC Overview
MetaRTC is a webRTC library for embedded/IoT, providing webRTC capabilities for the third generation Internet Metaverse.  
MetaRTC implements the webrtc protocol, supports webrtc/srt/rtmp, and has two versions of pure C and C++.  
The webrtc support is original, without citing the Google webrtc code, and can interoperate with the Google Webrtc library and browser.  
Support windows/linux/android etc.  
yangwebrtc for the PC version https://github.com/metartc/yangwebrtc  
Remark:metaRTC3.0 is stable  metaRTC4.0 is developing and not stable  
## The difference between MetaRTC and Google Webrtc  
advantage:  

1.MetaRTC compilation is simple  
Webrtc compilation is difficult, requires scientific Internet access, and warehouses dozens of G. And metatc on station B has a complete compilation tutorial and video
small size  
2.webrtc is developed using c++ and is bulky and not suitable for embedding. metartc is mostly developed in the c language and is inherently suitable for embedding.
easy to re-develop  
3.webrtc is developed by google, the amount of code is large, and the secondary development is difficult. the amount of meta code is small, the difficulty of secondary development is small, and there is a complete community of chinese people.  
4.create a chinese ecology  
 webrtc is p2p, there is no server side, and the open source server side, various, high learning costs, developers often struggle to use that webrtc open source service. metartc recommends using srs yang to set up a big guy open source server (written by the chinese), of course, if you have your own streaming media server also support docking.  
5.more open  
metartc is more localized, has independent development rights, the required functions and suggestions can be mentioned issue, and iterative metartc will be updated according to the developer's suggestions  
6.complete solutions available  
Metartc provides a full set of solutions, such as developers want to use H265, and SRS does not support 265, we extend H265 support on SRS to provide a complete solution from the client to the server  
7. more controllable  

Recent Java log4j security vulnerabilities, brushed the entire Internet, log4j broke most of the Java day, for RTC, more need an independent and controllable RTC  

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
IT CAN BE USED FOR: VIDEO CONFERENCING, HD RECORDING AND LIVE BROADCASTING, LIVE INTERACTIVE, CLOUD GAMES, CLOUD 3D AND OTHER VIDEO AND AUDIO APPLICATIONS.   
IT CAN BE USED FOR REMOTE EDUCATION, TELEMEDICINE, COMMAND AND DISPATCH, SECURITY MONITORING, FILM AND TELEVISION RECORDING, COLLABORATIVE OFFICE, LIVE INTERACTIVE AND OTHER INDUSTRY APPLICATIONS.  

The following is an enthusiastic netizen using metaRTC (yangwebrtc) test situation, end-to-end latency of 40ms.  
![](https://img-blog.csdnimg.cn/fbd331e04ad94910804a5786f725a297.png?x-oss-process=image/watermark,type_ZHJvaWRzYW5zZmFsbGJhY2s,shadow_50,text_Q1NETiBAeWFuZ3J0Yw==,size_20,color_FFFFFF,t_70,g_se,x_16)

## compile the tutorial
MetaRTC Embedded Webrtc Programming Guide https://blog.csdn.net/m0_56595685/article/details/122010285  
Video Compilation Tutorial https://www.bilibili.com/video/BV1d3411r742/  

