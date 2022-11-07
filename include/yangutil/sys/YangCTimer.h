//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGUTIL_SRC_YANGCTIMER_H_
#define SRC_YANGUTIL_SRC_YANGCTIMER_H_
#include <yangutil/yangtype.h>
#include <stdint.h>
#include <yangutil/sys/YangThread.h>
#ifdef _WIN32
#define Yang_Enable_Timer_Phtread 0
#else
#define Yang_Enable_Timer_Phtread 0
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
    #ifdef _WIN32   
        HANDLE	hTimerQueue;
        HANDLE	hTimerQueueTimer;
        HANDLE  winEvent;
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
