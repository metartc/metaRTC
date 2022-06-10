//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRTMP_INCLUDE_YangRecordLivingHandle_H_
#define YANGRTMP_INCLUDE_YangRecordLivingHandle_H_


#include <yangrecliving/YangLivingType.h>
#include <vector>
#include "yangutil/buffer/YangVideoBuffer.h"
class YangLivingHandle
{
public:
	 YangLivingHandle();
	 virtual ~YangLivingHandle();

         virtual void init()=0;
         virtual void recordFile()=0;
         virtual void stopRecord()=0;
         virtual void pauseRecord()=0;
         virtual void resumeRecord()=0;
         virtual YangVideoBuffer* getPrebuffer()=0;

         virtual void startVr(char* pbgfile)=0;
         virtual void stopVr()=0;


};
#endif
