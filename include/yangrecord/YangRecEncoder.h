//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGRECORD_SRC_YANGRECENCODER_H_
#define SRC_YANGRECORD_SRC_YANGRECENCODER_H_
#include "yangencoder/YangAudioEncoderHandle.h"
#include "yangencoder/YangVideoEncoderHandle.h"
#include "yangutil/buffer/YangAudioBuffer.h"
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangutil/buffer/YangVideoEncoderBuffer.h"

class YangRecEncoder {
public:
	YangRecEncoder(YangContext* pcontext);
	virtual ~YangRecEncoder();
public:
	void initVideoEncoder();
	void initAudioEncoder();
	void startAudioEncoder();
	void startVideoEncoder();

	void setInAudioBuffer(YangAudioBuffer *pal);
	void setInVideoBuffer(YangVideoBuffer *pvl);
	YangAudioEncoderBuffer * getOutAudioBuffer();
	YangVideoEncoderBuffer * getOutVideoBuffer();
	YangVideoMeta * getOutVideoMetaData();
	YangVideoEncoderHandle *m_ve=NULL;
	YangAudioEncoderHandle *m_ae=NULL;
private:
	YangAudioInfo *m_audio;
	YangVideoInfo *m_video;
	YangVideoEncInfo *m_enc;
	YangContext* m_context;
	YangVideoMeta *m_vmd;
	YangAudioEncoderBuffer *m_out_auidoBuffer;
	YangVideoEncoderBuffer *m_out_videoBuffer;

};

#endif /* SRC_YANGRECORD_SRC_YANGRECENCODER_H_ */
