#include <yangutil/sys/YangWebsocket.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangLog.h>



YangWebsocket::YangWebsocket() {
	m_receiveCallback=NULL;
	m_isStart=0;
}

YangWebsocket::~YangWebsocket() {

}

void YangWebsocket::setReceiveCallback(YangWebsocketCallback* pcallback){
	m_receiveCallback=pcallback;
}
YangWebsocketCallback* YangWebsocket::getReceiveCallback(){
	return m_receiveCallback;
}



void YangWebsocket::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangWebsocket::stop() {

	stopLoop();

}



