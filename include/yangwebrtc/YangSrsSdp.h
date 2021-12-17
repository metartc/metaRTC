#ifndef YANGRTP_Httpurl_H_
#define YANGRTP_Httpurl_H_
#include <yangutil/sys/YangHttp.h>
#include <yangstream/YangStreamType.h>
using namespace std;
class YangSrsSdp
{
public:
	YangSrsSdp();
	~YangSrsSdp();

	int32_t querySrs(char* purl, string psdp);
	SrsSdpResponseType m_srs;
	YangHttp m_http;

};

#endif
