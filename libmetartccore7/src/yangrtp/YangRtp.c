//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtp.h>
#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/buffer/YangBuffer.h>

#if Yang_OS_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif




/* @see https://tools.ietf.org/html/rfc1889#section-5.1
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                           timestamp                           |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |           synchronization source (SSRC) identifier            |
 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 |            contributing source (CSRC) identifiers             |
 |                             ....                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
uint32_t yang_rtp_fast_parse_ssrc(char* buf, int32_t size)
{
    if (size < 12) {
        return 0;
    }

    uint32_t value = 0;
    char* pp = (char*)&value;

    char* p = buf + 8;
    pp[3] = *p++;
    pp[2] = *p++;
    pp[1] = *p++;
    pp[0] = *p++;
    return value;
}

uint8_t yang_rtp_fast_parse_pt(char* buf, int32_t size)
{
    if (size < 12) {
        return 0;
    }
    return buf[1] & 0x7f;
}

int32_t yang_rtp_fast_parse_twcc(char* buf, int32_t size, uint8_t twcc_id, uint16_t* twcc_sn)
{
    int32_t err = Yang_Ok;

    int32_t need_size = 12 /*rtp head fix len*/ + 4 /* extension header len*/ + 3 /* twcc extension len*/;
    if(size < (need_size)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "required %d bytes, actual %d", need_size, size);
    }

    uint8_t first = buf[0];
    yangbool extension = (first & 0x10);
    uint8_t cc = (first & 0x0F);

    if(!extension) {
        return yang_error_wrap(ERROR_RTC_RTP, "no extension in rtp");
    }

    need_size += cc * 4; // csrc size
    if(size < (need_size)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "required %d bytes, actual %d", need_size, size);
    }
    buf += 12 + 4*cc;

    uint16_t value = *((uint16_t*)buf);
    value = ntohs(value);
    if(0xBEDE != value) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "no support this type(0x%02x) extension", value);
    }
    buf += 2;

    uint16_t extension_length = ntohs(*((uint16_t*)buf));
    buf += 2;
    extension_length *= 4;
    need_size += extension_length; // entension size
    if(size < (need_size)) {
        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "required %d bytes, actual %d", need_size, size);
    }

    while(extension_length > 0) {
        uint8_t v = buf[0];
        buf++;
        extension_length--;
        if(0 == v) {
            continue;
        }

        uint8_t id = (v & 0xF0) >>4;
        uint8_t len = (v & 0x0F) + 1;

        if(id == twcc_id) {
            *twcc_sn = ntohs(*((uint16_t*)buf));
            return err;
        } else {
            buf += len;
            extension_length -= len;
        }
    }


    return err;
}

// If value is newer than pre_value，return yangtrue; otherwise false
yangbool yang_seq_is_newer(uint16_t value, uint16_t pre_value)
{
    return yang_rtp_seq_distance(pre_value, value) > 0;
}

yangbool yang_seq_is_rollback(uint16_t value, uint16_t pre_value)
{
    if(yang_seq_is_newer(value, pre_value)) {
        return pre_value > value;
    }
    return yangfalse;
}

// If value is newer then pre_value, return positive, otherwise negative.
int32_t yang_seq_distance(uint16_t value, uint16_t pre_value)
{
    return yang_rtp_seq_distance(pre_value, value);
}


uint64_t yang_rtpExtensionOneByte_nb(YangRtpExtensionOneByte* onebyte){
	return 2;
}

int32_t yang_encode_rtpExtensionsOneByte(YangRtpExtensionOneByte* one,YangBuffer* buf){
	 int32_t err = Yang_Ok;

	    if (!yang_buffer_require(buf,2)) {
	        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
	    }

	    uint8_t id_len = (one->id & 0x0F)<< 4 | 0x00;
	    yang_write_1bytes(buf,id_len);
	    yang_write_1bytes(buf,one->value);

	    return err;
}

int32_t yang_encode_rtpExtensionsTwcc(YangRtpExtensionTwcc* twcc,YangBuffer* buf){
	int32_t err = Yang_Ok;

	if(!yang_buffer_require(buf,3)) {
		return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 3);
	}

	uint8_t id_len = (twcc->id & 0x0F)<< 4| 0x01;
	yang_write_1bytes(buf,id_len);
	yang_write_2bytes(buf,twcc->sn);

	return err;
}

int32_t yang_encode_rtpExtensions(YangRtpExtensions* ext,YangBuffer* buf){
	 int32_t err = Yang_Ok;

	    yang_write_2bytes(buf,0xBEDE);

	    // Write length.
	    int32_t len = 0;

	    if (ext->twcc.has_twcc) {
	        len += yang_rtpExtensionTwcc_nb(&ext->twcc);
	    }

	    if (ext->audio_level.has_ext) {
	        len += yang_rtpExtensionOneByte_nb(&ext->audio_level);
	    }

	    int32_t padding_count = (len % 4 == 0) ? 0 : (4 - len % 4);
	    len += padding_count;

	    if (!yang_buffer_require(buf,len)) {
	        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", len);
	    }

	    yang_write_2bytes(buf,len / 4);

	    // Write extensions.
	    if (ext->twcc.has_twcc) {
	        if ((err = yang_encode_rtpExtensionsTwcc(&ext->twcc,buf)) != Yang_Ok) {
	            return yang_error_wrap(err, "encode twcc extension");
	        }
	    }

	    if (ext->audio_level.has_ext) {
	        if (Yang_Ok != (err = yang_encode_rtpExtensionsOneByte(&ext->audio_level,buf))) {
	            return yang_error_wrap(err, "encode audio level extension");
	        }
	    }

	    // add padding
	    if (padding_count) {
	        yang_memset(buf->head, 0, padding_count);
	       yang_buffer_skip(buf,padding_count);
	    }

	    return err;
}

