/*
 * YangCSsrc.h
 *
 *  Created on: 2021年12月27日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_SYS_YANGCSSRC_H_
#define INCLUDE_YANGUTIL_SYS_YANGCSSRC_H_


#include <stdint.h>


uint32_t yang_generate_ssrc();
uint32_t yang_crc32_ieee(const void* buf, int32_t size, uint32_t previous);



#endif /* INCLUDE_YANGUTIL_SYS_YANGCSSRC_H_ */
