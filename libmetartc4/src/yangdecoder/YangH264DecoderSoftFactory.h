//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGH264DECODERSOFTFACTORY_H_
#define SRC_YANGH264DECODERSOFTFACTORY_H_
#include "yangdecoder/YangVideoDecoder.h"
YangVideoDecoder* yang_createH264Decoder();
class YangH264DecoderSoftFactory {
public:
	YangH264DecoderSoftFactory();
	virtual ~YangH264DecoderSoftFactory();
	YangVideoDecoder* createH264Decoder();
};

#endif /* SRC_YANGH264DECODERSOFTFACTORY_H_ */
