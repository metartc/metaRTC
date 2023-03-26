//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangpush/YangPushFactory.h>
#include "YangPushMessageHandle.h"
YangPushFactory::YangPushFactory() {


}

YangPushFactory::~YangPushFactory() {

}

YangPushHandle* YangPushFactory::createPushHandle(bool hasAudio,bool initVideo,int pvideotype, YangVideoInfo *screenvideo, YangVideoInfo *outvideo, YangContext *pcontext,
		YangSysMessageI *pmessage) {
	return new YangPushHandleImpl(hasAudio,initVideo,pvideotype,screenvideo,outvideo,pcontext, pmessage);
}

YangSysMessageHandle* YangPushFactory::createPushMessageHandle(bool hasAudio,bool initVideo,
		int pvideotype, YangVideoInfo *screenvideo, YangVideoInfo *outvideo, YangContext *pcontext,
        YangSysMessageI *pmessage,YangSysMessageHandleI* pmessagehandle) {
    return new YangPushMessageHandle(hasAudio,pvideotype,screenvideo,outvideo,pcontext, pmessage,pmessagehandle);
}

YangVideoBuffer* YangPushFactory::getPreVideoBuffer(YangSysMessageHandle* pmessageHandle){
	if(!pmessageHandle) return NULL;
	YangPushMessageHandle* mess=dynamic_cast<YangPushMessageHandle*>(pmessageHandle);
	if(mess&&mess->m_push) return mess->m_push->getPreVideoBuffer();
	return NULL;

}

YangSendVideoI* YangPushFactory::getSendVideo(YangSysMessageHandle* pmessageHandle){
	if(!pmessageHandle) return NULL;
		YangPushMessageHandle* mess=dynamic_cast<YangPushMessageHandle*>(pmessageHandle);
		if(mess&&mess->m_push) return mess->m_push->getSendVideo();
		return NULL;
}
