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
#include <yangutil/yang_unistd.h>
#include <pthread.h>



//如需自己实现注释掉
#include <yangp2p/YangP2pFactoryImpl.h>

int main(int argc, char* argv[])
{
	//如需自己实现替换掉
	YangP2pFactoryImpl factory;


    //Http Server



    YangIpc w;
    YangP2pFactory mf;

    YangSysMessageHandle *sysmessage=mf.createP2pMessageHandle(w.m_hasAudio,w.m_context,&w,&w,&factory);


    w.m_message=sysmessage;
    sysmessage->start();
    yang_usleep(500);
    w.initPreview();

    char c;
	while((c=getchar())!='q')
	{
        Sleep(1000);
	
	}


}
