#ifndef YANGWEBRTC_YANGUDPHANDLE_H_
#define YANGWEBRTC_YANGUDPHANDLE_H_
#ifdef __cplusplus
extern "C"{
#include <yangwebrtc/YangCUdpHandle.h>
}
#else
#include <yangwebrtc/YangCUdpHandle.h>
#endif

/**
using namespace std;
class YangUdpHandle:public YangThread ,public YangSendUdpData{
public:
	YangUdpHandle();
	virtual ~YangUdpHandle();

	void init(YangRtcSession* psession,char* pserverIp,int32_t plocalPort);
	int32_t sendUdp(const char *data, int32_t len);
	void setSockSrv(const char *ip, int32_t port);
	int32_t sendData(char* p,int32_t plen);
	void closeUdp();
	int32_t m_isStart;
	void stop();
protected:
	void run();
	void startLoop();
	void receive(char* data,int32_t plen);

	YangRtcSession *m_session;
	int32_t m_serverfd;
	int32_t m_lclPort;

	string m_serverIp;
	int32_t m_serverPort;
	int32_t m_loop;
	struct sockaddr_in m_lcl_addr,m_remote_addr;
};
**/
#endif /* YANGWEBRTC_YANGUDPHANDLE_H_ */
