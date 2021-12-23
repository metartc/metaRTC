
#ifndef INCLUDE_YANGUTIL_SYS_YANGFILE_H_
#define INCLUDE_YANGUTIL_SYS_YANGFILE_H_
#include <stdint.h>
int32_t yang_getCurpath(char* path);
int32_t yang_getLibpath(char* path);
int32_t yang_getCaFile(char* pem,char* key);

class YangFile {
public:
	YangFile();
	virtual ~YangFile();
	int32_t hasPath(const char* path);
	int32_t createPath(const char* path);
	int32_t getCurpath(char* path);

};

#endif /* INCLUDE_YANGUTIL_SYS_YANGFILE_H_ */
