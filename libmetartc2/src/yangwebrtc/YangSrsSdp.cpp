#include <yangwebrtc/YangSrsSdp.h>
#include <yangwebrtc/YangRtcSdp.h>

#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangString.h>
#include <yangutil/sys/YangLog.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;




YangSrsSdp::YangSrsSdp()
{

}
YangSrsSdp::~YangSrsSdp()
{

}


int32_t YangSrsSdp::querySrs(char* purl, string psdp)
{
	string sBuffer;
	m_http.queryPost(purl, psdp, sBuffer);
	yang_replace(sBuffer, "{", "");
	yang_replace(sBuffer, "}", "");
	yang_replace(sBuffer, "\\r\\n", "\r\n");
	vector<string> sb=yang_split(sBuffer, ',');
	for (int32_t i = 0; i < (int)sb.size(); i++) {
		vector<string> sb1 = yang_split_first(sb.at(i), ':');
		string key = sb1.at(0);
		string value = sb1.at(1);
		yang_replace(key, "\"", "");
		yang_replace(value, "\"", "");
		if (key.find("code") != key.npos) m_srs.retcode = value;
		if (key.find("server") != key.npos) m_srs.serverIp = value;
		if (key.find("sdp") != key.npos) {
			m_srs.sdp = value;
		}
		if (key.find("sessionid") != key.npos) m_srs.sessionid = value;
		
	}
	if(m_srs.retcode!="0") return 1;
	return Yang_Ok;

}
