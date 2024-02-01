//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangencoder/YangEncoderFactory.h>
#include <yangencoder/YangAudioEncoderAac.h>
#include <yangencoder/YangAudioEncoderMp3.h>
#include <yangencoder/YangAudioEncoderOpus.h>
#include <yangencoder/YangAudioEncoderSpeex.h>

#include <yangencoder/YangFfmpegEncoderMeta.h>
#include <yangencoder/YangGpuEncoderFactory.h>
#include <yangencoder/YangH265EncoderMeta.h>
#include <yangencoder/YangH265EncoderSoft.h>
#include <yangencoder/YangVideoEncoderFfmpeg.h>
#include <yangencoder/YangEncoderMediacodec.h>

#if Yang_Enable_Openh264
#include <yangencoder/YangOpenH264Encoder.h>
#else
#include <yangencoder/YangH264EncoderMeta.h>
#include <yangencoder/YangH264EncoderSoft.h>
#endif

YangEncoderFactory::YangEncoderFactory() {

}

YangEncoderFactory::~YangEncoderFactory() {

}
YangVideoEncoderMeta* YangEncoderFactory::createVideoEncoderMeta(
		YangVideoInfo *pcontext) {
#if Yang_Enable_Ffmpeg_Codec
	if (pcontext->videoEncHwType > 0)
		return new YangFfmpegEncoderMeta();
#endif
#if !Yang_Enable_Openh264
	if(pcontext->videoEncoderType==0) return new YangH264EncoderMeta();
#endif
	if (pcontext->videoEncoderType == 1)
		return new YangH265EncoderMeta();
#if Yang_Enable_Openh264
	return NULL;
#else
	return new YangH264EncoderMeta();
#endif
}

YangAudioEncoder* YangEncoderFactory::createAudioEncoder(YangAudioCodec paet,
		YangAudioInfo *pcontext) {
	if (paet == Yang_AED_MP3)
		return new YangAudioEncoderMp3();
	if (paet == Yang_AED_SPEEX)
		return new YangAudioEncoderSpeex();
	if (paet == Yang_AED_OPUS)
		return new YangAudioEncoderOpus();
	return new YangAudioEncoderAac();
}
YangAudioEncoder* YangEncoderFactory::createAudioEncoder(
		YangAudioInfo *pcontext) {
	YangAudioCodec maet = Yang_AED_AAC;

	if (pcontext->audioEncoderType == 1)
		maet = Yang_AED_MP3;
	if (pcontext->audioEncoderType == 2)
		maet = Yang_AED_SPEEX;
	if (pcontext->audioEncoderType == 3)
		maet = Yang_AED_OPUS;
	return createAudioEncoder(maet, pcontext);
}

YangVideoEncoder* YangEncoderFactory::createVideoEncoder(YangVideoCodec paet,
		YangVideoInfo *pcontext) {
#if Yang_OS_ANDROID
	if(pcontext->videoEncHwType==0){
#if Yang_Enable_Openh264
		return new YangOpenH264Encoder();
#else
		return  new YangH264EncoderSoft();
#endif
	}else{
		return new YangEncoderMediacodec();
	}
#else
	if (pcontext->videoEncHwType == 0) {
#if Yang_Enable_Openh264
		if (paet == Yang_VED_H264)
			return new YangOpenH264Encoder();
#else
		if (paet == Yang_VED_H264)		return  new YangH264EncoderSoft();
#endif
		if (paet == Yang_VED_H265)
			return new YangH265EncoderSoft();
	} else {
#if Yang_Enable_GPU_Encoding
                YangGpuEncoderFactory gf;
               return gf.createGpuEncoder();
#else
#if Yang_Enable_Ffmpeg_Codec
		return new YangVideoEncoderFfmpeg(paet, pcontext->videoEncHwType);
#endif
#endif
	}
#if Yang_Enable_Openh264
	return new YangOpenH264Encoder();
#else
			return  new YangH264EncoderSoft();
#endif

#endif
}
YangVideoEncoder* YangEncoderFactory::createVideoEncoder(
		YangVideoInfo *pcontext) {
	YangVideoCodec maet = Yang_VED_H264;
	if (pcontext->videoEncoderType == 0)
		maet = Yang_VED_H264;
	if (pcontext->videoEncoderType == 1)
		maet = Yang_VED_H265;

	return createVideoEncoder(maet, pcontext);
}
