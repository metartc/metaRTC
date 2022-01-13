#ifndef _YangRecordMp4_H
#define _YangRecordMp4_H
#include "yangutil/sys/YangThread.h"
#include "yangutil/sys/YangTime.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "YangFlvWrite.h"
#include "YangMp4File.h"
struct YangMp4FilePara{
	YangVideoMeta *vmd;
	uint8_t asc[10];
	 char *fileName;
	int32_t ascLen;
};
class YangRecordMp4:public YangThread
{
public:
	YangRecordMp4(YangAudioInfo *paudio,YangVideoInfo *pvideo,YangVideoEncInfo *penc);
	~YangRecordMp4(void);

	int32_t isMp4;
	int32_t m_isStart;
	void setInVideoBuffer(YangVideoEncoderBuffer *pbuf);
	void setInAudioBuffer(YangAudioEncoderBuffer *pbuf);
	void initPara(YangVideoMeta *p_vmd, char *filename,int32_t p_isMp4);
	void setFileTimeLen(int32_t ptlen_min);
	void pauseRec();
	void resumeRec();
	void stop();
protected:
	void run();
private:
	void startLoop();
	void stopLoop();
	void closeRec();

	YangMp4File *mp4;
	YangFlvWriter *flv;
	YangAudioInfo *m_audio;
	YangVideoInfo *m_video;
	YangVideoEncInfo *m_enc;
	YangAudioEncoderBuffer *m_in_audioBuffer;
	YangVideoEncoderBuffer *m_in_videoBuffer;
	int64_t oldalltick,alltick;
	int32_t curVideoTimestamp;
	long videoDestLen;
	int32_t frameType;
	uint8_t *srcVideoSource;
	uint8_t *srcAudioSource;
	 //int32_t audioBufLen;
	 int64_t m_videoTimestamp,m_preVideotimestamp,m_startStamp,m_mp4Stamp;
	// int32_t videoBufLen;

	int32_t m_isConvert;
	void writeVideoData();
	void writeAudioData();

private:
	int64_t m_prePauseTime,m_afterPauseTime;
	int64_t m_pauseTime;
	int64_t m_alltime1;
	int32_t m_alltime;
	int32_t m_fileTimeLen;
	int32_t m_isCreateNewFile;
	YangFrame m_videoFrame;
	YangFrame m_audioFrame;
	YangMp4FilePara m_mp4Para;
	void createNewfile();
	void createFile(char* filename);
	void initRecPara();
	int32_t m_fileId;
};

#endif

