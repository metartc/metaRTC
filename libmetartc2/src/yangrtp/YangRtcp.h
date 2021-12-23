

#ifndef YANGWEBRTC_YANGRTCRTCP_H_
#define YANGWEBRTC_YANGRTCRTCP_H_

#include <stdint.h>

// RTCP Packet Types, @see http://www.networksorcery.com/enp/protocol/rtcp.htm
enum YangRtcpType
{
    YangRtcpType_fir = 192,
    YangRtcpType_sr = 200,
    YangRtcpType_rr = 201,
    YangRtcpType_sdes = 202,
    YangRtcpType_bye = 203,
    YangRtcpType_app = 204,
    YangRtcpType_rtpfb = 205,
    YangRtcpType_psfb = 206,
    YangRtcpType_xr = 207,
};



// RTCP Header, @see http://tools.ietf.org/html/rfc3550#section-6.1
// @remark The header must be 4 bytes, which align with the max field size 2B.
struct YangRtcpHeader
{
	uint16_t rc:5;
	uint16_t padding:1;
	uint16_t version:2;
	uint16_t type:8;

	uint16_t length:16;

    YangRtcpHeader() {
        rc = 0;
        padding = 0;
        version = 0;
        type = 0;
        length = 0;
    }
};




#endif /* YANGWEBRTC_YANGRTCRTCP_H_ */
