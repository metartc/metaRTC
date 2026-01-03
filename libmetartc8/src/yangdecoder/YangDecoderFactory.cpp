//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangdecoder/YangDecoderFactory.h>
#include <yangdecoder/YangAudioDecoderOpus.h>

#if	Yang_Enable_H264Decoder
	#include <yangdecoder/YangH264DecoderSoft.h>
#endif

#include <yangdecoder/YangVideoDecoderFfmpeg.h>

#if Yang_OS_ANDROID
#include <yangdecoder/YangDecoderMediacodec.h>
#endif

#if Yang_OS_APPLE
#include <yangdecoder/YangVideoDecoderMac.h>
#endif

YangDecoderFactory::YangDecoderFactory() {


}

YangDecoderFactory::~YangDecoderFactory() {

}

YangAudioDecoder *YangDecoderFactory::createAudioDecoder(YangAudioCodec acodec,YangAudioParam *audioParam){

    return new YangAudioDecoderOpus(audioParam);
}

YangAudioDecoder *YangDecoderFactory::createAudioDecoder(YangAudioParam *audioParam){

    return createAudioDecoder(audioParam->encode,audioParam);
}
#if !Yang_OS_ANDROID
YangVideoDecoder* YangDecoderFactory::createFfmpegVideoDecoder(YangVideoCodec vcodec,YangVideoInfo *videoInfo){
#if Yang_Enable_Ffmpeg_Codec
    return new YangVideoDecoderFfmpeg(videoInfo,vcodec);
#else
	return NULL;
#endif
}
#endif
YangVideoDecoder* YangDecoderFactory::createVideoDecoder(YangVideoCodec vcodec,YangVideoInfo *videoInfo){

   if(videoInfo->videoDecHwType==0){
#if	Yang_Enable_H264Decoder
        return new YangH264DecoderSoft();
#else
        return new YangVideoDecoderFfmpeg(videoInfo,vcodec);
#endif
   }else{
#if Yang_OS_ANDROID
        return new YangDecoderMediacodec(videoInfo,vcodec);
#elif Yang_OS_APPLE
        return new YangVideoDecoderMac();
#else
        return new YangVideoDecoderFfmpeg(videoInfo,vcodec);
#endif

   }
	return NULL;
}

YangVideoDecoder* YangDecoderFactory::createVideoDecoder(YangVideoInfo *videoInfo){
    YangVideoCodec vcodec=Yang_VED_H264;
    if(videoInfo->videoDecoderType==Yang_VED_H265)
    	vcodec=Yang_VED_H265;
    return createVideoDecoder(vcodec,videoInfo);
}
