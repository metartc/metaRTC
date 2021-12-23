#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <stdio.h>
#include <string>
#include <string.h>

#include <sys/stat.h>

#include <iostream>

#ifdef _MSC_VER
#include <direct.h>
#endif

YangFile::YangFile() {


}

YangFile::~YangFile() {

}

int32_t YangFile::hasPath(const char* path){
  //  _waccess(path,0);
#ifndef _MSC_VER
	if( access(path, F_OK)!=0) return 1;
	if( access(path, W_OK)!=0) return 2;
#endif
	return 0;
}

int32_t YangFile::createPath(const char* path){

    #ifdef _WIN32
    #ifdef _MSC_VER
    return _mkdir(path);
    #else
        return mkdir(path);
    #endif
    #else
    mode_t mode=0755;
    return mkdir(path,mode);
    #endif

}
int32_t YangFile::getCurpath(char* path){
#ifdef _MSC_VER
    if(_getcwd(path, 255)) return Yang_Ok;
#else
    if(getcwd(path, 255)) return Yang_Ok;
#endif
	return 1;
}


int32_t yang_getCurpath(char* path){
#ifdef _MSC_VER
    if(_getcwd(path, 255)) return Yang_Ok;
#else
    if(getcwd(path, 255)) return Yang_Ok;
#endif
	return 1;
}
int32_t yang_getLibpath(char* path){

char tmp[255];
memset(tmp,0,sizeof(tmp));
#ifdef _MSC_VER
    if(_getcwd(tmp, 255)) {

#else
    if(getcwd(tmp, 255)) {
#endif
    	sprintf(path,"%s/lib",tmp);
    	return Yang_Ok;
    }
	return 1;
}

    int32_t yang_getCaFile(char* pem,char* key){
    	char tmp[200];
    	memset(tmp,0,sizeof(tmp));
#ifdef _MSC_VER
    if(_getcwd(tmp, 200)) {

#else
    if(getcwd(tmp, 200)) {
#endif
    	sprintf(pem,"%s/lib/client.pem",tmp);
    	sprintf(key,"%s/lib/client.key",tmp);
    	return Yang_Ok;
    }
    return 1;
    }

/**
int32_t createDirectory(std::string path)
{
	int32_t len = path.length();
	char tmpDirPath[256] = { 0 };
	for (int32_t i = 0; i < len; i++)
	{
		tmpDirPath[i] = path[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (_access(tmpDirPath, 0) == -1)
			{
				int32_t ret = _mkdir(tmpDirPath);
				if (ret == -1) return ret;
			}
		}
	}
	return 0;
}
**/
