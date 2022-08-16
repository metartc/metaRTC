//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGRECORD_INCLUDE_YANGRECORDAPP_H_
#define YANGRECORD_INCLUDE_YANGRECORDAPP_H_
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangaudiodev/YangAudioCapture.h>
#include "YangMp4FileApp.h"
#include "YangRecordCapture.h"

class YangRecordApp {
public:
	YangRecordApp(YangContext* pcontext);
	virtual ~YangRecordApp();
	YangRecordCapture *m_cap;
	YangMp4FileApp *m_rec;
	void init();
	void recordFile(char* filename);
	void stopRecord();
	void pauseRecord();
	void resumeRecord();

private:
	YangContext* m_context;


};

#endif /* YANGRECORD_INCLUDE_YANGRECORDAPP_H_ */
