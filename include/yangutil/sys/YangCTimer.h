//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRC_YANGUTIL_SRC_YANGCTIMER_H_
#define SRC_YANGUTIL_SRC_YANGCTIMER_H_
#include <yangutil/yangtype.h>
#include <pthread.h>
#include <stdint.h>
#ifdef _WIN32
#define Yang_Enable_Timer_Phtread 1
#else
#define Yang_Enable_Timer_Phtread 0
#endif

typedef struct YangCTimer{
	int32_t taskId;
	int32_t timerfd;
	int32_t efd;
	int32_t isStart;
	int32_t isloop;
	int32_t waitState;
	int32_t waitTime;
	pthread_t threadId;
#if Yang_Enable_Timer_Phtread
    pthread_mutex_t t_lock;
    pthread_cond_t t_cond_mess;
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
