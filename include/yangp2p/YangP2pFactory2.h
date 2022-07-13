//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGP2P_YANGP2PFACTORY2_H_
#define INCLUDE_YANGP2P_YANGP2PFACTORY2_H_
#include <yangp2p/YangP2pPublish.h>
#include <yangp2p/YangP2pDecoder.h>
class YangP2pFactory2 {
public:
	YangP2pFactory2();
	virtual ~YangP2pFactory2();
	YangP2pPublish* createPublish(YangContext *pcontext);
	YangP2pDecoder* createDecoder(YangContext *pcontext);
	void* getP2pCapture(int32_t pcapturetype,YangContext *pcontext);
};

#endif /* INCLUDE_YANGP2P_YANGP2PFACTORY2_H_ */
