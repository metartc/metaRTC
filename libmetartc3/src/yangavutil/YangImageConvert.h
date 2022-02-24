//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGAVUTIL_SRC_YANGIMAGECONVERT_H_
#define YANGAVUTIL_SRC_YANGIMAGECONVERT_H_
#include <stdint.h>

 struct RGB24{
        uint8_t    b;
        uint8_t    g;
        uint8_t    r;
} ;
class YangImageConvert {
public:
	YangImageConvert();
	virtual ~YangImageConvert();
	void RGB24_TO_YV12(uint8_t* yv12,uint8_t* rgb24,int32_t w,int32_t h);
	void RGB24_To_I420(  uint8_t *RGBbuf,  uint8_t *YUV,  int32_t width,  int32_t height );
	void YUY2_To_RGB24(uint8_t *YUY2buff,uint8_t *RGBbuff,unsigned long dwSize);
	int32_t YUVBlending(void* pBGYUV, void* pFGYUV, int32_t width, int32_t height, bool alphaBG, bool alphaFG) ;
	void plusAuthor(uint8_t* src,uint8_t * dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight,int32_t model);
	void plusYuy2(uint8_t* src,uint8_t * dest,int32_t srcWidth,int32_t srcHeight,int32_t destWidth,int32_t destHeight,int32_t model);
private:
	uint8_t clip255(long v);
};

#endif /* YANGAVUTIL_SRC_YANGIMAGECONVERT_H_ */
