/*
 * YangGetBits.h
 *
 *  Created on: 2019年8月17日
 *      Author: yang
 */

#ifndef YANGUTIL_VIDEO_YANGGETBITS_H_
#define YANGUTIL_VIDEO_YANGGETBITS_H_

#include <yangavutil/video/YangBittype.h>
#include "stdint.h"
#include "limits.h"


class YangGetBits {
public:
	YangGetBits();
	virtual ~YangGetBits();
};

//#define BITSTREAM_READER_LE
#ifndef UNCHECKED_BITSTREAM_READER
#define UNCHECKED_BITSTREAM_READER !CONFIG_YANG_BITSTREAM_READER
#endif

#ifndef CACHED_BITSTREAM_READER
#define CACHED_BITSTREAM_READER 0
#endif



#define yang_MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define YangERRTAG(a, b, c, d) (-(int)yang_MKTAG(a, b, c, d))
#define YangERROR_INVALIDDATA        YangERRTAG( 'I','N','D','A')
#define Yang_INPUT_BUFFER_PADDING_SIZE 64
#define YangMAX(a,b) ((a) > (b) ? (a) : (b))
#define Yang_TYPE int16_t
inline unsigned yang_zero_extend(unsigned val, unsigned bits) {
	return (val << ((8 * sizeof(int)) - bits)) >> ((8 * sizeof(int)) - bits);
}
inline int32_t yang_sign_extend(int32_t val, unsigned bits) {
	unsigned shift = 8 * sizeof(int) - bits;
	union {
		unsigned u;
		int32_t s;
	} v = { (unsigned) val << shift };
	return v.s >> shift;
}

inline  uint32_t  yang_get_bits(YangGetBitContext *s, int32_t n);
inline  void yang_skip_bits(YangGetBitContext *s, int32_t n);
inline  uint32_t  yang_show_bits(YangGetBitContext *s, int32_t n);

#define Yang_RL32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[3] << 24) |    \
               (((const uint8_t*)(x))[2] << 16) |    \
               (((const uint8_t*)(x))[1] <<  8) |    \
                ((const uint8_t*)(x))[0])
#   define Yang_RB32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[0] << 24) |    \
               (((const uint8_t*)(x))[1] << 16) |    \
               (((const uint8_t*)(x))[2] <<  8) |    \
                ((const uint8_t*)(x))[3])

#   define Yang_NEG_SSR32(a,s) ((( int32_t)(a))>>(32-(s)))
#   define Yang_NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))

#if CACHED_BITSTREAM_READER
#   define Yang_MIN_CACHE_BITS 64
#elif defined LONG_BITSTREAM_READER
#   define Yang_MIN_CACHE_BITS 32
#else
#   define Yang_MIN_CACHE_BITS 25
#endif

