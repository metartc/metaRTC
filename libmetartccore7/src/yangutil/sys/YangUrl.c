//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangCUrl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>

//webrtc://host[:port]/app/stream
int32_t yang_url_parse(YangIpFamilyType familyType,char* url,YangUrlData* data) {


	char *p = yang_strstr(url, "://");
	if (!p) {
		yang_error("Srs Webrt URL: No :// in url!");
		return 1;
	}
	int32_t len = (int) (p - url);
    if (len == 4 && yang_memcmp(url, "rtmp", 4) == 0) {
		data->netType = Yang_Rtmp;
		data->port=1935;
    } else if (len == 6 && yang_memcmp(url, "webrtc", 6) == 0) {
        data->netType = Yang_Webrtc;
        data->port=1985;
	} else {
		return 1;
	}
	p += 3;
	if (*p == 0) {
		yang_warn("No hostname in URL!");
		return 1;
	}

	char *end = p + yang_strlen(p);
	char *col = yang_strchr(p, ':');
	//schar *ques = yang_strchr(p, '?');
	char *slash = yang_strchr(p, '/');
	int32_t hostlen;
	if (slash)
		hostlen = slash - p;
	else
		hostlen = end - p;
	if (col && col - p < hostlen)
		hostlen = col - p;

	if (hostlen < 256) {
		 char s1[256]={0};
		 yang_memcpy(s1,p,hostlen);
		 yang_memset(data->server,0,sizeof(data->server));
		 yang_getIp(familyType,s1,data->server);

	} else {
		yang_warn("Hostname exceeds 255 characters!");
	}

	p += hostlen;

	if (*p == ':') {
		uint32_t  p2;
		p++;
		p2 = atoi(p);
		if (p2 > 65535) {
			yang_warn("Invalid port number!");
		} else {
			data->port = p2;
		}
	}

	if (!slash) {
		yang_warn("No application or playpath in URL!");
		return 0;
	}
	p = slash + 1;
	//parse app
	char *slash2, *slash3 = NULL, *slash4 = NULL;
	int32_t applen, appnamelen;

	slash2 = yang_strchr(p, '/');
	if (slash2)
		slash3 = yang_strchr(slash2 + 1, '/');
	if (slash3)
		slash4 = yang_strchr(slash3 + 1, '/');

	applen = end - p; /* ondemand, pass all parameters as app */
	appnamelen = applen; /* ondemand length */
	if (slash4)
		appnamelen = slash4 - p;
	else if (slash3)
		appnamelen = slash3 - p;
	else if (slash2)
		appnamelen = slash2 - p;

	applen = appnamelen;
	 yang_memset(data->app,0,sizeof(data->app));
	 yang_memcpy(data->app,p,applen);

	p += appnamelen;

	//parse streamName
	if (*p == '/')
		p++;

	if (end - p) {
		 yang_memset(data->stream,0,sizeof(data->stream));
		 yang_memcpy(data->stream,p,end - p);

	}

	return Yang_Ok;
}


