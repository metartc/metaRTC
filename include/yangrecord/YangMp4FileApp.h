/*
 * YangRecordHandle.h
 *
 *  Created on: 2020年10月4日
 *      Author: yang
 */

#ifndef SRC_YANGRECORD_SRC_YangMp4FileApp_H_
#define SRC_YANGRECORD_SRC_YangMp4FileApp_H_
#include <yangutil/yangavinfotype.h>
#include "yangrecord/YangRecEncoder.h"
#include "yangrecord/YangRecordMp4.h"

class YangMp4FileApp {
public:
	YangMp4FileApp(YangAudioInfo *paudio,YangVideoInfo *pvideo,YangVideoEncInfo *penc);
	virtual ~YangMp4FileApp();

	YangRecEncoder *m_enc;
	YangRecordMp4 *m_rec;

public:
	void init();
	void startRecordMp4(char *filename0,int32_t p_module,int32_t p_isMp4);
	void stopRecordMp4();
	void setInAudioBuffer(YangAudioBuffer *pbuf);
	void setInVideoBuffer(YangVideoBuffer *pbuf);
	void setFileTimeLen(int32_t ptlen_min);
	void pauseRecord();
	void resumeRecord();
private:
	YangAudioInfo *m_audio;
	YangVideoInfo *m_video;
	YangVideoEncInfo *m_encPara;
	int32_t m_isPause;
};

#endif /* SRC_YANGRECORD_SRC_YANGRECORDHANDLE_H_ */
