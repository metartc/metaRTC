#ifndef SRC_YANGUTIL_SRC_YANGCTIMER_H_
#define SRC_YANGUTIL_SRC_YANGCTIMER_H_
#include <pthread.h>
#include <stdint.h>


typedef struct YangCTimer{
	int32_t taskId;
	int32_t timerfd;
	int32_t efd;
	int32_t isStart;
	int32_t isloop;
	int32_t waitState;
	int32_t waitTime;
	pthread_t threadId;
	void (*doTask)(int32_t taskId,void* user);
	void* user;
}YangCTimer;
#ifdef __cplusplus
extern "C"{
#endif
void yang_init_timer(YangCTimer* timer,void* user,int32_t taskId,int32_t waitTime);
void yang_destroy_timer(YangCTimer* timer);
void yang_timer_start(YangCTimer* timer);
void yang_timer_stop(YangCTimer* timer);
#ifdef __cplusplus
}
#endif


#endif /* SRC_YANGUTIL_SRC_YANGTIMER_H_ */
