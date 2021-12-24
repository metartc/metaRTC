/*
 * YangCommon.h
 *
 *  Created on: 2019年7月20日
 *      Author: yang
 */

#ifndef YANGUTIL_VIDEO_YANGEndian_H_
#define YANGUTIL_VIDEO_YANGEndian_H_
#include <stdint.h>


char* yang_put_amf_string(char *c, const char *str);
char* yang_put_amf_double(char *c, double d);
char* yang_put_byte(char *output, uint8_t nVal);
char* yang_put_be16(char *output, unsigned short nVal);
char* yang_put_be24(char *output, uint32_t nVal);
char* yang_put_be32(char *output, uint32_t nVal);
char* yang_put_be64(char *output, unsigned long nVal);

uint32_t yang_get_be32(uint8_t *output);
uint16_t  yang_get_be16(uint8_t *output);
#endif /* YANGUTIL_VIDEO_YANGCOMMON_H_ */
