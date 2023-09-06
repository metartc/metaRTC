//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangLoadLib.h>
#include <yangutil/sys/YangLog.h>

extern "C"{
#include <yangutil/sys/YangFile.h>
}

#if Yang_OS_WIN
#include <windows.h>
#include <basetyps.h>
#ifdef _MSC_VER
#include <direct.h>
#endif
#else
   #include <dlfcn.h>
#endif



YangLoadLib::YangLoadLib(){
	m_handle=NULL;
}
YangLoadLib::~YangLoadLib(){
	unloadObject();
}
void* YangLoadLib::loadSysObject(const char *sofile)
{

#if Yang_OS_WIN
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
    if(yang_getLibpath(file_path_getcwd)!=Yang_Ok){
		yang_error( "Failed loading shared obj %s: %s,getcwd error!", sofile, (char *) dlerror());
		return NULL;
	}

#if Yang_OS_WIN
	sprintf(file1, "%s/%s.dll", file_path_getcwd, sofile);
    m_handle =  LoadLibraryA(file1);
#else
    sprintf(file1, "%s/%s.so", file_path_getcwd, sofile);
    m_handle = dlopen(file1, RTLD_NOW|RTLD_LOCAL);
#endif

    if (m_handle == 0) {

    	yang_error( "Failed loading shared obj %s: %s", sofile, (char *) dlerror());
    }
    return (m_handle);
}
#if Yang_OS_WIN
char *YangLoadLib::dlerror(){
    return (char*)"loadlib error";
}
#endif

void* YangLoadLib::loadFunction( const char *name)
{

#if Yang_OS_WIN
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
#if Yang_OS_WIN
    	 FreeLibrary( m_handle);
#else
	 dlclose(m_handle);;
#endif
	 m_handle=NULL;

    }
}

//#endif

