//
// Copyright (c) 2019-2026 yanggaofeng
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


typedef struct{
	yangbool auHasVCL;
	yangbool auIsIDR;
	uint32_t auSize;
}YangH265Au;

#endif /* YANGENCODER_INCLUDE_YANGENCODER_H_ */
