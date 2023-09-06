//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_SYS_YANGBITS_H_
#define INCLUDE_YANGUTIL_SYS_YANGBITS_H_
#include <stdint.h>
#if Yang_OS_WIN
static __inline uint16_t
uint16_identity (uint16_t __x)
{
  return __x;
}
#define __bswap_constant_16(x)					\
  ((__uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))

static __inline uint32_t
__uint32_identity (uint32_t __x)
{
  return __x;
}

static __inline uint64_t
__uint64_identity (uint64_t __x)
{
  return __x;
}
#define __bswap_constant_64(x)			\
  ((((x) & 0xff00000000000000ull) >> 56)	\
   | (((x) & 0x00ff000000000000ull) >> 40)	\
   | (((x) & 0x0000ff0000000000ull) >> 24)	\
   | (((x) & 0x000000ff00000000ull) >> 8)	\
   | (((x) & 0x00000000ff000000ull) << 8)	\
   | (((x) & 0x0000000000ff0000ull) << 24)	\
   | (((x) & 0x000000000000ff00ull) << 40)	\
   | (((x) & 0x00000000000000ffull) << 56))

 static __inline uint64_t
__bswap_64 (uint64_t __bsx)
{
//#if __GNUC_PREREQ (4, 3)
  return __builtin_bswap64 (__bsx);
//#else
  //return __bswap_constant_64 (__bsx);
//#endif
}
/* Swap bytes in 32-bit value.  */
#define __bswap_constant_32(x)					\
  ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)	\
   | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))

#define __bswap_constant_16(x)					\
  ((__uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
static __inline uint16_t
__bswap_16 (uint16_t __bsx)
{
//#if __GNUC_PREREQ (4, 8)
  return __builtin_bswap16 (__bsx);
//#else
 // return __bswap_constant_16 (__bsx);
//#endif
}
#define __bswap_32(x) ((unsigned int)__builtin_bswap32(x))
#define __bswap_constant_32(x)					\
  ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)	\
   | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))

//static inline uint32_t __bswap_32(uint32_t x)
//{
 //   x = ((x << 8) &0xFF00FF00) | ((x >> 8) &0x00FF00FF);
  //  return (x >> 16) | (x << 16);
//}
#endif
// Convert srs_utime_t as ms.
#define srsu2ms(us) ((us) / YANG_UTIME_MILLISECONDS)
#define srsu2msi(us) int((us) / YANG_UTIME_MILLISECONDS)
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define htobe16(x) __bswap_16 (x)
#  define htole16(x) __uint16_identity (x)
#  define be16toh(x) __bswap_16 (x)
#  define le16toh(x) __uint16_identity (x)

#  define htobe32(x) __bswap_32 (x)
#  define htole32(x) __uint32_identity (x)
#  define be32toh(x) __bswap_32 (x)
#  define le32toh(x) __uint32_identity (x)

#  define htobe64(x) __bswap_64 (x)
#  define htole64(x) __uint64_identity (x)
#  define be64toh(x) __bswap_64 (x)
#  define le64toh(x) __uint64_identity (x)

# else
#  define htobe16(x) __uint16_identity (x)
#  define htole16(x) __bswap_16 (x)
#  define be16toh(x) __uint16_identity (x)
#  define le16toh(x) __bswap_16 (x)

#  define htobe32(x) __uint32_identity (x)
#  define htole32(x) __bswap_32 (x)
#  define be32toh(x) __uint32_identity (x)
#  define le32toh(x) __bswap_32 (x)

#  define htobe64(x) __uint64_identity (x)
#  define htole64(x) __bswap_64 (x)
#  define be64toh(x) __uint64_identity (x)
#  define le64toh(x) __bswap_64 (x)
# endif



#endif /* INCLUDE_YANGUTIL_SYS_YANGBITS_H_ */
