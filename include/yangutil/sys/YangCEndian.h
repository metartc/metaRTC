/*
 * YangCEndian.h
 *
 *  Created on: 2021年12月26日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_SYS_YANGCENDIAN_H_
#define INCLUDE_YANGUTIL_SYS_YANGCENDIAN_H_


#include <stdint.h>


char* yang_put_amf_string(char *c, const char *str);
char* yang_put_amf_double(char *c, double d);
char* yang_put_byte(char *output, uint8_t nVal);
char* yang_put_be16(char *output, uint16_t nVal);
char* yang_put_be24(char *output, uint32_t nVal);
char* yang_put_be32(char *output, uint32_t nVal);
char* yang_put_be64(char *output, uint64_t nVal);

uint32_t yang_get_be32(uint8_t *output);
uint16_t  yang_get_be16(uint8_t *output);


#endif /* INCLUDE_YANGUTIL_SYS_YANGCENDIAN_H_ */
