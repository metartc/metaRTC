//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_
#define INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_
#include <stdio.h>
#include <stdint.h>

class YangSysMessageI {
public:
	YangSysMessageI(){};
	virtual ~YangSysMessageI(){};
	virtual void success()=0;
	virtual void failure(int32_t errcode)=0;
};
struct YangSysMessage {
	int32_t uid;
	int32_t messageId;
	int32_t handleState;
	YangSysMessageI *handle;
        void* user;
};
class YangSysMessageHandleI {
public:
	YangSysMessageHandleI() {};
	virtual ~YangSysMessageHandleI() {};
	virtual void receiveSysMessage(YangSysMessage *psm, int32_t phandleRet)=0;
};
void yang_post_message(int32_t st, int32_t uid,YangSysMessageI *mhandle,void* user=NULL);

void yang_post_state_message(int32_t st, int32_t uid, int32_t handleState,YangSysMessageI *mhandle);
#endif /* INCLUDE_YANGUTIL_SYS_YANGMESSAGEI_H_ */
