/*
 * YangEncoder.h
 *
 *  Created on: 2020年10月3日
 *      Author: yang
 */

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
	YangRecEncoder(YangAudioInfo *paudio,YangVideoInfo *pvideo,YangVideoEncInfo *penc);
	virtual ~YangRecEncoder();
public:
	void initVideoEncoder();
	void initAudioEncoder();
	void startAudioEncoder();
	void startVideoEncoder();
	//void setAVList(YangAudioList *pal,YangVideoList *pvl);
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
	YangVideoMeta *m_vmd;
	YangAudioEncoderBuffer *m_out_auidoBuffer;
	YangVideoEncoderBuffer *m_out_videoBuffer;

};

#endif /* SRC_YANGRECORD_SRC_YANGRECENCODER_H_ */
