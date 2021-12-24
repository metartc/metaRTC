#ifndef YANGRTP_YANGWEBSOCKET_H_
#define YANGRTP_YANGWEBSOCKET_H_
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangUrl.h>
#include <yangutil/sys/YangThread.h>

struct YangWebsocketData{
	uint8_t payload[1024];
	int nb;
};
class YangWebsocketCallback{
public:
	YangWebsocketCallback(){};
	virtual ~YangWebsocketCallback(){};
	virtual int receive(YangWebsocketData* data)=0;
};


class YangWebsocket :public YangThread{
public:
	YangWebsocket();
	virtual ~YangWebsocket();

	virtual int connectServer(std::string url)=0;
	virtual int disconnectServer()=0;
	virtual int sendData(uint8_t* p,int nb)=0;

	int m_isStart;
	void stop();

	void setReceiveCallback(YangWebsocketCallback* pcallback);
	YangWebsocketCallback* getReceiveCallback();
	static YangWebsocket* getWebsocket();

protected:
	void run();
	virtual void startLoop()=0;
	virtual void stopLoop()=0;

	YangUrlData m_urlData;
	YangWebsocketCallback* m_receiveCallback;


};

#endif /* YANGRTP_YANGWEBSOCKET_H_ */
