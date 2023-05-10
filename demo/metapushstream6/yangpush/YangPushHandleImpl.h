//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGPUSH_YANGPUSHHANDLEIMPL_H_
#define YANGPUSH_YANGPUSHHANDLEIMPL_H_
#include <yangpush/YangPushPublish.h>
#include <yangpush/YangPushHandle.h>
#include <yangpush/YangRtcPublish.h>
#include <yangpush/YangSendVideoImpl.h>

#include <yangutil/sys/YangUrl.h>


class YangPushHandleImpl :public YangPushHandle{
public:
	YangPushHandleImpl(bool hasAudio,bool initVideo,int pvideotype,YangVideoInfo* screenvideo,YangVideoInfo* outvideo,YangContext* pcontext,YangSysMessageI* pmessage);
	virtual ~YangPushHandleImpl();
	void init();
	void startCapture();
    int32_t publish(char* url,yangbool isWihp);

	YangVideoBuffer* getPreVideoBuffer();

	void disconnect();
	void changeSrc(int videoSrcType,bool pisinit);

	void addVr();
	void delVr();

	YangSendVideoI* getSendVideo();
	YangSendVideoImpl* m_send;

private:
    void startCamera();

    void stopCamera();

	void stopPublish();

	void switchToCamera(bool pisinit);

	void switchToOutside(bool pisinit);

private:
	bool m_hasAudio;
	int m_videoState;
	bool m_isInit;

	YangPushPublish* m_cap;
	YangRtcPublish* m_rtcPub;

	YangContext* m_context;
	YangUrlData m_url;
	YangSysMessageI* m_message;
	YangVideoInfo* m_screenInfo;

	YangVideoInfo* m_outInfo;

};

#endif /* YANGPUSH_YANGPUSHHANDLEIMPL_H_ */
