//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangTimer.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/yangtype.h>
#ifdef _WIN32

#else
#include <sys/time.h>
#endif
#include <time.h>
#include <stdio.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>

#if !Yang_Using_Phtread
#ifdef _WIN32
#pragma comment(lib,"Winmm.lib")
void  CALLBACK YangTimer::TimeEvent(PVOID user, BOOLEAN TimerOrWaitFired2)
{
    YangTimer* timer=(YangTimer*)user;
    if(timer->m_task) timer->m_task->doTask(timer->m_taskId);
    return;
}

void YangTimer::startWindowsEventTime(int pwaitTime,DWORD_PTR duser)
{
    m_hTimerQueue = CreateTimerQueue();
    if(m_hTimerQueue!=NULL){
        if (!CreateTimerQueueTimer(&m_hTimerQueueTimer, m_hTimerQueue, TimeEvent, this, 0, pwaitTime, WT_EXECUTEDEFAULT))
        {
            m_hTimerQueue = NULL;
            m_hTimerQueueTimer = NULL;
        }
    }


    return;
}
#else
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <fcntl.h>

#endif

#endif

YangTimer::YangTimer() {
    m_loop = 0;
    m_isStart = 0;
    m_waitState = 0;
    m_waitTime = 100;
#if Yang_Using_Phtread
    pthread_mutex_init(&m_lock,NULL);
    pthread_cond_init(&m_cond_mess,NULL);
    //m_lock = PTHREAD_MUTEX_INITIALIZER;
    //m_cond_mess = PTHREAD_COND_INITIALIZER;
#else
#ifdef _WIN32
    m_hTimerQueue=NULL;
    m_hTimerQueueTimer=NULL;
    m_winEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
#else
    m_timerfd = timerfd_create(CLOCK_REALTIME, 0);
    m_efd = -1;
#endif

#endif
    m_task = NULL;
    m_taskId = 0;
}

YangTimer::~YangTimer() {

    if (m_isStart) {
        stop();
        while (m_isStart)
            yang_usleep(1000);
    }
    m_task = NULL;
#if Yang_Using_Phtread
    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_cond_mess);
#endif
}
void YangTimer::setTaskId(int32_t ptaskId) {
    m_taskId = ptaskId;
}
void YangTimer::setTask(YangTimerTask *ptask) {
    m_task = ptask;
}
void YangTimer::setTimelen(int32_t ptimelen) {
    m_waitTime = ptimelen;
}
void YangTimer::run() {
    m_isStart = 1;
    startLoop();
    m_isStart = 0;
}
void YangTimer::stop() {
    stopLoop();

}
void YangTimer::stopLoop() {
    m_loop = 0;
#if Yang_Using_Phtread
    if(m_waitState){
        pthread_mutex_lock(&m_lock);
        pthread_cond_signal(&m_cond_mess);
        pthread_mutex_unlock(&m_lock);

    }
#else
#ifdef _WIN32
    if (m_hTimerQueueTimer != NULL)
        DeleteTimerQueueTimer(m_hTimerQueue, m_hTimerQueueTimer, INVALID_HANDLE_VALUE);
    if (m_hTimerQueue != NULL)
        DeleteTimerQueueEx(m_hTimerQueue, INVALID_HANDLE_VALUE);

    m_hTimerQueueTimer = NULL;
    m_hTimerQueue = NULL;
    SetEvent(m_winEvent);
#else
    if (m_isStart) {
        struct epoll_event tev;
        tev.events = EPOLLIN | EPOLLET;
        tev.data.fd = m_timerfd;
        epoll_ctl(m_efd, EPOLL_CTL_DEL, m_timerfd, &tev);
        close(m_efd);
        m_efd = -1;

    }
    close(m_timerfd);
    m_timerfd = -1;

#endif
#endif
}

void YangTimer::startLoop() {
    m_loop = 1;
    //pthread_mutex_unlock(&m_lock);
#if Yang_Using_Phtread
    struct timespec outtime;
    struct timeval now;
    pthread_mutex_lock(&m_lock);
    while (m_loop) {
        gettimeofday(&now, NULL);

        long nsec = now.tv_usec * 1000 + (m_waitTime % 1000) * 1000000;
        outtime.tv_sec=now.tv_sec + nsec / 1000000000 + m_waitTime / 1000;
        outtime.tv_nsec=nsec % 1000000000;

        m_waitState=1;

        pthread_cond_timedwait(&m_cond_mess, &m_lock,&outtime);
        m_waitState=0;
        if(m_task) m_task->doTask(m_taskId);
    }
    pthread_mutex_unlock(&m_lock);
#else
#ifdef _WIN32

    startWindowsEventTime(m_waitTime,(DWORD_PTR)this);
    if(WaitForSingleObject(m_winEvent,INFINITE) !=WAIT_OBJECT_0)
    {
        yang_error("YangTimer WaitForSingleObject fail");
    }

    CloseHandle(m_winEvent);
    m_winEvent=NULL;
#else
    struct itimerspec itimer;
    itimer.it_value.tv_sec = m_waitTime / 1000;
    itimer.it_value.tv_nsec = (m_waitTime % 1000) * 1000 * 1000;
    itimer.it_interval.tv_sec = m_waitTime / 1000;
    itimer.it_interval.tv_nsec = (m_waitTime % 1000) * 1000 * 1000;
    int ret = timerfd_settime(m_timerfd, TFD_TIMER_ABSTIME, &itimer, NULL);
    if (ret == -1) {
        yang_error("timerfd_settime");
    }

    int opts;
    opts = fcntl(m_timerfd, F_GETFL);
    if (opts < 0) {
        yang_error("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(m_timerfd, F_SETFL, opts) < 0) {
        yang_error("fcntl(sock,SETFL,opts)");
        exit(1);
    }
    m_efd = epoll_create(1);
    struct epoll_event tev;
    tev.events = EPOLLIN | EPOLLET;
    tev.data.fd = m_timerfd;
    epoll_ctl(m_efd, EPOLL_CTL_ADD, m_timerfd, &tev);
    struct epoll_event ev[1];
    while (m_loop) {
        int nev = epoll_wait(m_efd, ev, 1, 0);

        if (nev > 0 && (ev[0].events & EPOLLIN)) {
            uint64_t res;
            int bytes = read(m_timerfd, &res, sizeof(res));

            if (m_task)
                m_task->doTask(m_taskId);
        }
    }

#endif
#endif

}



