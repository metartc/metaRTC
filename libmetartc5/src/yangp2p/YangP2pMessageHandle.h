//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGMEETING_SRC_YangP2pMessageHandle_H_
#define SRC_YANGMEETING_SRC_YangP2pMessageHandle_H_
#include "YangP2pHandleImpl.h"
#include <yangutil/sys/YangThread.h>
#include <yangutil/sys/YangSysMessageHandle.h>
#include <vector>
#include <mutex>


using namespace std;

class YangP2pMessageHandle :public YangSysMessageHandle{
public:
    YangP2pMessageHandle(bool hasAudio,YangContext* pcontext,YangSysMessageI* pmessage,YangSysMessageHandleI* pmessageHandle,YangP2pFactoryI* factory);
	virtual ~YangP2pMessageHandle();

	void initAll();
	void deleteAll();
	void handleMessage(YangSysMessage* mss);
	YangP2pHandleImpl* m_p2p;
private:
	int32_t connectRtc(char* user);
	YangContext *m_context;
	YangSysMessageHandleI *m_receive;

};

#endif /* SRC_YANGMEETING_SRC_YangP2pMessageHandle_H_ */