uint64_t yang_rtpExtensionTwcc_nb(YangRtpExtensionTwcc* twcc){
	return 3;
}

YangRtpExtensionType yang_get_rtpExtensionType (YangRtpExtensionTypes* types,int32_t id)
{
    for (int32_t type = kRtpExtensionNone + 1; type < kRtpExtensionNumberOfExtensions; ++type) {
        if (types->ids[type] == id) {
            return (YangRtpExtensionType)(type);
        }
    }
    return kRtpExtensionNone;//YangRtpExtensionTypes_kInvalidType;
}
int32_t yang_decode_0xbede(YangRtpExtensions* ext,YangBuffer* buf)
{
    int32_t err = Yang_Ok;

    while (!yang_buffer_empty(buf)) {
        // The first byte maybe padding or id+len.
        if (!yang_buffer_require(buf,1)) {
            return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
        }
        uint8_t v = *((uint8_t*)buf->head);

        // Padding, ignore
        if(v == 0) {
            yang_buffer_skip(buf,1);
            continue;
        }

        //  0
        //  0 1 2 3 4 5 6 7
        // +-+-+-+-+-+-+-+-+
        // |  ID   |  len  |
        // +-+-+-+-+-+-+-+-+
        // Note that 'len' is the header extension element length, which is the
        // number of bytes - 1.
        uint8_t id = (v & 0xF0) >> 4;
        uint8_t len = (v & 0x0F) + 1;

        YangRtpExtensionType xtype = ext->types? yang_get_rtpExtensionType(ext->types,id) : kRtpExtensionNone;
        if (xtype == kRtpExtensionTransportSequenceNumber) {
            if (ext->decode_twcc_extension) {
                if ((err = yang_decode_rtpExtensionsTwcc(&ext->twcc,buf)) != Yang_Ok) {
                    return yang_error_wrap(err, "decode twcc extension");
                }
                ext->has_ext = yangtrue;
            } else {
                if (!yang_buffer_require(buf,len+1)) {
                    return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", len+1);
                }
                yang_buffer_skip(buf,len + 1);
            }
        } else if (xtype == kRtpExtensionAudioLevel) {
            if((err = yang_decode_rtpExtensionsOneByte(&ext->audio_level,buf)) != Yang_Ok) {
                return yang_error_wrap(err, "decode audio level extension");
            }
            ext->has_ext = yangtrue;
        } else {
            yang_buffer_skip(buf,1 + len);
        }
    }

    return err;
}

int32_t yang_decode_rtpExtensions(YangRtpExtensions* ext,YangBuffer* buf){
	int32_t err = Yang_Ok;

	    /* @see https://tools.ietf.org/html/rfc3550#section-5.3.1
	        0                   1                   2                   3
	        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	        |      defined by profile       |           length              |
	        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	        |                        header extension                       |
	        |                             ....                              |
	    */
	  if (!yang_buffer_require(buf,4)) {
	        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires 4 bytes");
	    }
	    uint16_t profile_id = yang_read_2bytes(buf);
	    uint16_t extension_length = yang_read_2bytes(buf);

	    // @see: https://tools.ietf.org/html/rfc5285#section-4.2
	 if (profile_id == 0xBEDE) {
	        YangBuffer xbuf;
	        yang_init_buffer(&xbuf,buf->head, extension_length * 4);
	        yang_buffer_skip(buf,extension_length * 4);
	        return yang_decode_0xbede(ext,&xbuf);
	    }  else if (profile_id == 0x1000) {
	        yang_buffer_skip(buf,extension_length * 4);
	    } else {
	        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "fail to parse extension");
	    }
	    return err;
}

int32_t yang_decode_rtpExtensionsTwcc(YangRtpExtensionTwcc* twcc,YangBuffer* buf){
	 int32_t err = Yang_Ok;

	    //   0                   1                   2
	    //   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
	    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    //  |  ID   | L=1   |transport wide sequence number |
	    //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	    if (!yang_buffer_require(buf,1)) {
	        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 1);
	    }
	    uint8_t v = yang_read_1bytes(buf);

	    twcc->id = (v & 0xF0) >> 4;
	    uint8_t len = (v & 0x0F);
	    if(!twcc->id || len != 1) {
	        return yang_error_wrap(ERROR_RTC_RTP, "invalid twcc id=%d, len=%d", twcc->id, len);
	    }

	    if (!yang_buffer_require(buf,2)) {
	        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
	    }
	    twcc->sn = yang_read_2bytes(buf);

	    twcc->has_twcc = yangtrue;
	    return err;
}

int32_t yang_decode_rtpExtensionsOneByte(YangRtpExtensionOneByte* one,YangBuffer* buf){
	int32_t err = Yang_Ok;

	    if (!yang_buffer_require(buf,2)) {
	        return yang_error_wrap(ERROR_RTC_RTP_MUXER, "requires %d bytes", 2);
	    }
	    uint8_t v = yang_read_1bytes(buf);

	    one->id = (v & 0xF0) >> 4;
	    uint8_t len = (v & 0x0F);
	    if(!one->id || len != 0) {
	        return yang_error_wrap(ERROR_RTC_RTP, "invalid rtp extension id=%d, len=%d", one->id, len);
	    }

	    one->value = yang_read_1bytes(buf);

	    one->has_ext = yangtrue;
	    return err;
}


