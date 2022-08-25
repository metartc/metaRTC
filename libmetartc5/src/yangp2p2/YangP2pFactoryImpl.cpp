//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangp2p/YangP2pFactoryImpl.h>
#include <yangp2p2/YangP2pPublishImpl.h>
#include <yangp2p2/YangP2pDecoderImpl.h>
#include <yangp2p2/YangP2pCaputreCamera.h>
YangP2pFactoryImpl::YangP2pFactoryImpl() {
	// TODO Auto-generated constructor stub

}

YangP2pFactoryImpl::~YangP2pFactoryImpl() {
	// TODO Auto-generated destructor stub
}

YangP2pPublish* YangP2pFactoryImpl::createPublish(YangContext *pcontext){
	return new YangP2pPublishImpl(pcontext);
}

YangP2pDecoder* YangP2pFactoryImpl::createDecoder(YangContext *pcontext){
	return new YangP2pDecoderImpl(pcontext);
}

void* YangP2pFactoryImpl::getP2pCapture(int32_t pcapturetype,YangContext *pcontext){

    return new YangP2pCaputreCamera(pcontext);
}
