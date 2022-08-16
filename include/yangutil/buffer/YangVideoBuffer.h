//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangVideoBuffer__
#define __YangVideoBuffer__
#include <string.h>
#include <yangutil/buffer/YangMediaBuffer.h>


#define yang_get_videoBuffer(x) new YangVideoBuffer(x->width,x->height,x->videoCaptureFormat==YangYuy2?16:12,x->bitDepth==8?1:2)
class YangVideoBuffer:public YangMediaBuffer
{
public:
	YangVideoBuffer(int32_t pBitDepthLen);

	YangVideoBuffer(int32_t pwid,int32_t phei,YangYuvType ptype,int32_t pBitDepthLen);
	~YangVideoBuffer(void);
	void initTemp();
	void init(int32_t pwid,int32_t phei,YangYuvType ptype);


	void reset();
	int32_t isPreview=0;
	int32_t m_width;
	int32_t m_height;
	int32_t m_length;
	int32_t m_frames;
	int32_t m_bitDepthLen;
	void putVideo(YangFrame* pframe);

	void getVideo(YangFrame* pframe);
	uint8_t * getVideoRef(YangFrame* pframe);
	int64_t getTimestamp(int64_t *timestamp);
	int64_t getNextTimestamp();
	YangFrame* getCurVideoFrame();

protected:


private:
	int32_t m_headerLen;


};
#endif
