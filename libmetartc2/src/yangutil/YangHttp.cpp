#include <yangutil/sys/YangHttp.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangString.h>
#include <yangutil/sys/YangFile.h>
#include <vector>
using namespace std;

void YangHttp::loadLib(){

	yang_curl_global_init=(CURLcode (*)(long flags))m_lib.loadFunction("curl_global_init");
	yang_curl_easy_init=(CURL* (*)(void))m_lib.loadFunction("curl_easy_init");
	yang_curl_easy_setopt=(CURLcode (*)(CURL *curl, CURLoption option, ...))m_lib.loadFunction("curl_easy_setopt");
	yang_curl_slist_append=(struct curl_slist* (*)(struct curl_slist *,
		                                                 const char *))m_lib.loadFunction("curl_slist_append");
	yang_curl_easy_perform=(CURLcode (*)(CURL *curl))m_lib.loadFunction("curl_easy_perform");
	yang_curl_easy_cleanup=(void (*)(CURL *curl))m_lib.loadFunction("curl_easy_cleanup");
	yang_curl_global_cleanup=(void (*)(void))m_lib.loadFunction("curl_global_cleanup");
}
void YangHttp::unloadLib(){
	yang_curl_global_init=NULL;
	yang_curl_easy_init=NULL;
	yang_curl_easy_setopt=NULL;
	yang_curl_slist_append=NULL;
	yang_curl_easy_perform=NULL;
	yang_curl_easy_cleanup=NULL;
	yang_curl_global_cleanup=NULL;
}

YangHttp::YangHttp()
{
	unloadLib();
	m_lib.loadObject("libcurl");
    loadLib();
}
YangHttp::~YangHttp()
{
	unloadLib();
	m_lib.unloadObject();
}
size_t YangHttp::WriteFunction(void* input, size_t uSize, size_t uCount, void* avg)
{
	size_t uLen = uSize * uCount;
	string* pStr = (string*)(avg);
	pStr->append((char*)(input), uLen);
	return uLen;
}
void string_replace(std::string& strBig, const std::string& strsrc, const std::string& strdst)
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}
int32_t YangHttp::queryPost(char* purl1, string psdp,string &outsdp)
{

    CURL* curl = NULL;
	CURLcode code;
	code = yang_curl_global_init(CURL_GLOBAL_DEFAULT);
	if (code != CURLE_OK) yang_error(" http global init error");
    curl = yang_curl_easy_init();
    if (curl == NULL) yang_error(" http easy init error");



    yang_curl_easy_setopt(curl, CURLOPT_URL, purl1);
    yang_curl_easy_setopt(curl, CURLOPT_POST, 1);
    yang_curl_easy_setopt(curl, CURLOPT_POSTFIELDS, psdp.c_str());
    yang_curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, psdp.length());

    yang_curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteFunction);
    yang_curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outsdp);



	curl_slist* pHeaders = NULL;
	pHeaders = yang_curl_slist_append(pHeaders, "Content-Type:application/json;charset=UTF-8");
    yang_curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaders);
    yang_curl_easy_setopt(curl, CURLOPT_POST, 1);
    code = yang_curl_easy_perform(curl);
	if (code != CURLE_OK) {
		//string err = "curl_easy_perform() Err...code=="+ to_string((int)code);
		printf("curl_easy_perform() Err...code==%d",(int)code);
		return 1;
	}


    yang_curl_easy_cleanup(curl);
	yang_curl_global_cleanup();

	return Yang_Ok;

}

int32_t YangHttp::querySslPost(char* purl, std::string psdp,std::string &outsdp){
    CURL* curl = NULL;
    CURLcode code;
    code = yang_curl_global_init(CURL_GLOBAL_DEFAULT);
    if (code != CURLE_OK) yang_error(" http global init error");
    curl = yang_curl_easy_init();
    if (curl == NULL) yang_error(" http easy init error");

    yang_curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    yang_curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    char cerpath[255]={0};
    char privatekey[255]={0};

    yang_getCaFile(cerpath,privatekey);
    yang_curl_easy_setopt(curl, CURLOPT_CAINFO, cerpath);
    yang_curl_easy_setopt(curl, CURLOPT_URL, purl); 	//
    yang_curl_easy_setopt(curl, CURLOPT_POST, 1);
    yang_curl_easy_setopt(curl, CURLOPT_POSTFIELDS, psdp.c_str());
    yang_curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, psdp.length());

    yang_curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteFunction);
    yang_curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outsdp);

    curl_slist* pHeaders = NULL;

    pHeaders = yang_curl_slist_append(pHeaders, "Content-Type:application/json;charset=UTF-8");
    yang_curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaders);
    yang_curl_easy_setopt(curl, CURLOPT_POST, 1);
    code = yang_curl_easy_perform(curl);
    if (code != CURLE_OK) {
        //string err = "curl_easy_perform() Err...code=="+ to_string((int)code);
        return yang_error_wrap(code,"curl_easy_perform() Err...code==%d",(int)code);
        //return 1;
    }


    yang_curl_easy_cleanup(curl);
    yang_curl_global_cleanup();

    return Yang_Ok;
}

void yang_http_post(){

}
