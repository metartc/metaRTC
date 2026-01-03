//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_
#define YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_
#include <yangutil/yangavinfotype.h>
#include "YangVideoEncoder.h"
#include "yangencoder/YangAudioEncoder.h"


class YangEncoderFactory {
public:
	YangEncoderFactory();
	virtual ~YangEncoderFactory();
    YangAudioEncoder* createAudioEncoder(YangAudioCodec acodec,YangAudioInfo *audioInfo);
    YangAudioEncoder* createAudioEncoder(YangAudioInfo *audioInfo);
    YangVideoEncoder* createVideoEncoder(YangVideoCodec vcodec,YangVideoInfo *videoInfo);
    YangVideoEncoder* createVideoEncoder(YangVideoInfo *videoInfo);
};

#endif /* YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_ */
