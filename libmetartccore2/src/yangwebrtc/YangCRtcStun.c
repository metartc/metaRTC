/*
 * YangCRtcStun.cpp
 *
 *  Created on: 2022年1月4日
 *      Author: yang
 */

#include <yangwebrtc/YangCRtcStun.h>
#include <yangutil/yangtype.h>
#include <yangutil/buffer/YangBuffer.h>
#include <yangutil/sys/YangLog.h>
int32_t yang_decode_rtcstun(const char* buf, const int32_t nb_buf)
{
    int32_t err = Yang_Ok;

    YangBuffer stream ;
    yang_init_buffer(&stream,(char*)(buf), nb_buf);


    if (yang_buffer_left(&stream) < 20) {
        return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, size=%d", stream.size);
    }

    //m_message_type = yang_read_2bytes(&stream);
    yang_buffer_skip(&stream,2);
    uint16_t message_len = yang_read_2bytes(&stream);
    //string magic_cookie = yang_read_string(&stream,4);
    yang_buffer_skip(&stream,4);
    //m_transcation_id = yang_read_string(&stream,12);
    yang_buffer_skip(&stream,12);
    if (nb_buf != 20 + message_len) {
        return yang_error_wrap(ERROR_RTC_STUN, "ERROR_RTC_STUN invalid stun packet, message_len=%d, nb_buf=%d", message_len, nb_buf);
    }

    while (yang_buffer_left(&stream) >= 4) {
        uint16_t type = yang_read_2bytes(&stream);
        uint16_t len = yang_read_2bytes(&stream);

        if (yang_buffer_left(&stream)  < len) {
            return yang_error_wrap(ERROR_RTC_STUN, "invalid stun packet");
        }

        //string val = yang_read_string(&stream,len);
        yang_buffer_skip(&stream,len);
        // padding
        if (len % 4 != 0) {
            //yang_read_string(&stream,4 - (len % 4));
            yang_buffer_skip(&stream,4 - (len % 4));
        }


    }

    return err;
}
