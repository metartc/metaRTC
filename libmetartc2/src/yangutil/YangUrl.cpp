#include <yangutil/sys/YangUrl.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSocket.h>
#include <string.h>


//webrtc://host[:port]/app/stream
int32_t yang_srs_url_parse(char* url,YangUrlData* data) {


	char *p = strstr(url, "://");
	if (!p) {
		yang_error("Srs Webrt URL: No :// in url!");
		return 1;
	}
	int32_t len = (int) (p - url);
    if (len == 4 && memcmp(url, "rtmp", 4) == 0) {
		data->netType = Yang_Rtmp;
		data->port=1935;
    } else if (len == 6 && memcmp(url, "webrtc", 6) == 0) {
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

	char *end = p + strlen(p);
	char *col = strchr(p, ':');
	//schar *ques = strchr(p, '?');
	char *slash = strchr(p, '/');
	int32_t hostlen;
	if (slash)
		hostlen = slash - p;
	else
		hostlen = end - p;
	if (col && col - p < hostlen)
		hostlen = col - p;

	if (hostlen < 256) {
		  yang_getIp(std::string(p, hostlen),data->server);

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

	slash2 = strchr(p, '/');
	if (slash2)
		slash3 = strchr(slash2 + 1, '/');
	if (slash3)
		slash4 = strchr(slash3 + 1, '/');

	applen = end - p; /* ondemand, pass all parameters as app */
	appnamelen = applen; /* ondemand length */
	if (slash4)
		appnamelen = slash4 - p;
	else if (slash3)
		appnamelen = slash3 - p;
	else if (slash2)
		appnamelen = slash2 - p;

	applen = appnamelen;
	data->app = std::string(p, applen);
	p += appnamelen;

	//parse streamName
	if (*p == '/')
		p++;

	if (end - p) {
		data->stream = std::string(p, end - p);
	}

	return Yang_Ok;
}


//http://host:port/path ws://host:port/path wss://host:port/path
int32_t yang_ws_url_parse(char* url,YangUrlData* data) {


	char *p = strstr(url, "://");
	if (!p) {
        yang_error("Ws Webrt URL: No :// in url!");
		return 1;
	}
	int32_t len = (int) (p - url);

    if (len == 2 && memcmp(url, "ws", 2) == 0) {
		data->netType = Yang_Websocket_Ws;
		data->port=8088;
    }else   if (len == 3 && memcmp(url, "wss", 3) == 0) {
		data->netType = Yang_Websocket_Wss;
		data->port=8089;
    }else if (len == 4 && memcmp(url, "http", 4) == 0) {
		data->netType = Yang_Websocket_Http;
		data->port=8088;
    } else if (len == 5 && memcmp(url, "https", 5) == 0) {
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

	char *end = p + strlen(p);
	char *col = strchr(p, ':');
	//schar *ques = strchr(p, '?');
	char *slash = strchr(p, '/');
	int32_t hostlen;
	if (slash)
		hostlen = slash - p;
	else
		hostlen = end - p;
	if (col && col - p < hostlen)
		hostlen = col - p;

	if (hostlen < 256) {
		data->server = std::string(p, hostlen);

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

	slash2 = strchr(p, '/');
	if (slash2)
		slash3 = strchr(slash2 + 1, '/');
	if (slash3)
		slash4 = strchr(slash3 + 1, '/');

	applen = end - p; /* ondemand, pass all parameters as app */
	appnamelen = applen; /* ondemand length */
	if (slash4)
		appnamelen = slash4 - p;
	else if (slash3)
		appnamelen = slash3 - p;
	else if (slash2)
		appnamelen = slash2 - p;

	applen = appnamelen;
	data->app = std::string(p, applen);


	return Yang_Ok;
}




