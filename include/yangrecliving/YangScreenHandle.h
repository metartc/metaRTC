//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGRECLIVING_YANGSCREENHANDLE_H_
#define INCLUDE_YANGRECLIVING_YANGSCREENHANDLE_H_
#include <yangrecliving/YangLivingType.h>
#include <vector>
#include "yangutil/buffer/YangVideoBuffer.h"
class YangScreenHandle
{
public:

	YangScreenHandle();
	 virtual ~YangScreenHandle();

		virtual void init()=0;
		virtual void recordFile()=0;
		virtual void stopRecord()=0;
		//virtual void pauseRecord()=0;
		//virtual void resumeRecord()=0;
		virtual YangVideoBuffer* getPrebuffer()=0;

		virtual void startScreen()=0;
		virtual void stopScreen()=0;

};




#endif /* INCLUDE_YANGRECLIVING_YANGSCREENHANDLE_H_ */
