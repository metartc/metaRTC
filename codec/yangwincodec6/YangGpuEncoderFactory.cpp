//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "yangencoder/YangGpuEncoderFactory.h"
#include "YangEncoderGpu.h"
YangGpuEncoderFactory::YangGpuEncoderFactory()
{

}
YangGpuEncoderFactory::~YangGpuEncoderFactory()
{

}

YangVideoEncoder* YangGpuEncoderFactory::createGpuEncoder(){
    return new YangEncoderGpu();
}