//http://host[:port]/app/stream
int32_t yang_http_url_parse(YangIpFamilyType familyType,char* url,YangUrlData* data) {


	char *p = yang_strstr(url, "://");
	if (!p) {
		yang_error("Srs Webrt URL: No :// in url!");
		return 1;
	}
	int32_t len = (int) (p - url);
    if (len == 4 && yang_memcmp(url, "http", 4) == 0) {
    	 data->netType = Yang_Webrtc;
    	 data->port=1985;
    } else if (len == 5 && yang_memcmp(url, "https", 5) == 0) {
        data->netType = Yang_Webrtc;
        data->port=1985;
	} else if (len == 6 && yang_memcmp(url, "webrtc", 6) == 0) {
        data->netType = Yang_Webrtc;
        data->port=1985;
	} else {
		return 1;
	}
	p += 3;
	if (*p == 0) {
		yang_warn("No hostname in URL!");
		return 1;
	}

	char *end = p + yang_strlen(p);
	char *col = yang_strchr(p, ':');
	//schar *ques = yang_strchr(p, '?');
	char *slash = yang_strchr(p, '/');
	int32_t hostlen;
	if (slash)
		hostlen = slash - p;
	else
		hostlen = end - p;
	if (col && col - p < hostlen)
		hostlen = col - p;

	if (hostlen < 256) {
		 char s1[256]={0};
		 yang_memcpy(s1,p,hostlen);
		 yang_memset(data->server,0,sizeof(data->server));
		 yang_getIp(familyType,s1,data->server);

	} else {
		yang_warn("Hostname exceeds 255 characters!");
	}

	p += hostlen;

	if (*p == ':') {
		uint32_t  p2;
		p++;
		p2 = yang_atoi(p);
		if (p2 > 65535) {
			yang_warn("Invalid port number!");
		} else {
			data->port = p2;
		}
	}

	if (!slash) {
		yang_warn("No application or playpath in URL!");
		return 0;
	}
	p = slash + 1;


	 yang_memset(data->stream,0,sizeof(data->stream));
	 yang_strcpy(data->stream,p);


	return Yang_Ok;
}


//http://host:port/path ws://host:port/path wss://host:port/path
int32_t yang_ws_url_parse(YangIpFamilyType familyType,char* url,YangUrlData* data) {


	char *p = yang_strstr(url, "://");
	if (!p) {
        yang_error("Ws Webrt URL: No :// in url!");
		return 1;
	}
	int32_t len = (int) (p - url);

    if (len == 2 && yang_memcmp(url, "ws", 2) == 0) {
		data->netType = Yang_Websocket_Ws;
		data->port=8088;
    }else   if (len == 3 && yang_memcmp(url, "wss", 3) == 0) {
		data->netType = Yang_Websocket_Wss;
		data->port=8089;
    }else if (len == 4 && yang_memcmp(url, "http", 4) == 0) {
		data->netType = Yang_Websocket_Http;
		data->port=8088;
    } else if (len == 5 && yang_memcmp(url, "https", 5) == 0) {
        data->netType = Yang_Websocket_Https;
        data->port=8089;
	} else {
		return 1;
	}
	p += 3;
	if (*p == 0) {
		yang_warn("No hostname in URL!");
		return 1;
	}

	char *end = p + yang_strlen(p);
	char *col = yang_strchr(p, ':');
	//schar *ques = yang_strchr(p, '?');
	char *slash = yang_strchr(p, '/');
	int32_t hostlen;
	if (slash)
		hostlen = slash - p;
	else
		hostlen = end - p;
	if (col && col - p < hostlen)
		hostlen = col - p;

	if (hostlen < 256) {
		yang_memset(data->server,0,sizeof(data->server));
		yang_memcpy(data->server,p, hostlen);
	} else {
		yang_warn("Hostname exceeds 255 characters!");
	}

	p += hostlen;

	if (*p == ':') {
		uint32_t  p2;
		p++;
		p2 = atoi(p);
		if (p2 > 65535) {
			yang_warn("Invalid port number!");
		} else {
			data->port = p2;
		}
	}

	if (!slash) {
		yang_warn("No application or playpath in URL!");
		return 0;
	}
	p = slash + 1;
	//parse app
	char *slash2, *slash3 = NULL, *slash4 = NULL;
	int32_t applen, appnamelen;

	slash2 = yang_strchr(p, '/');
	if (slash2)
		slash3 = yang_strchr(slash2 + 1, '/');
	if (slash3)
		slash4 = yang_strchr(slash3 + 1, '/');

	applen = end - p; /* ondemand, pass all parameters as app */
	appnamelen = applen; /* ondemand length */
	if (slash4)
		appnamelen = slash4 - p;
	else if (slash3)
		appnamelen = slash3 - p;
	else if (slash2)
		appnamelen = slash2 - p;

	applen = appnamelen;

	yang_memset(data->app,0,sizeof(data->app));
	yang_memcpy(data->app,p,applen);

	return Yang_Ok;
}




