//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGP2P_YANGP2PFACTORYIMPL_H_
#define INCLUDE_YANGP2P_YANGP2PFACTORYIMPL_H_
#include <yangp2p/YangP2pFactoryI.h>
class YangP2pFactoryImpl :public YangP2pFactoryI{
public:
	YangP2pFactoryImpl();
	virtual ~YangP2pFactoryImpl();
	YangP2pPublish* createPublish(YangContext *pcontext);
	YangP2pDecoder* createDecoder(YangContext *pcontext);
	void* getP2pCapture(int32_t pcapturetype,YangContext *pcontext);
};

#endif /* INCLUDE_YANGP2P_YANGP2PFACTORYIMPL_H_ */
