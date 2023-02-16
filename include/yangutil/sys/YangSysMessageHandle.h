
//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGSYSMESSAGEHANDLE_H_
#define INCLUDE_YANGUTIL_SYS_YANGSYSMESSAGEHANDLE_H_
#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/sys/YangThread2.h>
#include <vector>


using namespace std;

class YangSysMessageHandle :public YangThread{
public:
	YangSysMessageHandle();
	virtual ~YangSysMessageHandle();
	virtual void handleMessage(YangSysMessage* mss)=0;
	virtual void initAll()=0;
	virtual void deleteAll()=0;
	void putMessage(YangSysMessageI *handle,int32_t pst, int32_t puid, int32_t handleState,void* user=NULL);

	static YangSysMessageHandle* m_instance;
	yangbool m_isStart;
	yangbool m_loop;
	void stop();
protected:
	void run();
	void startLoop();
	void stopLoop();

private:
	vector<YangSysMessage*> m_sysMessages;
	yang_thread_mutex_t m_mutex;
	yang_thread_mutex_t m_lock;
	yang_thread_cond_t m_cond_mess;

	yangbool m_waitState;
	YangSysMessageHandleI* m_receive;

};

#endif /* INCLUDE_YANGUTIL_SYS_YANGSYSMESSAGEHANDLE_H_ */
