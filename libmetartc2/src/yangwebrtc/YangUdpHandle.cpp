#include <yangwebrtc/YangUdpHandle.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>





YangUdpHandle::YangUdpHandle() {
	m_lclPort = 8100;
	m_serverfd = -1;
	m_loop = 0;
	m_serverPort = 8000;
	m_isStart = 0;
	m_session = NULL;
}

YangUdpHandle::~YangUdpHandle() {
	stop();
	m_session = NULL;
	while (m_isStart)
		yang_usleep(1000);
}
void YangUdpHandle::init(YangRtcSession *psession, char *pserverIp,
		int32_t plocalPort) {
	m_session = psession;
	m_serverIp = pserverIp;
	m_lclPort = plocalPort;

#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif
	m_lcl_addr.sin_family = AF_INET;
	m_lcl_addr.sin_port = htons(m_lclPort);
	// lcl_addr.sin_addr.s_addr=inet_addr(ip);
	m_lcl_addr.sin_addr.s_addr = INADDR_ANY;
	m_serverfd = socket(AF_INET, SOCK_DGRAM, 0);

	m_remote_addr.sin_family = AF_INET;
	m_remote_addr.sin_port = htons(m_serverPort);
	m_remote_addr.sin_addr.s_addr = inet_addr(m_serverIp.c_str());

}

void YangUdpHandle::run() {
	m_isStart = 1;
	startLoop();
	m_isStart = 0;
}
void YangUdpHandle::stop() {
	m_loop = 0;
	closeUdp();

}
int32_t YangUdpHandle::sendUdp(const char *data, int32_t len) {

	int32_t n = 0;
	n = sendto(m_serverfd, data, len, 0, (sockaddr*) &m_remote_addr,
			sizeof(sockaddr));

	return n > 0 ? 0 : 1;
}
int32_t YangUdpHandle::sendData(char *p, int32_t plen) {
	return sendUdp(p, plen);
}

void YangUdpHandle::startLoop() {
#ifdef _WIN32
    int32_t timeout=200;
    setsockopt(m_serverfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeout,  sizeof(timeout));
#else
    struct timeval tv;
	tv.tv_sec = 0;
    tv.tv_usec = 200000;  // 200 ms
    setsockopt(m_serverfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv,	sizeof(struct timeval));
#endif

	printf("\n udp server is starting..localPort==%d",m_lclPort);

#ifdef _WIN32
    if (bind(m_serverfd, (struct sockaddr*) &m_lcl_addr, sizeof(struct sockaddr_in)) < 0) {
#else
	if (bind(m_serverfd, (struct sockaddr*) &m_lcl_addr,
			sizeof(struct sockaddr_in)) < 0) {
#endif

		perror("Udp server bind error");
		exit(1);
	}
	char buffer[2048] = { 0 };
	m_loop = 1;

	int32_t len = 0;
	if (m_session)
		m_session->startStunTimer();

	while (m_loop) {
		struct sockaddr_in src;
		socklen_t src_len = sizeof(src);
		memset(&src, 0, sizeof(src));
		memset(buffer, 0, 2048);
		if ((len = recvfrom(m_serverfd, buffer, 2048, 0, (sockaddr*) &src,
				&src_len)) > 0) {
            if (m_session)
				m_session->receive(buffer, len);
		}
	}

	closeUdp();

}

void YangUdpHandle::closeUdp() {
	if (m_serverfd>-1) {
#ifdef _WIN32
    		closesocket(m_serverfd);
#else
		close(m_serverfd);
#endif
		m_serverfd = -1;
	}

}
