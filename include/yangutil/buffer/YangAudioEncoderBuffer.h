//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangAudioEncoderBuffer_H
#define YangAudioEncoderBuffer_H

#include <yangutil/buffer/YangMediaBuffer.h>

class YangAudioEncoderBuffer: public YangMediaBuffer {
public:
	YangAudioEncoderBuffer(int32_t paudioCacheNum);
	~YangAudioEncoderBuffer(void);
	void reset();

	void putAudio(YangFrame* audioFrame);
	void getAudio(YangFrame* audioFrame);
	uint8_t* getAudioRef(YangFrame* pframe);
	void putPlayAudio(YangFrame* pframe);
	void getPlayAudio(YangFrame* audioFrame);

private:

};

#endif
