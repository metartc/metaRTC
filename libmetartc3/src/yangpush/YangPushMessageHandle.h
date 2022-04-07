//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef SRC_YANGMEETING_SRC_YangPushMessageHandle_H_
#define SRC_YANGMEETING_SRC_YangPushMessageHandle_H_
#include "YangPushHandleImpl.h"

#include <yangutil/sys/YangSysMessageHandle.h>
#include <vector>
#include <mutex>
#include "yangutil/sys/YangThread.h"

using namespace std;

class YangPushMessageHandle :public YangSysMessageHandle{
public:
    YangPushMessageHandle(bool hasAudio,int pvideotype,YangVideoInfo* screenvideo,YangVideoInfo* outvideo,YangContext* pcontext,YangSysMessageI* pmessage,YangSysMessageHandleI* pmessageHandle);
	virtual ~YangPushMessageHandle();

	void initAll();
	void deleteAll();
	void handleMessage(YangSysMessage* mss);
	YangPushHandleImpl* m_push;
private:
	int32_t pushPublish(char* user);
	YangContext *m_context;
	YangSysMessageHandleI *m_receive;

};

#endif /* SRC_YANGMEETING_SRC_YangPushMessageHandle_H_ */
