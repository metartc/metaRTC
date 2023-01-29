//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YANGUTIL_VIDEO_YANGEndian_H_
#define YANGUTIL_VIDEO_YANGEndian_H_
#include <yangutil/yangtype.h>
#ifdef __cplusplus
extern "C"{
#endif
char* yang_put_amf_string(char *c, const char *str);
char* yang_put_amf_double(char *c, double d);
char* yang_put_byte(char *output, uint8_t nVal);
char* yang_put_be16(char *output, uint16_t nVal);
char* yang_put_be24(char *output, uint32_t nVal);
char* yang_put_be32(char *output, uint32_t nVal);
char* yang_put_be64(char *output, uint64_t nVal);

uint32_t yang_get_be32(uint8_t *output);
uint16_t  yang_get_be16(uint8_t *output);
#ifdef __cplusplus
}
#endif
#endif /* YANGUTIL_VIDEO_YANGCOMMON_H_ */
