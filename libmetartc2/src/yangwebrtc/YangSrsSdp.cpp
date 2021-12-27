#include <yangwebrtc/YangSrsSdp.h>
#include <yangwebrtc/YangRtcSdp.h>

#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangString.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangHttp.h>
#include <yangutil/sys/YangHttpSocket.h>
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


int32_t YangSrsSdp::querySrs(char* ip,int32_t port,char* purl, string psdp)
{

	//YangHttp m_http;
	//m_http.queryPost(purl, psdp, sBuffer);
	yang_trace("\n************************querySrs****************************\n");
	//YangHttpSocket http;
	char* sdp=(char*)calloc(1024*12,1);
	if(yang_http_post(sdp,ip, port, purl, (uint8_t*)psdp.c_str(), psdp.length())){
		yang_free(sdp);
		return yang_error_wrap(1,"query srs sdp failure!");
	}

	string sBuffer=sdp;
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

	yang_free(sdp);
	if(m_srs.retcode!="0") return 1;
	return Yang_Ok;

}
