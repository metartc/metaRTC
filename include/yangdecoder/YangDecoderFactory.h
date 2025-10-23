//
// Copyright (c) 2019-2025 yanggaofeng
//
#ifndef YANGDECODER_INCLUDE_YANGDECODERFACTORY_H_
#define YANGDECODER_INCLUDE_YANGDECODERFACTORY_H_
#include "yangdecoder/YangAudioDecoder.h"
#include "yangdecoder/YangVideoDecoder.h"


class YangDecoderFactory {
public:
	YangDecoderFactory();
	virtual ~YangDecoderFactory();

    YangAudioDecoder* createAudioDecoder(YangAudioCodec acodec,YangAudioParam *audioParam);
    YangAudioDecoder* createAudioDecoder(YangAudioParam *audioParam);
    YangVideoDecoder* createVideoDecoder(YangVideoCodec vcodec,YangVideoInfo *videoInfo);
    YangVideoDecoder* createFfmpegVideoDecoder(YangVideoCodec vcodec,YangVideoInfo *videoInfo);
    YangVideoDecoder* createVideoDecoder(YangVideoInfo *videoInfo);
};

#endif /* YANGDECODER_INCLUDE_YANGDECODERFACTORY_H_ */
