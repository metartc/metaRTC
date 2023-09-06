//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGUTIL_SYS_YANGLOADLIB_H_
#define YANGUTIL_SYS_YANGLOADLIB_H_
#include <yangutil/yangtype.h>
#if Yang_OS_WIN
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

#if Yang_OS_WIN
HMODULE m_handle;
char *dlerror();
#else
void *m_handle;
#endif

};

#endif /* YANGUTIL_SYS_YANGLOADLIB_H_ */
