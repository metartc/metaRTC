/*
 * YangMeetingHandle.cpp
 *
 *  Created on: 2019年10月13日
 *      Author: yang
 */

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
	YangContext *m_context;
	YangSysMessageHandleI *m_receive;

};

#endif /* SRC_YANGMEETING_SRC_YangPushMessageHandle_H_ */
