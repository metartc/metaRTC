//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGSTREAM_YANGSTREAMCAPTURE_H_
#define INCLUDE_YANGSTREAM_YANGSTREAMCAPTURE_H_
#include "YangAudioStreamCapture.h"
#include "YangVideoStreamCapture.h"
class YangStreamCapture {
public:
	YangStreamCapture();
	virtual ~YangStreamCapture();
public:
	uint8_t* getVideoData();
	int32_t getVideoLen();
	int32_t getVideoFrametype();
	int64_t getVideoTimestamp();


	uint8_t* getAudioData();
	int32_t getAudioLen();
	int64_t getAudioTimestamp();
	YangAudioCodec getAudioType();


	void initVideo(int32_t transtype);
	void setVideoData(YangFrame* videoFrame,YangVideoCodec videoType=Yang_VED_264);
	void setVideoMeta(uint8_t* p,int32_t plen,YangVideoCodec videoType=Yang_VED_264);
	void setMetaTimestamp(int64_t timestamp);
	void setVideoFrametype(int32_t frametype);


	void setAudioData(YangFrame* audioFrame);
	void setAudioMetaData(uint8_t* p,int32_t plen);
	void setAudioFrametype(int32_t frametype);
	void initAudio(int32_t transType,int32_t sample,int32_t channel,YangAudioCodec audioType);
public:
	YangAudioStreamCapture m_audio;
	YangVideoStreamCapture m_video;
};

#endif /* INCLUDE_YANGSTREAM_YANGSTREAMCAPTURE_H_ */
