//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangThread2_H__
#define YangThread2_H__

#include "YangThread.h"

#define yang_stop_thread(x) if(x){while(x->m_isStart) yang_usleep(1000);}
#define yang_stop(x) if(x&&x->m_isStart){x->stop();}
class YangThread {
public:
	YangThread();
	virtual ~YangThread();

	int32_t start();
	void* join();
	int32_t detach();
	int32_t equals(YangThread *t);
	void exitThread(void *value_ptr);
	//int32_t cancel();
	yang_thread_t getThread();
	virtual void stop()=0;
protected:
	virtual void run() = 0;

private:
	static void* go(void *obj);
	yang_thread_t m_thread;

};

#endif
