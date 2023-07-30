//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGSDP_YANGAUDIOPAYLOAD_H_
#define SRC_YANGSDP_YANGAUDIOPAYLOAD_H_
#include <yangsdp/YangCodecPayload.h>

typedef struct
{
	int32_t minptime;
	int16_t use_inband_fec;
	int16_t usedtx;
}SrsOpusParameter;

typedef struct{
	YangCodecPayload codecPayload;
	int32_t channel;
	SrsOpusParameter opus_param;

}YangAudioPayload;

#endif /* SRC_YANGSDP_YANGAUDIOPAYLOAD_H_ */
