//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangVideoDecoderBuffer__
#define __YangVideoDecoderBuffer__
#include <string.h>
#include <stdint.h>
#include <yangutil/buffer/YangMediaBuffer.h>
class YangVideoDecoderBuffer:public YangMediaBuffer
{
public:
	YangVideoDecoderBuffer(int num);
	~YangVideoDecoderBuffer(void);

	void putEVideo(YangFrame* pframe);
	void getEVideo(YangFrame* pframe);
private:


};
#endif
