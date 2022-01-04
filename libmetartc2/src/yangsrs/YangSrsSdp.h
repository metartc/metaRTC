#ifndef YANGRTP_Httpurl_H_
#define YANGRTP_Httpurl_H_

#include <yangstream/YangStreamType.h>
using namespace std;
class YangSrsSdp
{
public:
	YangSrsSdp();
	~YangSrsSdp();

	int32_t querySrs(char* ip,int32_t port,char* purl, string psdp);
	SrsSdpResponseType m_srs;
};

#endif
