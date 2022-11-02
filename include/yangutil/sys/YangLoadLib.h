//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGUTIL_SYS_YANGLOADLIB_H_
#define YANGUTIL_SYS_YANGLOADLIB_H_
#include <stdint.h>
#ifdef _WIN32
#include <minwindef.h>
#endif
class YangLoadLib{
public:
	YangLoadLib();
	~YangLoadLib();

void *loadObject(const char *sofile);
void *loadSysObject(const char *sofile);
void *loadFunction(const char *name);
void unloadObject();

#ifdef _WIN32
HMODULE m_handle;
char *dlerror();
#else
void *m_handle;
#endif

};

#endif /* YANGUTIL_SYS_YANGLOADLIB_H_ */
