//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGSSL_YANGSSL_H_
#define INCLUDE_YANGSSL_YANGSSL_H_
#include <yangutil/yangtype.h>
#if Yang_Using_Openssl
#include <yangssl/YangOpenssl.h>
#else
#include <yangssl/YangMbedtls.h>
#endif
#endif /* INCLUDE_YANGSSL_YANGSSL_H_ */
