//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangp2p/YangP2pFactory2.h>
#include <yangp2p/YangP2pPublishImpl.h>
#include <yangp2p/YangP2pDecoderImpl.h>
#include <yangp2p/YangP2pCaputreCamera.h>
YangP2pFactory2::YangP2pFactory2() {
	// TODO Auto-generated constructor stub

}

YangP2pFactory2::~YangP2pFactory2() {
	// TODO Auto-generated destructor stub
}

YangP2pPublish* YangP2pFactory2::createPublish(YangContext *pcontext){
	return new YangP2pPublishImpl(pcontext);
}

YangP2pDecoder* YangP2pFactory2::createDecoder(YangContext *pcontext){
	return new YangP2pDecoderImpl(pcontext);
}

void* YangP2pFactory2::getP2pCapture(int32_t pcapturetype,YangContext *pcontext){

    return new YangP2pCaputreCamera(pcontext);
}
