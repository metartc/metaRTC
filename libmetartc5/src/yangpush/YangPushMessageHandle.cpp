//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangPushMessageHandle.h"
#include <yangutil/yang_unistd.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>

#include <yangpush/YangPushCommon.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

YangPushMessageHandle::YangPushMessageHandle(bool hasAudio,int pvideotype,YangVideoInfo* screenvideo,YangVideoInfo* outvideo,YangContext* pcontext,YangSysMessageI* pmessage,YangSysMessageHandleI* pmessageHandle) {

	m_context=pcontext;
	m_receive= pmessageHandle;
    m_push = new YangPushHandleImpl(hasAudio,false,pvideotype,screenvideo,outvideo,pcontext,pmessage);

}

YangPushMessageHandle::~YangPushMessageHandle() {
	deleteAll();
}
void YangPushMessageHandle::initAll(){

}
void YangPushMessageHandle::deleteAll(){
		m_context=NULL;
		yang_delete(m_push);
}
int32_t YangPushMessageHandle::pushPublish(char* user){
	if(m_push==NULL) return 1;

	return m_push->publish(user);
}
void YangPushMessageHandle::handleMessage(YangSysMessage *mss) {


	int32_t ret = Yang_Ok;

	switch (mss->messageId) {
	case YangM_Push_StartVideoCapture:
    {
        if(m_push) m_push->changeSrc(Yang_VideoSrc_Camera,false);
		break;
	}
	case YangM_Push_StartScreenCapture:
    {
        if(m_push) m_push->changeSrc(Yang_VideoSrc_Screen,false);
		break;
	}
    case YangM_Push_StartOutCapture:
    {
        if(m_push) m_push->changeSrc(Yang_VideoSrc_OutInterface,false);
        break;
    }
	case YangM_Push_Connect:	
	{
        if(mss->user&&m_push) ret = pushPublish((char*)mss->user);
		break;
	}
	case YangM_Push_Disconnect:
	{
            if(m_push)  m_push->disconnect();
			break;
	}
#ifndef __ANDROID__
	case YangM_Push_Record_Start:
	{
        if(mss->user&&m_push)  m_push->recordFile((char*)mss->user);
		break;
	}
	case YangM_Push_Record_Stop:
	{
            if(m_push)  m_push->stopRecord();
			break;
	}
#endif
	case YangM_Push_SwitchToCamera:
	{
             if(m_push)
            	 m_push->changeSrc(Yang_VideoSrc_Camera,false);
			break;
	}
	case YangM_Push_SwitchToScreen:
	{
           if(m_push)
	        	m_push->changeSrc(Yang_VideoSrc_Screen,false);
			break;
	}
	case YangM_Sys_Setvr:
		ret =Yang_Ok;
#if Yang_HaveVr
		if(m_push) m_push->addVr();
#endif
		break;
	case YangM_Sys_UnSetvr:
		ret = Yang_Ok;
#if Yang_HaveVr
		if(m_push) m_push->delVr();
#endif
		break;
	}

	if (mss->handle) {
		if (ret)
			mss->handle->failure(ret);
		else
			mss->handle->success();
	}

	if (m_receive)
			m_receive->receiveSysMessage(mss,ret);
}

