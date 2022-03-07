//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangScreenCaptureI_H_
#define YangScreenCaptureI_H_
class YangScreenCaptureHandleI {
public:
	YangScreenCaptureHandleI(){}
	virtual ~YangScreenCaptureHandleI(){}
	int32_t m_width=0,m_height=0;
	virtual int32_t init()=0;
	virtual int32_t captureFrame(uint8_t* p)=0;
};

#endif
