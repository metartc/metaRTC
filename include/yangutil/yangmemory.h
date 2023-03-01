//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef INCLUDE_YANGUTIL_YANGMEMORY_H_
#define INCLUDE_YANGUTIL_YANGMEMORY_H_

#include <string.h>
#define yang_memcpy memcpy
#define yang_malloc malloc
#define yang_calloc calloc
#define yangfree    free
#define yang_memset memset
#define yang_memmove memmove
#define yang_memcmp memcmp
#define yang_realloc realloc


#define yang_strcat strcat
#define yang_strncat strncat
#define yang_strcpy strcpy
#define yang_strncpy strncpy
#define yang_strlen strlen
#define yang_strnlen strnlen
#define yang_strchr strchr
#define yang_strnchr strnchr
#define yang_strrchr strrchr
#define yang_strcmp strcmp
#define yang_strncmp strncmp
#define yang_printf  printf
#define yang_sprintf sprintf
#define yang_snprintf snprintf
#define yang_vsnprintf vsnprintf
#define yang_sscanf   sscanf
#define yang_trimstrall trimstrall
#define yang_ltrimstr   ltrimstr
#define yang_rtrimstr   rtrimstr
#define yang_strstr strstr

#define yang_tolower    tolower
#define yang_toupper toupper

#define yang_atoi atoi
#define yang_atol atol

#define yang_static_strlen(string_literal) (sizeof(string_literal) - sizeof(""))


#endif /* INCLUDE_YANGUTIL_YANGMEMORY_H_ */
