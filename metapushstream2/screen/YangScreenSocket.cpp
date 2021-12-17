#include "YangScreenSocket.h"
#include <QDebug>
#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>

#include <fcntl.h>
#include <string>
#include <sstream>
#include <memory.h>
#include <vector>
#include <QMessageBox>
using namespace std;
#ifdef _WIN32
#define sockError()	WSAGetLastError()
#else
#endif
void MessageObject::message(){
    Q_EMIT SocketError();
}

YangScreenSocket::YangScreenSocket(void) {
    m_loop=0;
    m_isRuning=0;
    m_localPort=9996;
    m_context=nullptr;
    m_screenEvent=nullptr;
}

YangScreenSocket::~YangScreenSocket(void) {
    m_loop=0;

    while(m_isRuning){
        msleep(1);
    }
    m_context=NULL;
    //this->quit();
}
void YangScreenSocket::init(void) {


}
void YangScreenSocket::run(){
    m_isRuning=1;
    tcpCycle();
    m_isRuning=0;
}
int YangScreenSocket::sendHeartbeat(){

    return Yang_Ok;
}
void YangScreenSocket::sendTcp(char* str,int plen){

}
void YangScreenSocket::tcpCycle(){



}


int YangScreenSocket::reconnectServer(){
return Yang_Ok;
}

int YangScreenSocket::connectServer(){
return Yang_Ok;

}
