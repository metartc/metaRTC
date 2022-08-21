/*
 * YangP2pFactoryI.h
 *
 *  Created on: 2022年7月14日
 *      Author: yang
 */

#ifndef INCLUDE_YANGP2P_YANGP2PFACTORYI_H_
#define INCLUDE_YANGP2P_YANGP2PFACTORYI_H_

#include <yangp2p/YangP2pPublish.h>
#include <yangp2p/YangP2pDecoder.h>

class YangP2pFactoryI{
public:
	YangP2pFactoryI(){};
	virtual ~YangP2pFactoryI(){};
	virtual YangP2pPublish* createPublish(YangContext *pcontext)=0;
	virtual YangP2pDecoder* createDecoder(YangContext *pcontext)=0;
	virtual void* getP2pCapture(int32_t pcapturetype,YangContext *pcontext)=0;
};


#endif /* INCLUDE_YANGP2P_YANGP2PFACTORYI_H_ */
