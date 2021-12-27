/*
 * YangMath.c
 *
 *  Created on: 2021年12月27日
 *      Author: yang
 */

#include <yangutil/sys/YangCMath.h>
#include <yangutil/yangtype.h>

uint64_t yang_random() {
	static bool _random_contexttialized = false;
	if (!_random_contexttialized) {
		_random_contexttialized = true;

        srandom((uint64_t) (yang_update_system_time() | (getpid() << 13)));
	}

	return random();
}
