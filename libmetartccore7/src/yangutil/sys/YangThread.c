//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangutil/sys/YangThread.h>
#if !Yang_Enable_Phtread
#if Yang_OS_WIN
#include <process.h>


typedef struct{
    void* (*startfn)(void*);
    void* user;
}YangThreadPara;



DWORD WINAPI yang_thread_beginFn2( LPVOID lpParam ){
    YangThreadPara* pThis = (YangThreadPara*)lpParam;
    if(pThis){
        pThis->startfn(pThis->user);
    }
    yang_free(pThis);
    return 0;
}

 int yang_thread_create(yang_thread_t* const thread, const void* attr,
                          void* (*startfn)(void*), void* arg) {
  (void)attr;
   YangThreadPara* para=(YangThreadPara*)yang_calloc(sizeof (YangThreadPara),1);
   para->user=arg;
   para->startfn=startfn;

   *thread = (yang_thread_t)CreateThread(
               NULL,                   // default security attributes
               0,                      // use default stack size
               yang_thread_beginFn2,       // thread function name
               para,          // argument to thread function
               0,                      // use default creation flags
               NULL);   // returns the thread identif
  if (*thread == NULL) return 1;
 // SetThreadPriority(*thread, THREAD_PRIORITY_ABOVE_NORMAL);
  return 0;
}

 int yang_thread_join(yang_thread_t thread, void** value_ptr) {
  (void)value_ptr;
  return (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0 ||
          CloseHandle(thread) == 0);
}

// Mutex
 int yang_thread_mutex_init(yang_thread_mutex_t* const mutex, void* mutexattr) {
  (void)mutexattr;
  InitializeCriticalSection(mutex);
  return 0;
}

 int yang_thread_mutex_lock(yang_thread_mutex_t* const mutex) {
  EnterCriticalSection(mutex);
  return 0;
}

 int yang_thread_mutex_unlock(yang_thread_mutex_t* const mutex) {
  LeaveCriticalSection(mutex);
  return 0;
}

 int yang_thread_mutex_destroy(yang_thread_mutex_t* const mutex) {
  DeleteCriticalSection(mutex);
  return 0;
}

// Condition
 int yang_thread_cond_destroy(yang_thread_cond_t* const condition) {
  int err = 1;
  err &= (CloseHandle(condition->waiting_sem) != 0);
  err &= (CloseHandle(condition->received_sem) != 0);
  err &= (CloseHandle(condition->signal_event) != 0);
  return !err;
}

 int yang_thread_cond_init(yang_thread_cond_t* const condition, void* cond_attr) {
  (void)cond_attr;
  condition->waiting_sem = CreateSemaphore(NULL, 0, 1, NULL);
  condition->received_sem = CreateSemaphore(NULL, 0, 1, NULL);
  condition->signal_event = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (condition->waiting_sem == NULL ||
      condition->received_sem == NULL ||
      condition->signal_event == NULL) {
    yang_thread_cond_destroy(condition);
    return 1;
  }
  return 0;
}

 int yang_thread_cond_signal(yang_thread_cond_t* const condition) {
  int err = 1;
  if (WaitForSingleObject(condition->waiting_sem, 0) == WAIT_OBJECT_0) {
    // a thread is waiting in yang_thread_cond_wait: allow it to be notified
    err = SetEvent(condition->signal_event);
    // wait until the event is consumed so the signaler cannot consume
    // the event via its own yang_thread_cond_wait.
    err &= (WaitForSingleObject(condition->received_sem, INFINITE) !=
           WAIT_OBJECT_0);
  }
  return !err;
}

 int yang_thread_cond_wait(yang_thread_cond_t* const condition,
                             yang_thread_mutex_t* const mutex) {
  int err=1;
            // note that there is a consumer available so the signal isn't dropped in
  // yang_thread_cond_signal
  if (!ReleaseSemaphore(condition->waiting_sem, 1, NULL))
    return 1;
  // now unlock the mutex so yang_thread_cond_signal may be issued
  yang_thread_mutex_unlock(mutex);
  err = (WaitForSingleObject(condition->signal_event, INFINITE) ==
        WAIT_OBJECT_0);
  err &= ReleaseSemaphore(condition->received_sem, 1, NULL);
  yang_thread_mutex_lock(mutex);
  return !err;
}
#endif
#endif


