#include "YangPushMessageHandle.h"
#include <yangutil/yang_unistd.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <yangpush/YangPushCommon.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

YangPushMessageHandle::YangPushMessageHandle(bool hasAudio,int pvideotype,YangVideoInfo* screenvideo,YangVideoInfo* outvideo,YangContext* pcontext,YangSysMessageI* pmessage,YangSysMessageHandleI* pmessageHandle) {

//	m_receive = prec;
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

void YangPushMessageHandle::handleMessage(YangSysMessage *mss) {

	//int32_t uid = mss->uid;
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
	case YangM_Push_Publish_Start:	
	{
        if(mss->user&&m_push) ret = m_push->publish((char*)mss->user,yang_getLocalInfo().c_str(),m_context->sys.rtcLocalPort);
		break;
	}
	case YangM_Push_Publish_Stop:
	{
            if(m_push)  m_push->disconnect();
			break;
	}
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

