
#ifndef YANGWEBRTC_YANGRTCCOMM_H_
#define YANGWEBRTC_YANGRTCCOMM_H_
#include <stdint.h>
#define YANG_UTIME_MILLISECONDS 1000

const int32_t kRtpPacketSize        = 1500;

const uint8_t kSR   = 200;
const uint8_t kRR   = 201;
const uint8_t kSDES = 202;
const uint8_t kBye  = 203;
const uint8_t kApp  = 204;

// @see: https://tools.ietf.org/html/rfc4585#section-6.1
const uint8_t kRtpFb = 205;
const uint8_t kPsFb  = 206;
const uint8_t kXR    = 207;


// Firefox defaults as 109, Chrome is 111.
const int32_t kAudioPayloadType     = 111;
const int32_t kAudioChannel         = 2;
const int32_t kAudioSamplerate      = 48000;

// Firefox defaults as 126, Chrome is 102.
const int32_t kVideoPayloadType = 102;
const int32_t kVideoSamplerate  = 90000;
const int32_t kRtpMaxPayloadSize = kRtpPacketSize - 200;

const int32_t kRtcpPacketSize = 1500;
const uint8_t kRtcpVersion = 0x2;

// 1500 - 20(ip_header) - 8(udp_header)
const int32_t kMaxUDPDataSize = 1472;

// @see: https://tools.ietf.org/html/rfc4585#section-6.3
const uint8_t kPLI  = 1;
const uint8_t kSLI  = 2;
const uint8_t kRPSI = 3;
const uint8_t kAFB  = 15;

const int32_t kRtpHeaderFixedSize   = 12;
const uint8_t kRtpMarker        = 0x80;

// H.264 nalu header type mask.


// @see: https://tools.ietf.org/html/rfc6184#section-5.2
const uint8_t kStapA            = 24;

// @see: https://tools.ietf.org/html/rfc6184#section-5.2
const uint8_t kFuA              = 28;

// @see: https://tools.ietf.org/html/rfc6184#section-5.8
const uint8_t kStart            = 0x80; // Fu-header start bit
const uint8_t kEnd              = 0x40; // Fu-header end bit

#endif /* YANGWEBRTC_YANGRTCCOMM_H_ */
