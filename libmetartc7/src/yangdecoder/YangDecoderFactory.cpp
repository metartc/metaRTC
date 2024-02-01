//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangdecoder/YangDecoderFactory.h>
#include <yangdecoder/YangH264DecoderSoftFactory.h>
#include <yangdecoder/YangAudioDecoderOpus.h>
#include <yangdecoder/YangH264DecoderSoft.h>
#include <yangdecoder/YangDecoderOpenh264.h>

#if Yang_OS_ANDROID
#include <yangdecoder/YangDecoderMediacodec.h>
#else
#include <yangdecoder/pc/YangAudioDecoderAac.h>
#include <yangdecoder/pc/YangAudioDecoderSpeex.h>
#include <yangdecoder/pc/YangVideoDecoderFfmpeg.h>
#endif


YangDecoderFactory::YangDecoderFactory() {


}

YangDecoderFactory::~YangDecoderFactory() {

}

YangAudioDecoder *YangDecoderFactory::createAudioDecoder(YangAudioCodec paet,YangAudioParam *pcontext){

#if  !Yang_OS_ANDROID
	if(paet==Yang_AED_SPEEX) return new YangAudioDecoderSpeex(pcontext);
	if(paet==Yang_AED_AAC) return new YangAudioDecoderAac(pcontext);
#endif
	return new YangAudioDecoderOpus(pcontext);
}

YangAudioDecoder *YangDecoderFactory::createAudioDecoder(YangAudioParam *pcontext){


	return createAudioDecoder(pcontext->encode,pcontext);
}
#if !Yang_OS_ANDROID
YangVideoDecoder* YangDecoderFactory::createFfmpegVideoDecoder(YangVideoCodec paet,YangVideoInfo *pcontext){
#if Yang_Enable_Ffmpeg_Codec
	return new YangVideoDecoderFfmpeg(pcontext,paet);
#else
	return NULL;
#endif
}
#endif
YangVideoDecoder* YangDecoderFactory::createVideoDecoder(YangVideoCodec paet,YangVideoInfo *pcontext){

#if Yang_OS_ANDROID
	if(pcontext->videoDecHwType==0)
		return new YangH264DecoderSoft();
	else
		return new YangDecoderMediacodec(pcontext,paet);
#else
	if (paet == Yang_VED_H264)				{
		if(pcontext->videoDecHwType==0){
#if	Yang_Enable_H264Decoder
			return new YangH264DecoderSoft();
#else
			return new YangVideoDecoderFfmpeg(pcontext,paet);
#endif
		}else{
#if Yang_Enable_Ffmpeg_Codec
			return new YangVideoDecoderFfmpeg(pcontext,paet);
#else
			return NULL;
#endif
		}
	}
#if Yang_Enable_Ffmpeg_Codec
	if (paet == Yang_VED_H265)				return new YangVideoDecoderFfmpeg(pcontext,paet);
#endif
#endif
	return NULL;
}
YangVideoDecoder* YangDecoderFactory::createVideoDecoder(YangVideoInfo *pcontext){
	YangVideoCodec maet=Yang_VED_H264;
	if(pcontext->videoDecoderType==Yang_VED_H265) maet=Yang_VED_H265;
	return createVideoDecoder(maet,pcontext);
}
