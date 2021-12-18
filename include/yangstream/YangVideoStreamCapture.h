#ifndef YANGSTREAM_INCLUDE_YANGVIDEOSTREAMDATA_H_
#define YANGSTREAM_INCLUDE_YANGVIDEOSTREAMDATA_H_
#include <yangutil/yangavinfotype.h>
#include "stdint.h"
struct YangSpsppsConf{
	uint8_t* sps;
	uint8_t* pps;
	int32_t spsLen;
	int32_t ppsLen;
};
class YangVideoStreamCapture {
public:
	YangVideoStreamCapture();
	virtual ~YangVideoStreamCapture();

	void init(int32_t transtype);
	void setVideoData(YangFrame* videoFrame,YangVideoEncDecType videoType=Yang_VED_264);
	void setVideoMeta(uint8_t* p,int32_t plen,YangVideoEncDecType videoType=Yang_VED_264);
	void setFrametype(int32_t frametype);
	void setMetaTimestamp(int64_t timestamp);
	int64_t getRtmpTimestamp();
	int64_t getTsTimestamp();
	int64_t getMetaTimestamp();


public:
	//uint8_t* getVideoMeta();
	//int32_t getVideoMetaLen();
	uint8_t* getVideoData();
	YangSpsppsConf* getSpsppsConf();

	int32_t getFrametype();
	int32_t getVideoLen();
	int64_t getTimestamp();
protected:

	void initEncType(YangVideoEncDecType videoType,int32_t frametype);
	void initSpspps(uint8_t *p);
private:
	uint8_t* m_videoBufs;
	uint8_t* m_videoBuffer;
	//uint8_t* m_videoMeta;
	YangSpsppsConf *m_spsppsConf;
	int32_t m_frametype;
	int32_t m_videoLen;
	uint8_t* m_src;

	int32_t m_transType;
	int64_t videoTimestamp , baseTimestamp;
	int64_t curVideotimestamp ;
	int32_t preTimestamp ;

	int64_t m_metaTime;



};

#endif /* YANGSTREAM_INCLUDE_YANGVIDEOSTREAMDATA_H_ */
