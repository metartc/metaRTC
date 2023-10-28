//
// Copyright (c) 2019-2022 yanggaofeng
//

#include "YangThread2.h"
#include <stdio.h>
#define yang_error printf

YangThread::YangThread(){
#ifndef _MSC_VER

    m_thread=0;
#endif
}
YangThread::~YangThread(){
#ifdef _MSC_VER
   #if !Yang_Enable_Phtread
  //  if(m_thread) CloseHandle(m_thread);
    //m_thread=0;
    #endif
#endif
}

int32_t YangThread::start()
{
    if (yang_thread_create(&m_thread, 0, &YangThread::go, this))
    {
        yang_error("YangThread::start could not start thread");
        return -1;
    }

    return 0;
}

void* YangThread::go(void* obj)
{
    reinterpret_cast<YangThread*>(obj)->run();
    return NULL;
}

void* YangThread::join()
{
    void* ret;
    yang_thread_join(m_thread, &ret);
    return ret;
}

yang_thread_t YangThread::getThread()
{
    return m_thread;
}
/**
int32_t YangThread::detach()
{
    return yang_thread_detach(m_thread);
}

int32_t YangThread::equals(YangThread* t)
{
    return yang_thread_equal(m_thread, t->getThread());
}

void YangThread::exitThread(void* value_ptr)
{
	yang_thread_exit(value_ptr);
}

int32_t YangThread::cancel()
{
    return pthread_cancel(m_thread);
}
**/
