//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGHTTP_H_
#define INCLUDE_YANGUTIL_SYS_YANGHTTP_H_
#include <yangutil/yangtype.h>
#if Yang_Enable_Curl
#include <yangutil/sys/YangLoadLib.h>
#include <curl/curl.h>
#include <string>

class YangHttp {
public:
	YangHttp();
	virtual ~YangHttp();
	static size_t WriteFunction(void* input, size_t uSize, size_t uCount, void* avg);
		int32_t queryPost(char* purl, std::string psdp,std::string &outsdp);
        int32_t querySslPost(char* purl, std::string psdp,std::string &outsdp);

	private:
		YangLoadLib m_lib;
		void loadLib();
		void unloadLib();
		CURLcode (*yang_curl_global_init)(long flags);
		CURL* (*yang_curl_easy_init)(void);
		CURLcode (*yang_curl_easy_setopt)(CURL *curl, CURLoption option, ...);
		struct curl_slist* (*yang_curl_slist_append)(struct curl_slist *,
		                                                 const char *);
		CURLcode (*yang_curl_easy_perform)(CURL *curl);
		void (*yang_curl_easy_cleanup)(CURL *curl);
		void (*yang_curl_global_cleanup)(void);
};
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGHTTP_H_ */
