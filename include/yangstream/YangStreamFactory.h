
#ifndef YANGSTREAM_INCLUDE_YANGSTREAMFACTORY_H_
#define YANGSTREAM_INCLUDE_YANGSTREAMFACTORY_H_
#include <yangstream/YangStreamHandle.h>
//#include "YangPlay.h"
//#include "YangPush.h"
class YangStreamFactory {
public:
	YangStreamFactory();
	virtual ~YangStreamFactory();
	YangStreamHandle *createStreamHandle(int32_t transType,int32_t puid,YangContext* pcontext);
	//YangPlay* createPlayRtmp(int32_t puid);
	//YangPlay* createPlaySrt(int32_t puid);
	//YangPush* createPushRtmp(int32_t puid);
	//YangPush* createPushSrt(int32_t puid);
};

#endif /* YANGSTREAM_INCLUDE_YANGSTREAMFACTORY_H_ */
