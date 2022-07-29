//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangipc/YangIpc.h>
#include <yangutil/sys/YangCTimer.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>



#include <pthread.h>

YangCTimer* m_5stimer=NULL;
int g_waitState=0;
pthread_mutex_t g_lock ;
pthread_cond_t	g_cond ;
/*
 * ctrl + c controller
 */
static int32_t b_exit = 0;
static void ctrl_c_handler(int s){
    printf("\ncaught signal %d, exit.\n",s);
    b_exit = 1;
    pthread_mutex_lock(&g_lock);
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_lock);
}


static int32_t b_reload = 0;
static void reload_handler(int s){
    printf("\ncaught signal %d, reload.\n",s);
    b_reload = 1;
}

void g_yang_main_doTask(int32_t taskId, void *user) {
	if (user == NULL)	return;

	YangIpc *ipc = (YangIpc*) user;
	if (taskId == 5) {
		ipc->checkAlive(&ipc->session);
	}
}
void yang_initCtimer(YangIpc* ipc){
	m_5stimer = (YangCTimer*) calloc(1, sizeof(YangCTimer));
	yang_create_timer(m_5stimer, ipc, 5, 5000);
	m_5stimer->doTask = g_yang_main_doTask;

	yang_timer_start(m_5stimer);
}
void yang_stopCtimer(){
	yang_timer_stop(m_5stimer);
	yang_destroy_timer(m_5stimer);
	yang_free(m_5stimer);
}
int main(int argc, char* argv[])
{

    struct sigaction    sigIntHandler;
    struct sigaction    sigHupHandler;
    pthread_mutex_init(&g_lock,NULL);
    pthread_cond_init(&g_cond,NULL);


    //ctrl + c to exit
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, 0);

    //hup to reload
    sigHupHandler.sa_handler = reload_handler;
    sigemptyset(&sigHupHandler.sa_mask);
    sigHupHandler.sa_flags = 0;
    sigaction(SIGHUP, &sigHupHandler, 0);
    YangIpc ipc;
    memset(&ipc,0,sizeof(YangIpc));
    yang_create_ipc(&ipc);
    ipc.init(&ipc.session);
    ipc.start(&ipc.session);

    yang_initCtimer(&ipc);

    while(!b_exit)
    {
    	g_waitState=1;
    	pthread_cond_wait(&g_cond, &g_lock);
    	g_waitState=0;
    	if (b_reload) {
    		//reload
    		b_reload = 0;

    	}
    }
   yang_stopCtimer();
   yang_destroy_ipc(&ipc);
//EXIT_PROC:
	//yang_destroy_ipc(&ipc);



    return 0;
}
