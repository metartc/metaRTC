#ifndef YANGDECODER_SRC_YangDecoderH264_H_
#define YANGDECODER_SRC_YangDecoderH264_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <yangutil/yangtype.h>
#include "YangH264Dec.h"

#if !Yang_Using_H264Decoder_So

class YangDecoderH264{
public:
	YangDecoderH264();
	~YangDecoderH264();
	void init(uint8_t *headers, int headerLen);
	int decode(int isIframe,unsigned char *pData, int nSize,enum YangYuvType pyuvType,
			unsigned char *dest, int *pnFrameReturned);
	void decode_close();
	YangH264DecContext *m_context;
private:

};
#endif
#endif /* YANGDECODER_SRC_YANGH2645VIDEODECODERFFMPEG_H_ */
