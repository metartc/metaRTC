//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef YANGGPUENCODERFACTORY_H
#define YANGGPUENCODERFACTORY_H

#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/yangavinfotype.h>
class YangGpuEncoderFactory
{
public:

    YangGpuEncoderFactory();
    ~YangGpuEncoderFactory();
    YangVideoEncoder* createGpuEncoder();

};

#endif // YANGGPUENCODERFACTORY_H
