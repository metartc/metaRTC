#ifndef YANGPUSH_YANGPUSHHANDLEIMPL_H_
#define YANGPUSH_YANGPUSHHANDLEIMPL_H_
#include <yangpush/YangPushPublish.h>
#include <yangpush/YangPushHandle.h>
#include <yangpush/YangRtcPublish.h>
#include <yangutil/sys/YangUrl.h>
#include <yangrecord/YangRecordHandle.h>
#include <yangpush/YangSendVideoImpl.h>
class YangPushHandleImpl :public YangPushHandle{
public:
	YangPushHandleImpl(bool hasAudio,bool initVideo,int pvideotype,YangVideoInfo* screenvideo,YangVideoInfo* outvideo,YangContext* pcontext,YangSysMessageI* pmessage);
	virtual ~YangPushHandleImpl();
	void init();
	void startCapture();
    int publish(string url,string localIp,int32_t localport);
	void setScreenVideoInfo(int videoSrcType,YangVideoInfo* pvideo);
	void setScreenInterval(int32_t pinterval);
	YangVideoBuffer* getPreVideoBuffer();
	void recordFile(char* filename);
	void stopRecord();
	void disconnect();
	void changeSrc(int videoSrcType,bool pisinit);
	void setDrawmouse(bool isDraw);
	void addVr();
	void delVr();

	YangSendVideoI* getSendVideo();
	YangSendVideoImpl* m_send;

private:
    void startCamera();
    void startScreen();
    void stopCamera();
    void stopScreen();
	void stopPublish();

	void switchToCamera(bool pisinit);
	void switchToScreen(bool pisinit);
	void switchToOutside(bool pisinit);

private:
	bool m_hasAudio;
	int m_videoState;
	bool m_isInit;

	YangPushPublish* m_cap;
	YangRtcPublish* m_pub;
	YangRecordHandle *m_rec;
	YangContext* m_context;
	YangUrlData m_url;
	YangSysMessageI* m_message;
	YangVideoInfo* m_screenInfo;

	YangVideoInfo* m_outInfo;

};

#endif /* YANGPUSH_YANGPUSHHANDLEIMPL_H_ */
