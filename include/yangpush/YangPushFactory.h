#ifndef INCLUDE_YANGPUSH_YANGPUSHFACTORY_H_
#define INCLUDE_YANGPUSH_YANGPUSHFACTORY_H_
#include <yangpush/YangPushHandle.h>
#include <yangutil/sys/YangSysMessageHandle.h>
#include <yangpush/YangSendVideoI.h>
class YangPushFactory {
public:
	YangPushFactory();
	virtual ~YangPushFactory();
	YangPushHandle* createPushHandle(bool hasAudio,bool initVideo,int pvideotype,YangVideoInfo *screenvideo, YangVideoInfo *outvideo,YangContext* pcontext,YangSysMessageI* pmessage);
        YangSysMessageHandle* createPushMessageHandle(bool hasAudio,bool initVideo,int pvideotype,YangVideoInfo *screenvideo, YangVideoInfo *outvideo,YangContext* pcontext,YangSysMessageI* pmessage,YangSysMessageHandleI* pmessagehandle);
	YangVideoBuffer* getPreVideoBuffer(YangSysMessageHandle* pmessageHandle);
	YangSendVideoI* getSendVideo(YangSysMessageHandle* pmessageHandle);
};

#endif /* INCLUDE_YANGPUSH_YANGPUSHFACTORY_H_ */
