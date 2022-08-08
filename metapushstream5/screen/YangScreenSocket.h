//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangScreenSocket_
#define YangScreenSocket_

#include <QObject>
#include <iostream>
#include <QThread>
#include "screen/yangscreenevent.h"
#include "yangutil/yangavinfotype.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
//#define SOCKADDR_IN
#define socklen_t int
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
class MessageObject:public QObject
{
    Q_OBJECT
public:
    MessageObject(){};
    ~MessageObject(){};
    void message();
Q_SIGNALS:
   void SocketError();



};

class YangScreenSocket: public QThread
{
public:
    YangScreenSocket(void);
    virtual ~YangScreenSocket(void);

    int m_localPort;


    void init(void);
    int connectServer();
    int reconnectServer();
    void cycle();
    void tcpCycle();
    void sendTcp(char* str,int plen);
    int sendHeartbeat();

    void run() override;

    int m_loop;
    int m_isRuning;

    YangScreenEventI* m_screenEvent;
    YangContext *m_context;
};

#endif
