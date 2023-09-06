//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangCTimer.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangLog.h>

#if !Yang_Enable_Timer_Phtread
#if Yang_OS_WIN
void  CALLBACK g_yang_TimeEvent(PVOID user, BOOLEAN TimerOrWaitFired2)
{
    YangCTimer* timer=(YangCTimer*)user;
    if(timer->doTask) timer->doTask(timer->taskId,timer->user);
    return;
}
void g_yang_startWindowsEventTime2(int pwaitTime,YangCTimer *timer)
{
    if(timer==NULL) return;

    timer->hTimerQueue = CreateTimerQueue();
    if(timer->hTimerQueue!=NULL){
        if (!CreateTimerQueueTimer(&timer->hTimerQueueTimer, timer->hTimerQueue, g_yang_TimeEvent, timer, 0, pwaitTime, WT_EXECUTEDEFAULT))
        {
            timer->hTimerQueue = NULL;
            timer->hTimerQueueTimer = NULL;
        }
    }


    return;
}
#else
#include <sys/time.h>
    #if !Yang_OS_APPLE
		#include <sys/timerfd.h>
		#include <sys/epoll.h>
	#endif
#endif
#endif
#include <fcntl.h>

void yang_create_timer(YangCTimer *timer, void *user, int32_t taskId,
		int32_t waitTime) {
	if (timer == NULL)
		return;
	timer->isloop = yangfalse;
	timer->isStart = yangfalse;
	timer->waitState = 0;
	timer->waitTime = waitTime;
#if Yang_Enable_Timer_Phtread
	yang_thread_mutex_init(&timer->t_lock,NULL);
	yang_thread_cond_init(&timer->t_cond_mess,NULL);
#else
#if Yang_OS_WIN
    timer->hTimerQueue=NULL;
    timer->hTimerQueueTimer=NULL;
    timer->winEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
#elif Yang_OS_APPLE
	timer->_timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_global_queue(0, 0));
#else
    timer->timerfd = timerfd_create(CLOCK_REALTIME, 0);//TFD_NONBLOCK | TFD_CLOEXEC);
    timer->efd = -1;
#endif

