//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangEndian.h>


char * yang_put_amf_string( char *c, const char *str )
{
	unsigned short len = yang_strlen( str );
    c=yang_put_be16( c, len );
    yang_memcpy(c,str,len);
    return c+len;
}
char * yang_put_amf_double( char *c, double d )
{
    *c++ = 0;  /* type: Number */
    {
        uint8_t *ci, *co;
        ci = (uint8_t *)&d;
        co = (uint8_t *)c;
        co[0] = ci[7];
        co[1] = ci[6];
        co[2] = ci[5];
        co[3] = ci[4];
        co[4] = ci[3];
        co[5] = ci[2];
        co[6] = ci[1];
        co[7] = ci[0];
    }
    return c+8;
}
char * yang_put_byte( char *output, uint8_t nVal )
{
    output[0] = nVal;
    return output+1;
}
char * yang_put_be16(char *output, uint16_t  nVal )
{
    output[1] = nVal & 0xff;
    output[0] = nVal >> 8;
    return output+2;
}
char * yang_put_be24(char *output,uint32_t   nVal )
{
    output[2] = nVal & 0xff;
    output[1] = nVal >> 8;
    output[0] = nVal >> 16;
    return output+3;
}
char * yang_put_be32(char *output, uint32_t  nVal )
{
    output[3] = nVal & 0xff;
    output[2] = nVal >> 8;
    output[1] = nVal >> 16;
    output[0] = nVal >> 24;
    return output+4;
}
char *  yang_put_be64( char *output, uint64_t nVal )
{
    output=yang_put_be32( output, nVal >> 32 );
    output=yang_put_be32( output, nVal );
    return output;
}

uint32_t  yang_get_be32(uint8_t *output) {
		return output[3]|output[2]<<8|output[1]<<16|output[0]<<24;
}

uint16_t  yang_get_be16(uint8_t *output) {
		return output[1]|output[0]<<8;
}
