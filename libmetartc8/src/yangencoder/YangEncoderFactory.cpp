//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangencoder/YangEncoderFactory.h>

#include <yangencoder/YangAudioEncoderOpus.h>


#include <yangencoder/YangGpuEncoderFactory.h>

#include <yangencoder/YangH265EncoderSoft.h>

#include <yangencoder/YangVideoEncoderMac.h>
#include <yangencoder/YangEncoderMediacodec.h>

#if Yang_Enable_Openh264
#include <yangencoder/YangOpenH264Encoder.h>
#else
#include <yangencoder/YangH264EncoderSoft.h>
#endif

YangEncoderFactory::YangEncoderFactory() {

}

YangEncoderFactory::~YangEncoderFactory() {

}


YangAudioEncoder* YangEncoderFactory::createAudioEncoder(YangAudioCodec acodec,
        YangAudioInfo *audioInfo) {

    return new YangAudioEncoderOpus();
}
YangAudioEncoder* YangEncoderFactory::createAudioEncoder(
        YangAudioInfo *audioInfo) {

    YangAudioCodec acodec=(YangAudioCodec)audioInfo->audioEncoderType;
    return createAudioEncoder(acodec, audioInfo);
}

YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoCodec vcodec,YangVideoInfo *videoInfo) {

    if(videoInfo->videoEncHwType==0){
        if (vcodec == Yang_VED_H264){
#if Yang_Enable_Openh264
            return new YangOpenH264Encoder();
#else
            return  new YangH264EncoderSoft();
#endif
        }else if (vcodec == Yang_VED_H265){
            return new YangH265EncoderSoft();
        }
    }else{
#if Yang_OS_ANDROID
        return new YangEncoderMediacodec();
#elif Yang_OS_APPLE
        return new YangVideoEncoderMac();
#elif Yang_OS_WIN
        #if Yang_Enable_GPU_Encoding
         YangGpuEncoderFactory gf;
         return gf.createGpuEncoder();
        #endif
#endif
    }
    return NULL;
}

YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoInfo *videoInfo) {
    YangVideoCodec vcodec = Yang_VED_H264;
    if (videoInfo->videoEncoderType == 0)
        vcodec = Yang_VED_H264;
    if (videoInfo->videoEncoderType == 1)
        vcodec = Yang_VED_H265;

    return createVideoEncoder(vcodec, videoInfo);
}
