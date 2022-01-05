/*
 * yangrecordtype.h
 *
 *  Created on: 2020年10月12日
 *      Author: yang
 */

#ifndef YANGRECLIVING_INCLUDE_YANGRECORDTYPE_H_
#define YANGRECLIVING_INCLUDE_YANGRECORDTYPE_H_
#include <yangutil/yangavinfotype.h>

struct YangRecordParam{
	char app[20];
	char filePath[128];
	int32_t roomId;
	int32_t hzhType;
	int32_t isMp4;
	int32_t fileTimeLen;
	int32_t livingModel;
	int32_t recordModel;
	int32_t mode;
};
class YangRecordContext:public YangContext{
public:
	YangRecordContext();
	~YangRecordContext();
public:
	void initExt(void *filename);
	YangRecordParam record;
	//YangCameraParam camera;
	int32_t createFile;
	int32_t createRtmp;

	int32_t createFile3;
	int32_t createRtmp3;
	char filename[50];
	char filenames[50];
	char bgFilename[128];
};
enum YangRecodeModuleType{
	Yang_Record_Film=1,
	Yang_Record_Hzh,
	Yang_Record_Res,
	Yang_Record_Both
};


#endif /* YANGRECLIVING_INCLUDE_YANGRECORDTYPE_H_ */
