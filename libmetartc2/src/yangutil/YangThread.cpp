

#include "yangutil/sys/YangThread.h"
#include "yangutil/sys/YangLog.h"

YangThread::YangThread(){
#ifndef _MSC_VER

    m_thread=0;
#endif
}
YangThread::~YangThread(){

}

int32_t YangThread::start()
{
    if (pthread_create( &m_thread, 0, &YangThread::go, this))
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
    pthread_join(m_thread, &ret);
    return ret;
}

pthread_t YangThread::getThread()
{
    return m_thread;
}

int32_t YangThread::detach()
{
    return pthread_detach(m_thread);
}

int32_t YangThread::equals(YangThread* t)
{
    return pthread_equal(m_thread, t->getThread());
}

void YangThread::exitThread(void* value_ptr)
{
    pthread_exit(value_ptr);
}

int32_t YangThread::cancel()
{
    return pthread_cancel(m_thread);
}