#endif

	timer->user = user;
	timer->doTask = NULL;
	timer->taskId = taskId;
}
void yang_destroy_timer(YangCTimer *timer) {
	if (timer == NULL)		return;
#if Yang_Enable_Timer_Phtread
	yang_thread_mutex_destroy(&timer->t_lock);
	yang_thread_cond_destroy(&timer->t_cond_mess);
#endif
}
void* yang_run_timer_thread(void *obj) {
	YangCTimer *timer = (YangCTimer*) obj;
	timer->isStart = yangtrue;
	timer->isloop = yangtrue;
#if Yang_Enable_Timer_Phtread
    struct timespec outtime;
    struct timeval now;
    yang_thread_mutex_lock(&timer->t_lock);
    while (timer->isloop) {
        gettimeofday(&now, NULL);

        long nsec = now.tv_usec * 1000 + (timer->waitTime % 1000) * 1000000;
        outtime.tv_sec=now.tv_sec + nsec / 1000000000 + timer->waitTime / 1000;
        outtime.tv_nsec=nsec % 1000000000;

        timer->waitState=1;

        yang_thread_cond_timedwait(&timer->t_cond_mess, &timer->t_lock,&outtime);
        timer->waitState=0;
        if(timer->doTask) timer->doTask(timer->taskId, timer->user);
    }
    yang_thread_mutex_unlock(&timer->t_lock);
#else
    #if Yang_OS_WIN
    g_yang_startWindowsEventTime2(timer->waitTime,timer);
    if(WaitForSingleObject(timer->winEvent,INFINITE) !=WAIT_OBJECT_0)
    {
        yang_error("YangTimer WaitForSingleObject fail");
    }

    CloseHandle(timer->winEvent);
    timer->winEvent=NULL;
	#elif Yang_OS_APPLE

    #else
	struct itimerspec itimer;
	itimer.it_value.tv_sec = timer->waitTime / 1000;
	itimer.it_value.tv_nsec = (timer->waitTime % 1000) * 1000 * 1000;
	itimer.it_interval.tv_sec = timer->waitTime / 1000;
	itimer.it_interval.tv_nsec = (timer->waitTime % 1000) * 1000 * 1000;
	int ret = timerfd_settime(timer->timerfd, TFD_TIMER_ABSTIME, &itimer, NULL);
	if (ret == -1) {
		yang_error("timerfd_settime");
	}

	int opts;
	opts = fcntl(timer->timerfd, F_GETFL);
	if (opts < 0) {
		yang_error("fcntl(sock,GETFL)");
		_exit(1);
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(timer->timerfd, F_SETFL, opts) < 0) {
		yang_error("fcntl(sock,SETFL,opts)");
		_exit(1);
	}
	timer->efd = epoll_create1(0);
	struct epoll_event tev;
	tev.events = EPOLLIN | EPOLLET;
	tev.data.fd = timer->timerfd;
	epoll_ctl(timer->efd, EPOLL_CTL_ADD, timer->timerfd, &tev);
	struct epoll_event ev[1];
	while (timer->isloop) {
       // int nev = epoll_wait(timer->efd, ev, 1, 0);
        int nev = epoll_wait(timer->efd, ev, 1, Yang_CTimer_Epoll_Timeout);
		if (nev > 0 && (ev[0].events & EPOLLIN)) {
			uint64_t res;
			int bytes =	read(timer->timerfd, &res, sizeof(res));
			(void)bytes;
			if (timer->doTask)
				timer->doTask(timer->taskId, timer->user);
		}
	}
    #endif

#endif
	timer->isStart = yangfalse;
	return NULL;
}
void yang_timer_start(YangCTimer *timer) {
	if (timer == NULL||timer->isStart)
		return;

#if Yang_OS_APPLE
	dispatch_source_set_timer(timer->_timer, DISPATCH_TIME_NOW, timer->waitTime * NSEC_PER_MSEC, timer->waitTime * NSEC_PER_MSEC);
	dispatch_source_set_event_handler(timer->_timer, ^{
         if(timer->doTask) timer->doTask(timer->taskId,timer->user);
	});
    timer->isStart = yangtrue;
    timer->isloop = yangtrue;

	dispatch_resume(timer->_timer);
#else
    if (yang_thread_create(&timer->threadId, 0, yang_run_timer_thread, timer)) {
        yang_error("YangThread::start could not start thread");

    }
#endif
}
void yang_timer_stop(YangCTimer *timer) {
	if (timer == NULL||!timer->isStart)
		return;
	if (timer->isStart) {
		timer->isloop = yangfalse;

#if Yang_Enable_Timer_Phtread
    if(timer->waitState){
    	yang_thread_mutex_lock(&timer->t_lock);
    	yang_thread_cond_signal(&timer->t_cond_mess);
        yang_thread_mutex_unlock(&timer->t_lock);

    }
	while (timer->isStart)
					yang_usleep(1000);
#else

    #if Yang_OS_WIN
    if (timer->hTimerQueueTimer != NULL)
        DeleteTimerQueueTimer(timer->hTimerQueue, timer->hTimerQueueTimer, INVALID_HANDLE_VALUE);
    if (timer->hTimerQueue != NULL)
        DeleteTimerQueueEx(timer->hTimerQueue, INVALID_HANDLE_VALUE);

    timer->hTimerQueueTimer = NULL;
    timer->hTimerQueue = NULL;
    SetEvent(timer->winEvent);
     while (timer->isStart)			yang_usleep(1000);
	#elif Yang_OS_APPLE
		dispatch_source_cancel(timer->_timer);
        timer->isStart = yangfalse;

    #else
     while (timer->isStart)			yang_usleep(1000);
	epoll_ctl(timer->efd, EPOLL_CTL_DEL, timer->timerfd, NULL);
	close(timer->efd);
	close(timer->timerfd);
	timer->timerfd = -1;
    #endif

#endif

	}

}

