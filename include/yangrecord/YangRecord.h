/*
 * YangRecord.h
 *
 *  Created on: 2020年10月11日
 *      Author: yang
 */

#ifndef YANGRECORD_INCLUDE_YANGRECORD_H_
#define YANGRECORD_INCLUDE_YANGRECORD_H_

#include "yangutil/sys/YangTime.h"
#include "YangFlvWrite.h"

#include "YangMp4File.h"

struct YangMp4Para {
	YangVideoMeta *vmd;
	uint8_t asc[10];
	char *fileName;
	int32_t ascLen;
};
class YangRecord {
public:
	YangRecord(YangAudioInfo *paudio, YangVideoInfo *pvideo,
			YangVideoEncInfo *penc);
	~YangRecord(void);

	int32_t isMp4;
	void initPara(YangVideoMeta *p_vmd, char *filename, int32_t p_isMp4);
	void setFileTimeLen(int32_t ptlen_min);
	void pauseRec();
	void resumeRec();
	void writeVideoData(YangFrame* videoFrame);
	void writeAudioData(YangFrame* audioFrame);
	void closeRec();
	YangMp4File *mp4;
	YangFlvWriter *flv;
	YangAudioInfo *m_audio;
	YangVideoInfo *m_video;
	YangVideoEncInfo *m_enc;

	int64_t oldalltick, alltick;
	int32_t curVideoTimestamp;
	long videoDestLen;
	int64_t preVideotimestamp, basestamp, minusStamp;

private:
	int64_t m_prePauseTime, m_afterPauseTime;
	int64_t m_pauseTime;
	int64_t m_alltime1;
	int32_t m_alltime;
	int32_t m_fileTimeLen;
	int32_t m_isCreateNewFile;
	//YangTime m_time;
	YangMp4Para m_mp4Para;
	void createNewfile();
	void createFile(char *filename);
	void initRecPara();
	int32_t m_fileId;
	//YangAudioFrame m_audioFrame;
	//YangVideoFrame m_videoFrame;
};

#endif /* YANGRECORD_INCLUDE_YANGRECORD_H_ */
