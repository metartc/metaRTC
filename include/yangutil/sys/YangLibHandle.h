//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_SYS_YANGLIBHANDLE_H_
#define INCLUDE_YANGUTIL_SYS_YANGLIBHANDLE_H_

	typedef struct{

	void* context;
	void* (*loadObject)(void* context,const char *sofile);
	void* (*loadSysObject)(void* context,const char *sofile);
	void* (*loadFunction)(void* context,const char *name);
	void (*unloadObject)(void* context);
	char* (*getError)();
	}YangLibHandle;
#ifdef __cplusplus
extern "C"{
#endif
void yang_create_libhandle(YangLibHandle* handle);
void yang_destroy_libhandle(YangLibHandle* handle);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGUTIL_SYS_YANGLIBHANDLE_H_ */
