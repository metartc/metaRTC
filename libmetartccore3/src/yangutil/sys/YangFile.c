//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangFile.h>
#include <yangutil/yang_unistd.h>
#include <stdio.h>

#include <string.h>

#include <sys/stat.h>
#include <yangutil/YangErrorCode.h>


#ifdef _MSC_VER
#include <direct.h>
#endif

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



