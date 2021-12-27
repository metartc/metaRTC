/*
 * YangCTime.h
 *
 *  Created on: 2021年12月27日
 *      Author: yang
 */

#ifndef INCLUDE_YANGUTIL_SYS_YANGCTIME_H_
#define INCLUDE_YANGUTIL_SYS_YANGCTIME_H_
#include <stdint.h>
int64_t yang_get_system_micro_time();
int64_t yang_get_system_micro_time();

int64_t yang_get_milli_time();//haomiao
int64_t yang_get_micro_time();//weimiao

#define yang_get_milli_tick  yang_get_milli_time
#define yang_get_micro_tick  yang_get_micro_time




int64_t yang_get_nano_tick();//namiao



#endif /* INCLUDE_YANGUTIL_SYS_YANGCTIME_H_ */
