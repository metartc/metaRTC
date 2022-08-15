//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGUTIL_SRC_YANGTIMER_H_
#define SRC_YANGUTIL_SRC_YANGTIMER_H_
#include <yangutil/sys/YangThread.h>
#include <stdint.h>


#define Yang_Using_Phtread 1


#if !Yang_Using_Phtread
#ifdef _WIN32
#include <windows.h>
#endif
#endif

class YangTimerTask {
public:
	YangTimerTask() {};
	virtual ~YangTimerTask() {};
	virtual void doTask(int32_t taskId)=0;
};


class YangTimer: public YangThread {
public:
	YangTimer();
	virtual ~YangTimer();
	int32_t m_isStart;
	int32_t m_loop;
	int32_t m_waitState;
	int32_t m_waitTime;

	void setTask(YangTimerTask *ptask);
	void setTimelen(int32_t ptimelen);//hao miao


	void setTaskId(int32_t ptaskId);
	void stop();

#if !Yang_Using_Phtread
#ifdef _WIN32
	static void  CALLBACK TimeEvent(PVOID lpParam, BOOLEAN TimerOrWaitFired);
#endif
#endif

protected:
	void run();
	void startLoop();
	void stopLoop();
private:
	YangTimerTask *m_task;
    int32_t m_taskId;



#if Yang_Using_Phtread
        pthread_mutex_t m_lock;
        pthread_cond_t m_cond_mess;
#else
#ifdef _WIN32
        void startWindowsEventTime(int pwaitTime,DWORD_PTR duser);
	HANDLE	m_hTimerQueue;
	HANDLE	m_hTimerQueueTimer;
	HANDLE m_winEvent;
#else
	int m_timerfd;
	int m_efd;
#endif
#endif

};

#endif /* SRC_YANGUTIL_SRC_YANGTIMER_H_ */
