## MetaRTC Overview
**Code once run everywhere, A pure C version of webRTC**    
MetaRTC是一个跨平台的webRTC SDK,更适合嵌入式/物联网的webRTC应用  
MetaRTC is a cross-platform webRTC SDK, more suitable for embedded/Internet of Things webRTC applications.    

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
+ Linux/Windows/Mac/Android/IOS
+ ARM/MIPS/Loongson/RISC-V/X86
+ MP4/FLV Recording  
  + AAC/H264  
  + AAC/H265(HEVC)
+ Qt/Flutter/Lvgl/Native

## New Features

## flutter ui demo

demo/metaplayer7_flutter

download: https://github.com/metartc/metaRTC/releases/tag/7.0.072 file:metartc7.072.linux.7z

### lvgl ui demo 

demo/metaplayer7_lvgl

download: https://github.com/metartc/metaRTC/releases/tag/7.0.050 file:metartc7.062.linux.lvgl.7z

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

## Peer connect demo

	int32_t err = Yang_Ok;
	char* localSdp=NULL;
	char* remoteSdp=NULL;
	yangbool enableWhipWhep = yangtrue; 
	YangRtcDirection direction = YangSendonly;//YangSendrecv,YangSendonly,YangRecvonly
	YangPeerConnection *peer = (YangPeerConnection*)yang_calloc(sizeof(YangPeerConnection),1);
	yang_create_peerConnection(peer);
	peer->addAudioTrack(&peer->peer,Yang_AED_OPUS);
	peer->addVideoTrack(&peer->peer,Yang_VED_H264);
	peer->addTransceiver(&peer->peer,direction);
	//sfu server
	if(enableWhipWhep)
	     err = peer->connectWhipWhepServer(&peer->peer,url);
	else
	     err = peer->connectSfuServer(&peer->peer);
	//p2p
	peer->createDataChannel(&peer->peer);//add datachannel
	if((err=peer->createOffer(&peer->peer, &localSdp))!=Yang_Ok){
	    yang_error("createOffer fail!");
	    goto cleanup;
	}
	if((err=peer->setLocalDescription(&peer->peer, localSdp))!=Yang_Ok){
	    yang_error("setLocalDescription fail!");
	    goto cleanup;
	}
	......
	//get remote peer sdp
	if((err=peer->setRemoteDescription(&peer->peer,remoteSdp))!=Yang_Ok){
	    yang_error("setRemoteDescription fail!");
	    goto cleanup;
	}

 


## metaRTC服务支持(service support)
微信号: taihang82  
email: yangrtc@aliyun.com yangrtc@outlook.com   
twitter: metaRTC@metartc_sdk  
### metaRTC微信公众号

![gzh2](https://github.com/metartc/metaRTC/assets/87118023/29a65e31-083f-4151-ab86-0103766e9d0a)


### metaRTC问答星球      
  ![xingqiu2](https://user-images.githubusercontent.com/87118023/227077884-0163fcb6-ab0d-4933-88c9-0164b80f4d02.jpg)  
https://t.zsxq.com/0cfpXQYoX

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

libmetartccore7/libmetartccore7.pro

libmetartc7/libmetartc7.pro

### windows

#### visual studio

project_msvc/

#### Qt(msvc)

libmetartccore7/libmetartccore7.pro

libmetartc7/libmetartc7.pro
### mac/ios
### xcode
project_xcode/

## demo compile
### Qt demo
demo/metapushstream6/metapushstream7.pro  
demo/metaplayer6/metaplayer7.pro
### android demo
android studio ide(api:29)  
demo/metapushstream7_android  
demo/metaplayer7_android  

## metaRTC streams to janus via the whip protocol  
https://github.com/metartc/metaRTC/wiki/metaRTC-streams-to-janus-via-the-whip-protocol

## Learning metaRTC 
https://github.com/metartc/metaRTC/wiki/metaRTC-Learning   
## MetaRTC vs webrtc
https://github.com/metartc/metaRTC/wiki/metaRTC-vs-webrtc
## metaRTC features
https://github.com/metartc/metaRTC/wiki/metaRTC-Features



