//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRECLIVING_INCLUDE_YANGRECORDUTILFACTORY_H_
#define YANGRECLIVING_INCLUDE_YANGRECORDUTILFACTORY_H_
#include <yangrecliving/YangLivingType.h>
#include "YangVrHandle.h"
#include "YangLivingHandle.h"
#include "YangScreenHandle.h"
#include "yangutil/sys/YangSysMessageHandle.h"


class YangRecordUtilFactory {
public:
	YangRecordUtilFactory();
	virtual ~YangRecordUtilFactory();
	void createIni(const char* p_filename,YangRecordContext *pcontext);
	void createIni(const char* p_filename,YangRecordParam *pcontext);
	YangVrHandle* createRecordHandle(YangRecordContext *pcontext);
	YangLivingHandle* createLivingHandle(YangRecordContext *pcontext);
	//YangScreenHandle* createScreenHandle(YangRecordContext *pcontext);

	YangSysMessageHandle* createVrRecMessageHandle(YangRecordContext *pcontext);
	YangSysMessageHandle* createRecMessageHandle(YangRecordContext *pcontext);

	YangVrHandle* getVrHandle(YangSysMessageHandle* pms);
	YangLivingHandle* getLivingHandle(YangSysMessageHandle* pms);
};

#endif /* YANGRECLIVING_INCLUDE_YANGRECORDUTILFACTORY_H_ */