#if !CACHED_BITSTREAM_READER

	#define Yang_OPEN_READER_NOSIZE(name, gb)            \
		uint32_t  name ## _index = (gb)->index;  \
		uint32_t   name ## _cache;

	#if UNCHECKED_BITSTREAM_READER
		#define Yang_OPEN_READER(name, gb) Yang_OPEN_READER_NOSIZE(name, gb)

		#define Yang_BITS_AVAILABLE(name, gb) 1
	#else
		#define Yang_OPEN_READER(name, gb)                   \
			Yang_OPEN_READER_NOSIZE(name, gb);               \
			uint32_t  name ## _size_plus8 = (gb)->size_in_bits_plus8

		#define Yang_BITS_AVAILABLE(name, gb) name ## _index < name ## _size_plus8
	#endif

	#define Yang_CLOSE_READER(name, gb) (gb)->index = name ## _index

	# ifdef LONG_BITSTREAM_READER

	# define Yang_UPDATE_CACHE_LE(name, gb) name ## _cache = \
		  Yang_RL64((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

	# define Yang_UPDATE_CACHE_BE(name, gb) name ## _cache = \
		  Yang_RB64((gb)->buffer + (name ## _index >> 3)) >> (32 - (name ## _index & 7))

	#else

	# define Yang_UPDATE_CACHE_LE(name, gb) name ## _cache = \
		  Yang_RL32((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

	# define Yang_UPDATE_CACHE_BE(name, gb) name ## _cache = \
		  Yang_RB32((gb)->buffer + (name ## _index >> 3)) << (name ## _index & 7)

	#endif

	#ifdef BITSTREAM_READER_LE

	# define Yang_UPDATE_CACHE(name, gb) Yang_UPDATE_CACHE_LE(name, gb)

	# define Yang_Yang_SKIP_BITS(name, gb, num) name ## _cache >>= (num)

	#else

	# define Yang_UPDATE_CACHE(name, gb) Yang_UPDATE_CACHE_BE(name, gb)

	# define Yang_Yang_SKIP_BITS(name, gb, num) name ## _cache <<= (num)

	#endif

	#if UNCHECKED_BITSTREAM_READER
	#   define Yang_SKIP_COUNTER(name, gb, num) name ## _index += (num)
	#else
	#   define Yang_SKIP_COUNTER(name, gb, num) \
		name ## _index = FFMIN(name ## _size_plus8, name ## _index + (num))
	#endif

	#define Yang_BITS_LEFT(name, gb) ((int)((gb)->size_in_bits - name ## _index))

	#define Yang_SKIP_BITS(name, gb, num)                \
		do {                                        \
			Yang_Yang_SKIP_BITS(name, gb, num);              \
			Yang_SKIP_COUNTER(name, gb, num);            \
		} while (0)

	#define LAST_Yang_SKIP_BITS(name, gb, num) Yang_SKIP_COUNTER(name, gb, num)

	#define Yang_SHOW_UBITS_LE(name, gb, num) yang_zero_extend(name ## _cache, num)
	#define Yang_SHOW_SBITS_LE(name, gb, num) yang_sign_extend(name ## _cache, num)

	#define Yang_SHOW_UBITS_BE(name, gb, num) Yang_NEG_USR32(name ## _cache, num)
	#define Yang_SHOW_SBITS_BE(name, gb, num) Yang_NEG_SSR32(name ## _cache, num)

	#ifdef BITSTREAM_READER_LE
	#   define Yang_SHOW_UBITS(name, gb, num) Yang_SHOW_UBITS_LE(name, gb, num)
	#   define Yang_SHOW_SBITS(name, gb, num) Yang_SHOW_SBITS_LE(name, gb, num)
	#else
	#   define Yang_SHOW_UBITS(name, gb, num) Yang_SHOW_UBITS_BE(name, gb, num)
	#   define Yang_SHOW_SBITS(name, gb, num) Yang_SHOW_SBITS_BE(name, gb, num)
	#endif

	#define Yang_GET_CACHE(name, gb) ((uint32_t) name ## _cache)

#endif

static  int32_t yang_get_bits_count(YangGetBitContext *s) {
#if CACHED_BITSTREAM_READER
	return s->index - s->bits_left;
#else
	return s->index;
#endif
}

#if CACHED_BITSTREAM_READER
static  void yang_refill_32(YangGetBitContext *s)
{
#if !UNCHECKED_BITSTREAM_READER
	if (s->index >> 3 >= s->buffer_end - s->buffer)
	return;
#endif

#ifdef BITSTREAM_READER_LE
	s->cache = (uint64_t)Yang_RL32(s->buffer + (s->index >> 3)) << s->bits_left | s->cache;
#else
	s->cache = s->cache | (uint64_t)Yang_RB32(s->buffer + (s->index >> 3)) << (32 - s->bits_left);
#endif
	s->index += 32;
	s->bits_left += 32;
}

static  void yang_refill_64(YangGetBitContext *s)
{
#if !UNCHECKED_BITSTREAM_READER
	if (s->index >> 3 >= s->buffer_end - s->buffer)
	return;
#endif

#ifdef BITSTREAM_READER_LE
	s->cache = Yang_RL64(s->buffer + (s->index >> 3));
#else
	s->cache = Yang_RB64(s->buffer + (s->index >> 3));
#endif
	s->index += 64;
	s->bits_left = 64;
}

static  uint64_t yang_get_val(YangGetBitContext *s, unsigned n, int32_t is_le)
{
	uint64_t ret;
	av_assert2(n>0 && n<=63);
	if (is_le) {
		ret = s->cache & ((UINT64_C(1) << n) - 1);
		s->cache >>= n;
	} else {
		ret = s->cache >> (64 - n);
		s->cache <<= n;
	}
	s->bits_left -= n;
	return ret;
}

static  unsigned yang_show_val(const YangGetBitContext *s, unsigned n)
{
#ifdef BITSTREAM_READER_LE
	return s->cache & ((UINT64_C(1) << n) - 1);
#else
	return s->cache >> (64 - n);
#endif
}
#endif

/**
 * Skips the specified number of bits.
 * @param n the number of bits to skip,
 *          For the UNCHECKED_BITSTREAM_READER this must not cause the distance
 *          from the start to overflow int32_t. Staying within the bitstream + padding
 *          is sufficient, too.
 */
static  void yang_skip_bits_long(YangGetBitContext *s, int32_t n) {
#if CACHED_BITSTREAM_READER
	yang_skip_bits(s, n);
#else
#if UNCHECKED_BITSTREAM_READER
	s->index += n;
#else
	s->index += av_clip(n, -s->index, s->size_in_bits_plus8 - s->index);
#endif
#endif
}

#if CACHED_BITSTREAM_READER
static  void yang_skip_remaining(YangGetBitContext *s, unsigned n)
{
#ifdef BITSTREAM_READER_LE
	s->cache >>= n;
#else
	s->cache <<= n;
#endif
	s->bits_left -= n;
}
#endif

/**
 * Read MPEG-1 dc-style VLC (sign bit + mantissa with no MSB).
 * if MSB not set it is negative
 * @param n length in bits
 */
static  int32_t yang_get_xbits(YangGetBitContext *s, int32_t n) {
#if CACHED_BITSTREAM_READER
	int32_t cache = yang_show_bits(s, 32);
	int32_t sign = ~cache >> 31;
	yang_skip_remaining(s, n);

	return ((((uint32_t)(sign ^ cache)) >> (32 - n)) ^ sign) - sign;
#else
	register int32_t sign;
	register int32_t cache;
	Yang_OPEN_READER(re, s);
	// av_assert2(n>0 && n<=25);
	Yang_UPDATE_CACHE(re, s);
	cache = Yang_GET_CACHE(re, s);
	sign = ~cache >> 31;
	LAST_Yang_SKIP_BITS(re, s, n);
	Yang_CLOSE_READER(re, s);
	return (Yang_NEG_USR32(sign ^ cache, n) ^ sign) - sign;
#endif
}

#if !CACHED_BITSTREAM_READER
static  int32_t yang_get_xbits_le(YangGetBitContext *s, int32_t n) {
	register int32_t sign;
	register int32_t cache;
	Yang_OPEN_READER(re, s);
	// av_assert2(n>0 && n<=25);
	Yang_UPDATE_CACHE_LE(re, s);
	cache = Yang_GET_CACHE(re, s);
	sign = yang_sign_extend(~cache, n) >> 31;
	LAST_Yang_SKIP_BITS(re, s, n);
	Yang_CLOSE_READER(re, s);
	return (yang_zero_extend(sign ^ cache, n) ^ sign) - sign;
}
#endif

static  int32_t yang_get_sbits(YangGetBitContext *s, int32_t n) {
	register int32_t tmp;
#if CACHED_BITSTREAM_READER
	av_assert2(n>0 && n<=25);
	tmp = sign_extend(yang_get_bits(s, n), n);
#else
	Yang_OPEN_READER(re, s);
	//av_assert2(n>0 && n<=25);
	Yang_UPDATE_CACHE(re, s);
	tmp = Yang_SHOW_SBITS(re, s, n);
	LAST_Yang_SKIP_BITS(re, s, n);
	Yang_CLOSE_READER(re, s);
#endif
	return tmp;
}

/**
 * Read 1-25 bits.
 */
inline  uint32_t  yang_get_bits(YangGetBitContext *s, int32_t n) {
	register int32_t tmp;
#if CACHED_BITSTREAM_READER

	av_assert2(n>0 && n<=32);
	if (n > s->bits_left) {
		yang_refill_32(s);
		if (s->bits_left < 32)
		s->bits_left = n;
	}

#ifdef BITSTREAM_READER_LE
	tmp = yang_get_val(s, n, 1);
#else
	tmp = yang_get_val(s, n, 0);
#endif
#else
	Yang_OPEN_READER(re, s);
	// av_assert2(n>0 && n<=25);
	Yang_UPDATE_CACHE(re, s);
	tmp = Yang_SHOW_UBITS(re, s, n);
	LAST_Yang_SKIP_BITS(re, s, n);
	Yang_CLOSE_READER(re, s);
#endif
	return tmp;
}

/**
 * Read 0-25 bits.
 */
inline int32_t yang_get_bitsz(YangGetBitContext *s, int32_t n) {
	return n ? yang_get_bits(s, n) : 0;
}

inline  uint32_t  yang_get_bits_le(YangGetBitContext *s, int32_t n) {
#if CACHED_BITSTREAM_READER
	av_assert2(n>0 && n<=32);
	if (n > s->bits_left) {
		yang_refill_32(s);
		if (s->bits_left < 32)
		s->bits_left = n;
	}

	return yang_get_val(s, n, 1);
#else
	register int32_t tmp;
	Yang_OPEN_READER(re, s);
	// av_assert2(n>0 && n<=25);
	Yang_UPDATE_CACHE_LE(re, s);
	tmp = Yang_SHOW_UBITS_LE(re, s, n);
	LAST_Yang_SKIP_BITS(re, s, n);
	Yang_CLOSE_READER(re, s);
	return tmp;
#endif
}

/**
 * Show 1-25 bits.
 */
inline  uint32_t  yang_show_bits(YangGetBitContext *s, int32_t n) {
	register int32_t tmp;
#if CACHED_BITSTREAM_READER
	if (n > s->bits_left)
	yang_refill_32(s);

	tmp = yang_show_val(s, n);
#else
	Yang_OPEN_READER_NOSIZE(re, s);
	//  av_assert2(n>0 && n<=25);
	Yang_UPDATE_CACHE(re, s);
	tmp = Yang_SHOW_UBITS(re, s, n);
#endif
	return tmp;
}

inline  void yang_skip_bits(YangGetBitContext *s, int32_t n) {
#if CACHED_BITSTREAM_READER
	if (n < s->bits_left)
	yang_skip_remaining(s, n);
	else {
		n -= s->bits_left;
		s->cache = 0;
		s->bits_left = 0;

		if (n >= 64) {
			unsigned skip = (n / 8) * 8;

			n -= skip;
			s->index += skip;
		}
		yang_refill_64(s);
		if (n)
		yang_skip_remaining(s, n);
	}
#else
	Yang_OPEN_READER(re, s);
	LAST_Yang_SKIP_BITS(re, s, n);
	Yang_CLOSE_READER(re, s);
#endif
}

inline  uint32_t  yang_get_bits1(YangGetBitContext *s) {
#if CACHED_BITSTREAM_READER
	if (!s->bits_left)
	yang_refill_64(s);

#ifdef BITSTREAM_READER_LE
	return yang_get_val(s, 1, 1);
#else
	return yang_get_val(s, 1, 0);
#endif
#else
	uint32_t  index = s->index;
	uint8_t result = s->buffer[index >> 3];
#ifdef BITSTREAM_READER_LE
	result >>= index & 7;
	result &= 1;
#else
	result <<= index & 7;
	result >>= 8 - 1;
#endif
#if !UNCHECKED_BITSTREAM_READER
	if (s->index < s->size_in_bits_plus8)
#endif
	index++;
	s->index = index;

	return result;
#endif
}

inline  uint32_t  yang_show_bits1(YangGetBitContext *s) {
	return yang_show_bits(s, 1);
}

inline  void yang_skip_bits1(YangGetBitContext *s) {
	yang_skip_bits(s, 1);
}

/**
 * Read 0-32 bits.
 */
inline  uint32_t  yang_get_bits_long(YangGetBitContext *s, int32_t n) {
	//av_assert2(n>=0 && n<=32);
	if (!n) {
		return 0;
#if CACHED_BITSTREAM_READER
	}
	return yang_get_bits(s, n);
#else
	} else if (n <= Yang_MIN_CACHE_BITS) {
		return yang_get_bits(s, n);
	} else {
#ifdef BITSTREAM_READER_LE
		unsigned ret = yang_get_bits(s, 16);
		return ret | (yang_get_bits(s, n - 16) << 16);
#else
		unsigned ret = yang_get_bits(s, 16) << (n - 16);
		return ret | yang_get_bits(s, n - 16);
#endif
	}
#endif
}

/**
 * Read 0-64 bits.
 */
inline  uint64_t yang_get_bits64(YangGetBitContext *s, int32_t n) {
	if (n <= 32) {
		return yang_get_bits_long(s, n);
	} else {
#ifdef BITSTREAM_READER_LE
		uint64_t ret = yang_get_bits_long(s, 32);
		return ret | (uint64_t) yang_get_bits_long(s, n - 32) << 32;
#else
		uint64_t ret = (uint64_t) yang_get_bits_long(s, n - 32) << 32;
		return ret | yang_get_bits_long(s, 32);
#endif
	}
}

/**
 * Read 0-32 bits as a signed integer.
 */
inline  int32_t yang__get_sbits_long(YangGetBitContext *s, int32_t n) {
	// sign_extend(x, 0) is undefined
	if (!n)
		return 0;

	return yang_sign_extend(yang_get_bits_long(s, n), n);
}

/**
 * Show 0-32 bits.
 */
inline  uint32_t  yang_show_bits_long(YangGetBitContext *s, int32_t n) {
	if (n <= Yang_MIN_CACHE_BITS) {
		return yang_show_bits(s, n);
	} else {
		YangGetBitContext gb = *s;
		return yang_get_bits_long(&gb, n);
	}
}

inline  int32_t yang_check_marker(void *logctx, YangGetBitContext *s,
		const char *msg) {
	int32_t bit = yang_get_bits1(s);
	// if (!bit)
	//   av_log(logctx, AV_LOG_INFO, "Marker bit missing at %d of %d %s\n", yang_get_bits_count(s) - 1, s->size_in_bits, msg);

	return bit;
}

/**
 * Initialize YangGetBitContext.
 * @param buffer bitstream buffer, must be AV_INPUT_BUFFER_PADDING_SIZE bytes
 *        larger than the actual read bits because some optimized bitstream
 *        readers read 32 or 64 bit at once and could read over the end
 * @param bit_size the size of the buffer in bits
 * @return 0 on success, AVERROR_INVALIDDATA if the buffer_size would overflow.
 */
inline  int32_t yang_init_get_bits(YangGetBitContext *s,  uint8_t *buffer,
		int32_t bit_size) {
	int32_t buffer_size;
	int32_t ret = 0;

	if (bit_size >= INT_MAX - YangMAX(7, Yang_INPUT_BUFFER_PADDING_SIZE*8)
			|| bit_size < 0 || !buffer) {
		bit_size = 0;
		buffer = 0;
		ret = YangERROR_INVALIDDATA;
	}

	buffer_size = (bit_size + 7) >> 3;

	s->buffer = buffer;
	s->size_in_bits = bit_size;
	s->size_in_bits_plus8 = bit_size + 8;
	s->buffer_end = buffer + buffer_size;
	s->index = 0;

#if CACHED_BITSTREAM_READER
	yang_refill_64(s);
#endif

	return ret;
}

/**
 * Initialize YangGetBitContext.
 * @param buffer bitstream buffer, must be AV_INPUT_BUFFER_PADDING_SIZE bytes
 *        larger than the actual read bits because some optimized bitstream
 *        readers read 32 or 64 bit at once and could read over the end
 * @param byte_size the size of the buffer in bytes
 * @return 0 on success, AVERROR_INVALIDDATA if the buffer_size would overflow.
 */
inline  int32_t yang_init_get_bits8(YangGetBitContext *s,  uint8_t *buffer,
		int32_t byte_size) {
	if (byte_size > INT_MAX / 8 || byte_size < 0)
		byte_size = -1;
	return yang_init_get_bits(s, buffer, byte_size * 8);
}

static   uint8_t *align_yang_get_bits(YangGetBitContext *s) {
	int32_t n = -yang_get_bits_count(s) & 7;
	if (n)
		yang_skip_bits(s, n);
	return s->buffer + (s->index >> 3);
}

/**
 * If the vlc code is invalid and max_depth=1, then no bits will be removed.
 * If the vlc code is invalid and max_depth>1, then the number of bits removed
 * is undefined.
 */
#define Yang_GET_VLC(code, name, gb, table, bits, max_depth)         \
    do {                                                        \
        int32_t n, nb_bits;                                         \
        uint32_t  index;                                     \
                                                                \
        index = Yang_SHOW_UBITS(name, gb, bits);                     \
        code  = table[index][0];                                \
        n     = table[index][1];                                \
                                                                \
        if (max_depth > 1 && n < 0) {                           \
            LAST_Yang_SKIP_BITS(name, gb, bits);                     \
            Yang_UPDATE_CACHE(name, gb);                             \
                                                                \
            nb_bits = -n;                                       \
                                                                \
            index = Yang_SHOW_UBITS(name, gb, nb_bits) + code;       \
            code  = table[index][0];                            \
            n     = table[index][1];                            \
            if (max_depth > 2 && n < 0) {                       \
                LAST_Yang_SKIP_BITS(name, gb, nb_bits);              \
                Yang_UPDATE_CACHE(name, gb);                         \
                                                                \
                nb_bits = -n;                                   \
                                                                \
                index = Yang_SHOW_UBITS(name, gb, nb_bits) + code;   \
                code  = table[index][0];                        \
                n     = table[index][1];                        \
            }                                                   \
        }                                                       \
        Yang_SKIP_BITS(name, gb, n);                                 \
    } while (0)

#define Yang_GET_RL_VLC(level, run, name, gb, table, bits,  \
                   max_depth, need_update)                      \
    do {                                                        \
        int32_t n, nb_bits;                                         \
        uint32_t  index;                                     \
                                                                \
        index = Yang_SHOW_UBITS(name, gb, bits);                     \
        level = table[index].level;                             \
        n     = table[index].len;                               \
                                                                \
        if (max_depth > 1 && n < 0) {                           \
            Yang_SKIP_BITS(name, gb, bits);                          \
            if (need_update) {                                  \
                Yang_UPDATE_CACHE(name, gb);                         \
            }                                                   \
                                                                \
            nb_bits = -n;                                       \
                                                                \
            index = Yang_SHOW_UBITS(name, gb, nb_bits) + level;      \
            level = table[index].level;                         \
            n     = table[index].len;                           \
            if (max_depth > 2 && n < 0) {                       \
                LAST_Yang_SKIP_BITS(name, gb, nb_bits);              \
                if (need_update) {                              \
                    Yang_UPDATE_CACHE(name, gb);                     \
                }                                               \
                nb_bits = -n;                                   \
                                                                \
                index = Yang_SHOW_UBITS(name, gb, nb_bits) + level;  \
                level = table[index].level;                     \
                n     = table[index].len;                       \
            }                                                   \
        }                                                       \
        run = table[index].run;                                 \
        Yang_SKIP_BITS(name, gb, n);                                 \
    } while (0)

/* Return the LUT element for the given bitstream configuration. */
inline  int32_t yang_set_idx(YangGetBitContext *s, int32_t code, int32_t *n, int32_t *nb_bits,
Yang_TYPE (*table)[2]) {
	unsigned idx;

	*nb_bits = -*n;
	idx = yang_show_bits(s, *nb_bits) + code;
	*n = table[idx][1];

	return table[idx][0];
}

/**
 * Parse a vlc code.
 * @param bits is the number of bits which will be read at once, must be
 *             identical to nb_bits in init_vlc()
 * @param max_depth is the number of times bits bits must be read to completely
 *                  read the longest vlc code
 *                  = (max_vlc_length + bits - 1) / bits
 * @returns the code parsed or -1 if no vlc matches
 */
inline int32_t Yang_GET_VLC2(YangGetBitContext *s, Yang_TYPE (*table)[2], int32_t bits,
		int32_t max_depth) {
#if CACHED_BITSTREAM_READER
	int32_t nb_bits;
	unsigned idx = yang_show_bits(s, bits);
	int32_t code = table[idx][0];
	int32_t n = table[idx][1];

	if (max_depth > 1 && n < 0) {
		yang_skip_remaining(s, bits);
		code = yang_set_idx(s, code, &n, &nb_bits, table);
		if (max_depth > 2 && n < 0) {
			yang_skip_remaining(s, nb_bits);
			code = yang_set_idx(s, code, &n, &nb_bits, table);
		}
	}
	yang_skip_remaining(s, n);

	return code;
#else
	int32_t code;

	Yang_OPEN_READER(re, s);
	Yang_UPDATE_CACHE(re, s);

	Yang_GET_VLC(code, re, s, table, bits, max_depth);

	Yang_CLOSE_READER(re, s);

	return code;
#endif
}

inline  int32_t yang_decode012(YangGetBitContext *gb) {
	int32_t n;
	n = yang_get_bits1(gb);
	if (n == 0)
		return 0;
	else
		return yang_get_bits1(gb) + 1;
}

inline  int32_t yang_decode210(YangGetBitContext *gb) {
	if (yang_get_bits1(gb))
		return 0;
	else
		return 2 - yang_get_bits1(gb);
}

inline  int32_t yang_get_bits_left(YangGetBitContext *gb) {
	return gb->size_in_bits - yang_get_bits_count(gb);
}

inline  int32_t yang_skip_1stop_8data_bits(YangGetBitContext *gb) {
	if (yang_get_bits_left(gb) <= 0)
		return YangERROR_INVALIDDATA;

	while (yang_get_bits1(gb)) {
		yang_skip_bits(gb, 8);
		if (yang_get_bits_left(gb) <= 0)
			return YangERROR_INVALIDDATA;
	}

	return 0;
}


#endif /* YANGUTIL_VIDEO_YANGGETBITS_H_ */
