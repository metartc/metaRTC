//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangP2pMessageHandle.h"
#include <yangutil/yang_unistd.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>

#include <yangp2p/YangP2pCommon.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

YangP2pMessageHandle::YangP2pMessageHandle(bool hasAudio,YangContext* pcontext,YangSysMessageI* pmessage,YangSysMessageHandleI* pmessageHandle,YangP2pFactoryI* factory) {
	m_context=pcontext;
	m_receive= pmessageHandle;
	m_p2p = new YangP2pHandleImpl(hasAudio,pcontext,pmessage,factory);
}

YangP2pMessageHandle::~YangP2pMessageHandle() {
	deleteAll();
}
void YangP2pMessageHandle::initAll(){

}
void YangP2pMessageHandle::deleteAll(){
	m_context=NULL;
	yang_delete(m_p2p);
}
int32_t YangP2pMessageHandle::connectRtc(char* user){
	if(m_p2p==NULL) return ERROR_RTC_CONNECT;

	return m_p2p->connectRtc(user);
}
void YangP2pMessageHandle::handleMessage(YangSysMessage *mss) {
	int32_t ret = Yang_Ok;
	switch (mss->messageId) {
	case YangM_P2p_StartVideoCapture:
	{
		if(m_p2p) m_p2p->init();
		break;
	}
	case YangM_P2p_Connect:
	{
		if(mss->user&&m_p2p) ret = connectRtc((char*)mss->user);
		break;
	}
	case YangM_P2p_Disconnect:
	{
		if(m_p2p)  m_p2p->disconnect();
		break;
	}


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

