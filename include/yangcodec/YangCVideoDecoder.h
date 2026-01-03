//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGCODEC_YANGCVIDEODECODER_H_
#define INCLUDE_YANGCODEC_YANGCVIDEODECODER_H_
#include "YangCodec.h"
typedef struct{
	void* session;
	int32_t (*init)(void* session,YangCodecCallback* callback);
	void (*parseHeader)(void* session,uint8_t *p, int32_t  pLen, int32_t  *pwid,
			int32_t  *phei, int32_t  *pfps);
	int32_t (*decode)(void* session,YangFrame* pframe);

}YangCVideoDecoder;

#endif /* INCLUDE_YANGCODEC_YANGCVIDEODECODER_H_ */
