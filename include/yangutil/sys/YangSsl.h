//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGSSL_H_
#define INCLUDE_YANGUTIL_SYS_YANGSSL_H_

#include <stdint.h>

#include <string.h>

#ifdef __cplusplus
extern "C"{
#include <yangutil/sys/YangCSsl.h>
}
#include <string>
std::string encode_hmac(char* hamc_buf, const int32_t hmac_buf_len);
std::string encode_fingerprint(uint32_t crc32);
#else
#include <yangutil/sys/YangCSsl.h>
#endif


#endif /* INCLUDE_YANGUTIL_SYS_YANGSSL_H_ */
