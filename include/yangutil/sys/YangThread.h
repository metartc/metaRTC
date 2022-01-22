#ifndef YangThread_H__
#define YangThread_H__
#include <stdint.h>
#include <yangutil/yangtype.h>
#include <pthread.h>

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
	int32_t cancel();
	pthread_t getThread();
	virtual void stop()=0;
protected:
	virtual void run() = 0;

private:
	static void* go(void *obj);
	pthread_t m_thread;

};

#endif
