/*
 * YangVrHandle.h
 *
 *  Created on: 2020年10月29日
 *      Author: yang
 */

#ifndef YANGRECLIVING_SRC_YANGVRHANDLE_H_
#define YANGRECLIVING_SRC_YANGVRHANDLE_H_
#include <yangrecliving/YangLivingType.h>
#include <vector>
#include "yangutil/buffer/YangVideoBuffer.h"

class YangVrHandle {
public:
	YangVrHandle();
	virtual ~YangVrHandle();
	 virtual void setVideoCount(int32_t vcount)=0;
	 virtual void setRecordModule(int32_t mo)=0;;
	 virtual void setHzhtype(int32_t ttype)=0;;
	 virtual void startRecordWave(char* filename)=0;;
	 virtual void stopRecordWave()=0;;
	 virtual void initPara(YangRecordContext *pra)=0;;
	 virtual void initAll()=0;;
	 virtual void reset()=0;;


	 virtual void startRecordLiving()=0;;
	 virtual void stopRecordLiving()=0;;
	 virtual void startFilmLiving()=0;;
	 virtual void stopFilmLiving()=0;;
	 virtual void startMultiLiving()=0;;
	 virtual void stopMultiLiving()=0;;


	// virtual void closeAll()=0;;
	 virtual void pause()=0;;
	 virtual void resume()=0;;
	 virtual void change(int32_t st)=0;;
	 virtual int32_t isValid(int32_t p_vtype)=0;;
	 virtual std::vector<YangVideoBuffer*>* getPreVideoBuffer()=0;;


};

#endif /* YANGRECLIVING_SRC_YANGVRHANDLE_H_ */
