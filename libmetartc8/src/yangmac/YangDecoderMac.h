//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGDECODERMAC_H_
#define SRC_YANGDECODERMAC_H_
#include <yangutil/yangavinfo.h>
#include <yangcodec/YangCVideoDecoder.h>
#ifdef __cplusplus
extern "C"{
#endif
	int32_t yang_create_ios_decoder(YangCVideoDecoder* decoder);
	void yang_destroy_ios_decoder(YangCVideoDecoder* decoder);
#ifdef __cplusplus
}
#endif

#endif /* SRC_YANGDECODERMAC_H_ */

