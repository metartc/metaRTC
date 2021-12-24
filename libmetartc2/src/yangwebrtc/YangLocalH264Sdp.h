#ifndef SRC_YANGWEBRTC_YANGLOCALH264SDP_H_
#define SRC_YANGWEBRTC_YANGLOCALH264SDP_H_

const char* local_h264_sdp_str="o=Yangwebrtc 93937470806864 2 IN IP4 0.0.0.0\n"\
		"s=YangPlaySession\nt=0 0\na=ice-lite\na=group:BUNDLE 0 1\n"\
		"a=msid-semantic: WMS live/livestream\nm=audio 9 UDP/TLS/RTP/SAVPF 111\n"\
		"c=IN IP4 0.0.0.0\n"\
		"a=ice-ufrag:7nlf8226\n"\
		"a=ice-pwd:86971b13h9723f8oc0z72h82a42x6184\n"\
		"a=fingerprint:sha-256 A0:8B:98:29:DE:2E:16:0C:60:C6:D0:09:1C:A4:E9:3F:7C:06:98:53:03:A3:6F:0E:67:2B:9A:9D:F0:4F:A3:D6\n"\
		"a=setup:passive\na=mid:0\na=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\n"\
		"a=sendonly\n"\
		"a=rtcp-mux\na=rtcp-rsize\n"\
		"a=rtpmap:111 opus/48000/2\n"\
		"a=rtcp-fb:111 transport-cc\n"\
		"a=fmtp:111 minptime=10;useinbandfec=1\n"\
		"a=ssrc:144706935 cname:5hcuv2i7jtwg6bj1\n"\
		"a=ssrc:144706935 msid:- 79ac70ad-c572-4017-86a1-330d56089c70\n"\
		"a=ssrc:144706935 mslabel:-\n"\
		"a=ssrc:144706935 label:79ac70ad-c572-4017-86a1-330d56089c70\n"\
		"a=candidate:0 1 udp 2130706431 127.0.0.1 8000 typ host generation 0\n"\
		"m=video 9 UDP/TLS/RTP/SAVPF 125 114\nc=IN IP4 0.0.0.0\na=ice-ufrag:7nlf8226\n"\
		"a=ice-pwd:86971b13h9723f8oc0z72h82a42x6184\n"\
		"a=fingerprint:sha-256 A0:8B:98:29:DE:2E:16:0C:60:C6:D0:09:1C:A4:E9:3F:7C:06:98:53:03:A3:6F:0E:67:2B:9A:9D:F0:4F:A3:D6\n"\
		"a=setup:passive\n"\
		"a=mid:1\na=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\n"\
		"a=sendonly\n"\
		"a=rtcp-mux\n"\
		"a=rtcp-rsize\n"\
		"a=rtpmap:125 H264/90000\n"\
		"a=rtcp-fb:125 transport-cc\n"\
		"a=rtcp-fb:125 nack\n"\
		"a=rtcp-fb:125 nack pli\n"\
		"a=fmtp:125 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\n"\
		"a=rtpmap:114 red/90000\na=ssrc:144706936 cname:5hcuv2i7jtwg6bj1\n"\
		"a=ssrc:144706936 msid:- 549dbfe5-fd12-4c5f-9182-d513f6defce5\n"\
		"a=ssrc:144706936 mslabel:-\n"\
		"a=ssrc:144706936 label:549dbfe5-fd12-4c5f-9182-d513f6defce5\n"\
		"a=candidate:0 1 udp 2130706431 127.0.0.1 8000 typ host generation 0";



#endif /* SRC_YANGWEBRTC_YANGLOCALH264SDP_H_ */
