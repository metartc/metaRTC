//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGENCODER_INCLUDE_YANGENCODER_H_
#define YANGENCODER_INCLUDE_YANGENCODER_H_
#include <stdint.h>
class YangEncoderCallback{
public:
	YangEncoderCallback(){};
	virtual ~YangEncoderCallback(){};
	virtual void onVideoData(YangFrame* pframe)=0;
	virtual void onAudioData(YangFrame* pframe)=0;
};




#endif /* YANGENCODER_INCLUDE_YANGENCODER_H_ */
