//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangIpc.h"
#include <yangp2p/YangP2pFactory.h>
#include <yangp2p/YangP2pFactoryI.h>

#include <yangutil/sys/YangSysMessageHandle.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>



#include <pthread.h>

int g_waitState=0;
pthread_mutex_t g_lock ;//= 0;//PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	g_cond ;//PTHREAD_COND_INITIALIZER;
/*
 * ctrl + c controller
 */
static bool b_exit = 0;
static void ctrl_c_handler(int s){
    printf("\ncaught signal %d, exit.\n",s);
    b_exit = true;
    pthread_mutex_lock(&g_lock);
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_lock);
}


static bool b_reload = 0;
static void reload_handler(int s){
    printf("\ncaught signal %d, reload.\n",s);
    b_reload = true;
}
//如需自己实现注释掉
#include <yangp2p/YangP2pFactoryImpl.h>

int main(int argc, char* argv[])
{
	//如需自己实现替换掉
	YangP2pFactoryImpl factory;
    struct sigaction    sigIntHandler;
    struct sigaction    sigHupHandler;
    g_lock = PTHREAD_MUTEX_INITIALIZER;
    g_cond = PTHREAD_COND_INITIALIZER;
    //Http Server


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




    YangIpc w;
    YangP2pFactory mf;

    YangSysMessageHandle *sysmessage=mf.createP2pMessageHandle(w.m_hasAudio,w.m_context,&w,&w,&factory);


    w.m_message=sysmessage;
    sysmessage->start();
    yang_usleep(500);
    w.initPreview();


	while(!b_exit)
	{


		  g_waitState=1;
		        pthread_cond_wait(&g_cond, &g_lock);
		        g_waitState=0;
		if (b_reload) {
            //reload
    		b_reload = false;

		}
	}

EXIT_PROC:




    return 0;
}
