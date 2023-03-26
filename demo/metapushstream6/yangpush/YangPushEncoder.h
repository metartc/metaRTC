//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGAPP_YangPushEncoder_H_
#define YANGAPP_YangPushEncoder_H_

#include <yangutil/buffer/YangAudioBuffer.h>
#include <yangutil/buffer/YangAudioEncoderBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <yangutil/buffer/YangVideoBuffer.h>

#include <yangencoder/YangAudioEncoderHandle.h>
#include <yangencoder/YangVideoEncoderHandle.h>


class YangPushEncoder {
public:
	YangPushEncoder(YangContext *pcontext);
	virtual ~YangPushEncoder();
public:
	void setVideoInfo(YangVideoInfo* pvideo);
	void initVideoEncoder();
	void initAudioEncoder();
	void startAudioEncoder();
	void startVideoEncoder();
	void setInAudioBuffer(YangAudioBuffer *pal);
	void setInVideoBuffer(YangVideoBuffer *pvl);
	YangAudioEncoderBuffer * getOutAudioBuffer();
	YangVideoEncoderBuffer * getOutVideoBuffer();
	YangVideoMeta * getOutVideoMetaData();
	void stopAll();
	void sendMsgToEncoder(YangRequestType req);

	void deleteVideoEncoder();
private:

	YangVideoEncoderHandle *m_ve;
	YangAudioEncoderHandle *m_ae;

	YangAudioEncoderBuffer *m_out_auidoBuffer;
	YangVideoEncoderBuffer *m_out_videoBuffer;

	YangContext *m_context;
	YangVideoMeta *m_vmd;
	YangVideoInfo* m_videoInfo;
};

#endif /* YANGAPP_YANGENCODERAPP_H_ */
