//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangdecoder/YangDecoderFactory.h>
#include <yangdecoder/YangAudioDecoderOpus.h>
#ifndef __ANDROID__
#include <yangdecoder/pc/YangAudioDecoderAac.h>
#include <yangdecoder/pc/YangAudioDecoderSpeex.h>
#include <yangdecoder/pc/YangVideoDecoderFfmpeg.h>
#endif


//#include "YangVideoDecoderIntel.h"
#include "YangH264DecoderSoft.h"
#include "YangH264DecoderSoftFactory.h"
YangDecoderFactory::YangDecoderFactory() {


}

YangDecoderFactory::~YangDecoderFactory() {

}

YangAudioDecoder *YangDecoderFactory::createAudioDecoder(YangAudioCodec paet,YangAudioParam *pcontext){
	//if(paet==Yang_AED_MP3) return new YangAudioDecoderMp3(pcontext);
#ifndef __ANDROID__
	if(paet==Yang_AED_SPEEX) return new YangAudioDecoderSpeex(pcontext);
	if(paet==Yang_AED_AAC) return new YangAudioDecoderAac(pcontext);
#endif
	 return new YangAudioDecoderOpus(pcontext);
}

YangAudioDecoder *YangDecoderFactory::createAudioDecoder(YangAudioParam *pcontext){


	return createAudioDecoder(pcontext->encode,pcontext);
}
#ifndef __ANDROID__
YangVideoDecoder* YangDecoderFactory::createFfmpegVideoDecoder(YangVideoCodec paet,YangVideoInfo *pcontext){
	return new YangVideoDecoderFfmpeg(pcontext,paet);
}
#endif
YangVideoDecoder* YangDecoderFactory::createVideoDecoder(YangVideoCodec paet,YangVideoInfo *pcontext){
	if (paet == Yang_VED_264)				{
		if(pcontext->videoDecHwType==0){
#if	Yang_Using_H264Decoder
            return new YangH264DecoderSoft();
#else
           return new YangVideoDecoderFfmpeg(pcontext,paet);
#endif
		}else{
#ifndef __ANDROID__
		return new YangVideoDecoderFfmpeg(pcontext,paet);
#endif
		}
	}
#ifndef __ANDROID__
	if (paet == Yang_VED_265)				return new YangVideoDecoderFfmpeg(pcontext,paet);
#endif
	//de264=new YangVideoDecoderIntel();
	// if(p_decType==Type_Nvdia)   de264= new YangH264DecoderNv();
#if YangLibva
		return new YangVideoDecoderIntel();
#else
		return NULL;
#endif
}
	YangVideoDecoder* YangDecoderFactory::createVideoDecoder(YangVideoInfo *pcontext){
		YangVideoCodec maet=Yang_VED_264;
		if(pcontext->videoDecoderType==Yang_VED_265) maet=Yang_VED_265;
			//if(pcontext->videoDecoderType==1) maet=Type_Dec_Ffmpeg;
			//if(pcontext->audioDecoderType==2) maet=Yang_AE_SPEEX;
			return createVideoDecoder(maet,pcontext);
	}
