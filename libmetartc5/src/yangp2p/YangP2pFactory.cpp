//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangp2p/YangP2pFactory.h>
#include "YangP2pMessageHandle.h"


YangP2pFactory::YangP2pFactory() {


}

YangP2pFactory::~YangP2pFactory() {

}

YangP2pHandle* YangP2pFactory::createP2pHandle(bool hasAudio, YangContext *pcontext,YangSysMessageI *pmessage,YangP2pFactoryI* factory) {
    return new YangP2pHandleImpl(hasAudio,pcontext, pmessage,factory);
}

YangSysMessageHandle* YangP2pFactory::createP2pMessageHandle(bool hasAudio, YangContext *pcontext,
        YangSysMessageI *pmessage,YangSysMessageHandleI* pmessagehandle,YangP2pFactoryI* factory) {
    return new YangP2pMessageHandle(hasAudio,pcontext, pmessage,pmessagehandle,factory);
}

YangVideoBuffer* YangP2pFactory::getPreVideoBuffer(YangSysMessageHandle* pmessageHandle){
	if(!pmessageHandle) return NULL;
	YangP2pMessageHandle* mess=dynamic_cast<YangP2pMessageHandle*>(pmessageHandle);
	if(mess&&mess->m_p2p) return mess->m_p2p->getPreVideoBuffer();
	return NULL;

}

vector<YangVideoBuffer*>* YangP2pFactory::getPlayVideoBuffer(YangSysMessageHandle* pmessageHandle){
	if(!pmessageHandle) return NULL;
	YangP2pMessageHandle* mess=dynamic_cast<YangP2pMessageHandle*>(pmessageHandle);
	if(mess&&mess->m_p2p) return mess->m_p2p->getPlayVideoBuffer();
	return NULL;
}
