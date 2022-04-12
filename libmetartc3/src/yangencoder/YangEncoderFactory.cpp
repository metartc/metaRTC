//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangencoder/YangAudioEncoderAac.h>
#include <yangencoder/YangAudioEncoderMp3.h>
#include <yangencoder/YangAudioEncoderMp3.h>
#include <yangencoder/YangAudioEncoderSpeex.h>
#include <yangencoder/YangAudioEncoderMp3.h>
#include <yangencoder/YangFfmpegEncoderMeta.h>
#include "yangencoder/YangEncoderFactory.h"

#include "YangAudioEncoderMp3.h"
#include "YangAudioEncoderOpus.h"
#include <yangencoder/YangGpuEncoderFactory.h>
#include <yangencoder/YangH264EncoderIntel.h>
#include <yangencoder/YangH264EncoderMeta.h>
#include <yangencoder/YangH264EncoderSoft.h>
#include <yangencoder/YangH265EncoderMeta.h>
#include <yangencoder/YangH265EncoderSoft.h>
#include <yangencoder/YangVideoEncoderFfmpeg.h>
#include <yangencoder/YangEncoderMediacodec.h>
YangEncoderFactory::YangEncoderFactory() {


}

YangEncoderFactory::~YangEncoderFactory() {

}
YangVideoEncoderMeta* YangEncoderFactory::createVideoEncoderMeta(YangVideoInfo *pcontext){
	if(pcontext->videoEncHwType>0) return new YangFfmpegEncoderMeta();
	if(pcontext->videoEncoderType==0) return new YangH264EncoderMeta();
	if(pcontext->videoEncoderType==1) return new YangH265EncoderMeta();
	return new YangH264EncoderMeta();
}


YangAudioEncoder *YangEncoderFactory::createAudioEncoder(YangAudioCodec paet,YangAudioInfo *pcontext){
	if(paet==Yang_AED_MP3) return new YangAudioEncoderMp3();
	if(paet==Yang_AED_SPEEX) return new YangAudioEncoderSpeex();
	if(paet==Yang_AED_OPUS) return new YangAudioEncoderOpus();
	return new YangAudioEncoderAac();
}
YangAudioEncoder *YangEncoderFactory::createAudioEncoder(YangAudioInfo *pcontext){
	YangAudioCodec maet=Yang_AED_AAC;

	if(pcontext->audioEncoderType==1) maet=Yang_AED_MP3;
	if(pcontext->audioEncoderType==2) maet=Yang_AED_SPEEX;
	if(pcontext->audioEncoderType==3) maet=Yang_AED_OPUS;
	return createAudioEncoder(maet,pcontext);
}

YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoCodec paet,YangVideoInfo *pcontext){
#ifdef __ANDROID__
	if(pcontext->videoEncHwType==0)
		return new YangH264EncoderSoft();
	else
		return new YangEncoderMediacodec();
#else
	if(pcontext->videoEncHwType==0){
		//printf("\n*********************pate===%d\n",paet);
		if (paet == Yang_VED_264)		return  new YangH264EncoderSoft();
		if (paet == Yang_VED_265)		return  new YangH265EncoderSoft();
	}else{
 #if Yang_GPU_Encoding
                YangGpuEncoderFactory gf;
               return gf.createGpuEncoder();
#else
					return new YangVideoEncoderFfmpeg(paet,pcontext->videoEncHwType);
#endif
	}

		return new YangH264EncoderSoft();
#endif
}
	YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoInfo *pcontext){
		YangVideoCodec maet=Yang_VED_264;
			if(pcontext->videoEncoderType==0) maet=Yang_VED_264;
			if(pcontext->videoEncoderType==1) maet=Yang_VED_265;
			//if(pcontext->audioEncoderType==2) maet=Yang_AE_SPEEX;
			return createVideoEncoder(maet,pcontext);
	}
