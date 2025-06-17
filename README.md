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

### Pure C

    #include <yangrtc/YangWhip.h> 
    #include <yangrtc/YangPeerInfo.h> 
    #include <yangrtc/YangPeerConnection.h>
    
    int32_t localPort=16000;  
    YangAVInfo* avinfo;
    YangPeerConnection* conn=(YangPeerConnection*)calloc(sizeof(YangPeerConnection),1);    
    
    //yang_init_peerInfo(&conn->peer.peerInfo);
    yang_avinfo_initPeerInfo(&conn->peer.peerInfo,avinfo);
    conn->peer.peerInfo.rtc.rtcLocalPort = localPort;
    conn->peer.peerInfo.direction = YangRecvonly;
    conn->peer.peerInfo.uid = uid;
    conn->peer.peerCallback.recvCallback.context=this;
    conn->peer.peerCallback.recvCallback.receiveAudio=g_rtcrecv_receiveAudio;
    conn->peer.peerCallback.recvCallback.receiveVideo=g_rtcrecv_receiveVideo;
    conn->peer.peerCallback.recvCallback.receiveMsg=g_rtcrecv_receiveMsg;
    
    yang_create_peerConnection(conn);
    conn->addAudioTrack(&conn->peer,Yang_AED_OPUS);
    conn->addVideoTrack(&conn->peer,Yang_VED_H264);
    conn->addTransceiver(&conn->peer,YangMediaAudio,YangRecvonly);
    conn->addTransceiver(&conn->peer,YangMediaVideo,YangRecvonly);
    //sfu   
    if(isWhip)
        yang_whip_connectWhipWhepServer(&conn->peer,url);
    else
        yang_whip_connectSfuServer(&conn->peer,url,mediaServer);
    //p2p
    conn->createDataChannel(&conn->peer);
    if((err=conn->createOffer(&conn->peer, &localSdp))!=Yang_Ok){
        yang_error("createOffer fail",);
        goto cleanup;
    }
    
    if((err=conn->setLocalDescription(&conn->peer, localSdp))!=Yang_Ok){
            yang_error("setLocalDescription fail");
            goto cleanup;
    }
    
    ......
    //get remote peer sdp
    if((err=conn->setRemoteDescription(&peer->peer,remoteSdp))!=Yang_Ok){
        yang_error("setRemoteDescription fail!");
        goto cleanup;
    }

### C++

```
#include <yangrtc/YangWhip.h>
#include <yangrtc/YangPeerInfo.h>
#include <yangrtc/YangPeerConnection7.h>

int32_t localPort=16000;
YangAVInfo* avinfo;
YangPeerInfo peerInfo;

//yang_init_peerInfo(&peerInfo);
yang_avinfo_initPeerInfo(&peerInfo,avinfo);
peerInfo.uid=0;
peerInfo.direction=YangSendonly;
peerInfo.rtc.rtcLocalPort = localPort;

//YangCallbackReceive* receive
//YangCallbackIce* ice
//YangCallbackRtc* rtc
//YangCallbackSslAlert* sslAlert);

YangPeerConnection7* conn=new YangPeerConnection7(&peerInfo,receive,ice, rtc,sslAlert);
conn->addAudioTrack(Yang_AED_OPUS);
conn->addVideoTrack(Yang_VED_H264);
conn->addTransceiver(YangMediaAudio,peerInfo.direction);
conn->addTransceiver(YangMediaVideo,peerInfo.direction);

//sfu
if(isWhip)
    yang_whip_connectWhipWhepServer(&conn->m_peer,url);
else
    yang_whip_connectSfuServer(&conn->m_peer,url,mediaServer);

//p2p
conn->createDataChannel();
if((err=conn->createOffer(&localSdp))!=Yang_Ok){
    yang_error("createOffer fail",);
    goto cleanup;
}

if((err=conn->setLocalDescription(localSdp))!=Yang_Ok){
    yang_error("setLocalDescription fail");
    goto cleanup;
}

......
//get remote peer sdp
if((err=conn->setRemoteDescription(remoteSdp))!=Yang_Ok){
    yang_error("setRemoteDescription fail!");
    goto cleanup;
}
```

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

demo/metapushstream7/metapushstream7.pro  
demo/metaplayer7/metaplayer7.pro

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
