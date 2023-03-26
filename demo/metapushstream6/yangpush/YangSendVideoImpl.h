//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGPUSH_YANGSENDVIDEOIMPL_H_
#define SRC_YANGPUSH_YANGSENDVIDEOIMPL_H_
#include <yangutil/buffer/YangVideoBuffer.h>
#include <yangavutil/video/YangYuvConvert.h>
#include <yangpush/YangSendVideoI.h>
class YangSendVideoImpl:public YangSendVideoI {
public:
	YangSendVideoImpl();
	virtual ~YangSendVideoImpl();
	void putVideoRgba(uint8_t* data,int len,int64_t timestamp);
	void putVideoI420(uint8_t* data,int len,int64_t timestamp);
	void init(int32_t wid,int32_t hei);


	YangVideoBuffer* m_outPreVideoBuffer;
	YangVideoBuffer* m_outVideoBuffer;
private:
	YangYuvConvert m_yuv;
	YangFrame m_videoFrame;
	uint8_t* m_buf;

	int32_t m_width;
	int32_t m_height;
	int32_t m_len;


};

#endif /* SRC_YANGPUSH_YANGSENDVIDEOIMPL_H_ */
