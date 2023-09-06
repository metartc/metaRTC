//
// Copyright (c) 2019-2022 yanggaofeng blueskiner
//
#ifndef SRC_YANGUTIL_SRC_YANGCTIMER_H_
#define SRC_YANGUTIL_SRC_YANGCTIMER_H_
#include <yangutil/yangtype.h>
#include <stdint.h>
#include <yangutil/sys/YangThread.h>

#if Yang_OS_APPLE
	#include <dispatch/dispatch.h>

#endif
typedef struct YangCTimer{
	int32_t taskId;

	int32_t isStart;
	int32_t isloop;
	int32_t waitState;
	int32_t waitTime;
	yang_thread_t threadId;
#if Yang_Enable_Timer_Phtread
	yang_thread_mutex_t t_lock;
	yang_thread_cond_t t_cond_mess;
#else
    #if Yang_OS_WIN   
        HANDLE	hTimerQueue;
        HANDLE	hTimerQueueTimer;
        HANDLE  winEvent;
	#elif Yang_OS_APPLE
		dispatch_source_t _timer;
	#else
        int32_t timerfd;
        int32_t efd;
    #endif
#endif
	void (*doTask)(int32_t taskId,void* user);
	void* user;
}YangCTimer;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_timer(YangCTimer* timer,void* user,int32_t taskId,int32_t waitTime);
void yang_destroy_timer(YangCTimer* timer);
void yang_timer_start(YangCTimer* timer);
void yang_timer_stop(YangCTimer* timer);
#ifdef __cplusplus
}
#endif


#endif /* SRC_YANGUTIL_SRC_YANGTIMER_H_ */
