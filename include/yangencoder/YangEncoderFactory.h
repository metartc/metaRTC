/*
 * YangAudioEncoderFactory.h
 *
 *  Created on: 2020年9月3日
 *      Author: yang
 */

#ifndef YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_
#define YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_
#include <yangutil/yangavinfotype.h>
#include "YangVideoEncoder.h"
#include "yangencoder/YangAudioEncoder.h"
#include <yangavutil/video/YangVideoEncoderMeta.h>


class YangEncoderFactory {
public:
	YangEncoderFactory();
	virtual ~YangEncoderFactory();
	YangAudioEncoder* createAudioEncoder(YangAudioEncDecType paet,YangAudioInfo *pcontext);
	YangAudioEncoder* createAudioEncoder(YangAudioInfo *pcontext);
	YangVideoEncoder* createVideoEncoder(YangVideoEncDecType paet,YangVideoInfo *pcontext);
	YangVideoEncoder* createVideoEncoder(YangVideoInfo *pcontext);
	YangVideoEncoderMeta* createVideoEncoderMeta(YangVideoInfo *pcontext);
};

#endif /* YANGENCODER_INCLUDE_YANGVIDEOENCODERFACTORY_H_ */
