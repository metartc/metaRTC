
#ifndef YANGSTREAM_INCLUDE_YANGAUDIOSTREAMDATA_H_
#define YANGSTREAM_INCLUDE_YANGAUDIOSTREAMDATA_H_
#include <yangutil/yangavinfotype.h>
#include <stdint.h>
class YangAudioStreamCapture{
public:
	YangAudioStreamCapture();
	virtual ~YangAudioStreamCapture();
	void setAudioData(YangFrame* audioFrame);
	void setAudioMetaData(uint8_t* p,int32_t plen);
	void setFrametype(int32_t frametype);
	void init(int32_t transType,int32_t sample,int32_t channel,YangAudioEncDecType audioType);
	uint8_t* getAudioData();
	int64_t getRtmpTimestamp();
	int64_t getTsTimestamp();
public:
	int64_t getTimestamp();
	int32_t getAudioLen();
	int32_t getFrametype();
	YangAudioEncDecType getAudioType();

private:
	uint8_t* m_audioBufs;
	uint8_t* m_audioBuffer;
	int32_t m_audioHeaderLen;
	uint8_t* m_src;
	int32_t m_srcLen;
	int32_t m_audioLen;

	int64_t atime ;
	int64_t m_unitAudioTime;
	double atime1;
	double perSt ;

	int32_t m_transType;
	int32_t m_frametype;
	YangAudioEncDecType m_audioType;
};

#endif /* YANGSTREAM_INCLUDE_YANGAUDIOSTREAMDATA_H_ */
