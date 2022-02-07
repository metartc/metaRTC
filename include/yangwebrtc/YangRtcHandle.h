//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_
#define INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_
#include <yangstream/YangStreamCapture.h>
#include <yangstream/YangStreamType.h>


class YangRtcHandle
{
public:
	YangRtcHandle();
	virtual ~YangRtcHandle();
	virtual void init(YangStreamConfig* pconf)=0;

	virtual int32_t connectRtcServer()=0;
	virtual int32_t disconnectServer()=0;
	virtual int32_t getState()=0;
	virtual int32_t publishVideo(YangStreamCapture* videoFrame)=0;
	virtual int32_t publishAudio(YangStreamCapture* audioFrame)=0;	
	void setReceiveCallback(YangReceiveCallback* cbk);
	static YangRtcHandle* createRtcHandle(YangContext* pcontext);

	YangReceiveCallback* m_recvcb;
protected:

	YangStreamConfig *m_conf;
};


#endif /* INCLUDE_YANGWEBRTC_YANGRTCHANDLE_H_ */
