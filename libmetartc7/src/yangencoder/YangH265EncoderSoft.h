//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGENCODER_SRC_YANGH265ENCODERSOFT_H_
#define YANGENCODER_SRC_YANGH265ENCODERSOFT_H_


#include <yangencoder/YangVideoEncoder.h>
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangutil/buffer/YangVideoEncoderBuffer.h>
#include <yangutil/sys/YangLoadLib.h>
#include <x265.h>



class YangH265EncoderSoft : public YangVideoEncoder {
public:
	YangH265EncoderSoft();
	virtual ~YangH265EncoderSoft();
	static void initX265Param(YangVideoInfo *pvp,YangVideoEncInfo *penc,x265_param *param);
	int32_t init(YangContext* pcontext,YangVideoInfo* pvideoInfo);
	void setVideoMetaData(YangVideoMeta *pvmd);
	void parseRtmpHeader(uint8_t *p, int32_t pLen, int32_t *pwid, int32_t *phei, int32_t *pfps);
	void sendMsgToEncoder(YangRtcEncoderMessage *msg);
private:
	int32_t m_inWidth;
	int32_t m_inHeight;
	int32_t m_outWidth;
	int32_t m_outHeight;
	int32_t m_sendKeyframe;

	int32_t encode(YangFrame* pframe, YangEncoderCallback* pcallback);
protected:
	bool m_hasHeader;
	int32_t hex2bin(char *str, char **hex);
	uint8_t *m_nal;
	int32_t nal_len, destLength;


private:
	uint8_t *m_buffer;

	int32_t m_frametype;

	x265_nal *m_265Nal;
	uint32_t m_i265Nal;
	x265_picture *m_265Pic;
	x265_encoder *m_265Handle;

private:
	YangLoadLib m_lib;
	void loadLib();
	void unloadLib();
	x265_param* (*yang_x265_param_alloc)(void);
	void (*yang_x265_param_free)(x265_param*);
	void (*yang_x265_param_default)(x265_param*);
	int32_t (*yang_x265_param_default_preset)(x265_param*, const char *preset,	const char *tune);
	int32_t (*yang_x265_param_apply_profile)(x265_param*, const char *profile);

	x265_picture* (*yang_x265_picture_alloc)();
	void (*yang_x265_picture_init)(x265_param *param, x265_picture *pic);
	int32_t (*yang_x265_encoder_encode)(x265_encoder*, x265_nal **pp_nal,uint32_t *pi_nal, x265_picture *pic_in, x265_picture *pic_out);
	void (*yang_x265_picture_free)(x265_picture *pic);

	void (*yang_x265_encoder_close)(x265_encoder*);
	x265_encoder* (*yang_x265_encoder_open)(x265_param*);
};

#endif /* YANGENCODER_SRC_YANGH265ENCODERSOFT_H_ */
