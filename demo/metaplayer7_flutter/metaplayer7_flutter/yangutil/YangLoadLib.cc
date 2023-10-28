//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "YangLoadLib.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define yang_error printf
#ifdef _WIN32
#include <windows.h>
#include <basetyps.h>
#ifdef _MSC_VER
#include <direct.h>
#endif
#else
   #include <dlfcn.h>
#endif

int32_t yang_getLibpath(char* path){

	char tmp[255];
	memset(tmp,0,sizeof(tmp));
#ifdef _MSC_VER
	if(_getcwd(tmp, 255)) {

#else
		if(getcwd(tmp, 255)) {
#endif
         
			sprintf(path,"%s",tmp);
			return 0;
		}
		return 1;
	}

YangLoadLib::YangLoadLib(){
	m_handle=NULL;
}
YangLoadLib::~YangLoadLib(){
	unloadObject();
}
void* YangLoadLib::loadSysObject(const char *sofile)
{

#ifdef _WIN32
   // LPCSTR
    m_handle = LoadLibraryA(sofile);
#else
    m_handle = dlopen(sofile, RTLD_NOW|RTLD_LOCAL);
#endif

    if (m_handle == 0) {
          	yang_error("Failed loading %s: %s", sofile, (char *) dlerror());
    }
    return (m_handle);
}
#define LENTH 200
void* YangLoadLib::loadObject(const char *sofile)
{


	char file1[LENTH+50];
	char file_path_getcwd[LENTH];
	memset(file1, 0, LENTH+50);
	memset(file_path_getcwd, 0, LENTH);
    if(yang_getLibpath(file_path_getcwd)!=0){
		yang_error( "Failed loading shared obj %s: %s,getcwd error!", sofile, (char *) dlerror());
		return NULL;
	}

#ifdef _WIN32
	sprintf(file1, "%s/lib/%s.dll", file_path_getcwd, sofile);
    m_handle =  LoadLibraryA(file1);
#else
    sprintf(file1, "%s/lib/%s.so", file_path_getcwd, sofile);
  
    m_handle = dlopen(file1, RTLD_NOW|RTLD_LOCAL);
#endif

    if (m_handle == 0) {

    	yang_error( "Failed loading shared obj %s: %s", sofile, (char *) dlerror());
    }
    return (m_handle);
}
#ifdef _WIN32
char *YangLoadLib::dlerror(){
    return (char*)"loadlib error";
}
#endif

void* YangLoadLib::loadFunction( const char *name)
{

#ifdef _WIN32
	void *symbol = (void *) GetProcAddress(m_handle, name);
#else
	void *symbol = dlsym(m_handle, name);
#endif

    if (symbol == NULL) {
        	yang_error("Failed loading function %s: %s", name,        (const char *) dlerror());
    }
    return (symbol);
}

void YangLoadLib::unloadObject()
{
    if (m_handle) {
#ifdef _WIN32
    	 FreeLibrary( m_handle);
#else
	 dlclose(m_handle);;
#endif
	 m_handle=NULL;

    }
}

//#endif

