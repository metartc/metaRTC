//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGENCODER_YANGENCODERMAC_H_
#define SRC_YANGENCODER_YANGENCODERMAC_H_
#include <yangcodec/YangCVideoEncoder.h>
#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_create_ios_encoder(YangCVideoEncoder* encoder);
void yang_destroy_ios_encoder(YangCVideoEncoder* encoder);

#ifdef __cplusplus
}
#endif




#endif /* SRC_YANGENCODER_YANGENCODERMAC_H_ */

