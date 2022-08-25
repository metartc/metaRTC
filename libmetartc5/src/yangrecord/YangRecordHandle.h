//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRECORD_INCLUDE_YangRecordHandle_H_
#define YANGRECORD_INCLUDE_YangRecordHandle_H_
#include <yangcapture/YangMultiVideoCapture.h>
#include <yangaudiodev/YangAudioCapture.h>
#include <yangrecord/YangMp4FileApp.h>
#include <yangpush/YangPushCapture.h>

class YangRecordHandle {
public:
	YangRecordHandle(YangContext* pcontext);
	virtual ~YangRecordHandle();
	YangPushCapture *m_cap;
	YangMp4FileApp *m_rec;
	void init(YangPushCapture* pcap);
	void recordFile(char* filename);
	void stopRecord();
	//void pauseRecord();
	//void resumeRecord();

private:
	YangContext* m_context;


};

#endif /* YANGRECORD_INCLUDE_YangRecordHandle_H_ */
