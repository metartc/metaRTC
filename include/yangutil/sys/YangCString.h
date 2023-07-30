//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGCSTRING_H_
#define INCLUDE_YANGUTIL_SYS_YANGCSTRING_H_

#include <yangutil/yangtype.h>

typedef struct {
    int32_t capacity;
    int32_t vsize;
    char **str;
}YangStrings;

#ifdef __cplusplus
extern "C"{
#endif
void yang_cint32_random(int32_t len,char* data);
void yang_cstr_random(int32_t len,char* data);
int32_t yang_cstr_split(char *src, char *delim, YangStrings* istr);
void yang_cstr_replace(char *str,char* dst, char *orig, char *rep);
void yang_destroy_strings(YangStrings* strs);
int32_t yang_cstr_userfindindex(char* p,char c);
int32_t yang_cstr_userfindupindex(char* p,char c,int32_t n);
int32_t yang_cstr_isnumber(char* p,int32_t n);
int32_t yang_yang_strcmp(char* str1,char* str2);
void yang_itoa(int32_t num,char* data,int32_t n);
void yang_itoa2(uint32_t num,char* data,int32_t n);
int32_t yang_get_line(char* buf,char *line, int32_t n);
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_YANGUTIL_SYS_YANGCSTRING_H_ */
