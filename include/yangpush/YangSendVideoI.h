//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGPUSH_YANGSENDVIDEOI_H_
#define INCLUDE_YANGPUSH_YANGSENDVIDEOI_H_
#include <stdint.h>
class YangSendVideoI{
public:
	YangSendVideoI(){};
	virtual ~YangSendVideoI(){};
	virtual void putVideoRgba(uint8_t* data,int len,int64_t timestamp)=0;
	virtual void putVideoI420(uint8_t* data,int len,int64_t timestamp)=0;
};




#endif /* INCLUDE_YANGPUSH_YANGSENDVIDEOI_H_ */
