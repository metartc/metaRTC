//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangLog.h>

#include <yangrtp/YangRtcpSR.h>
#include <yangrtp/YangRtpConstant.h>

void yang_create_rtcpSR(YangRtcpCommon *comm) {
	if (comm == NULL)
		return;
	comm->header.padding = 0;
	comm->header.type = YangRtcpType_sr;
	comm->header.rc = 0;
	comm->header.version = kRtcpVersion;
	comm->header.length = 6;
    if (comm->sr == NULL)    comm->sr = (YangRtcpSR*) yang_calloc(1, sizeof(YangRtcpSR));

	comm->ssrc = 0;
	comm->sr->ntp = 0;
	comm->sr->rtp_ts = 0;
	comm->sr->send_rtp_packets = 0;
	comm->sr->send_rtp_bytes = 0;



}

void yang_destroy_rtcpSR(YangRtcpCommon *comm) {
	if (comm || comm->sr == NULL)
		return;
	yang_free(comm->sr);
}

int32_t yang_decode_rtcpSR(YangRtcpCommon *comm, YangBuffer *buffer) {
	if (comm == NULL || buffer == NULL)
		return 1;
	/* @doc: https://tools.ietf.org/html/rfc3550#section-6.4.1
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 header |V=2|P|    RC   |   PT=SR=200   |             length            |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                         SSRC of sender                        |
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 sender |              NTP timestamp, most significant word             |
	 info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |             NTP timestamp, least significant word             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                         RTP timestamp                         |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                     sender's packet count                     |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                      sender's octet count                     |
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 report |                 SSRC_1 (SSRC of first source)                 |
	 block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 1    | fraction lost |       cumulative number of packets lost       |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |           extended highest sequence number received           |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                      interarrival jitter                      |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                         last SR (LSR)                         |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                   delay since last SR (DLSR)                  |
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 report |                 SSRC_2 (SSRC of second source)                |
	 block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 2    :                               ...                             :
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 |                  profile-specific extensions                  |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	int32_t err = Yang_Ok;
	comm->data = buffer->head;
	comm->nb_data = yang_buffer_left(buffer);

	if (Yang_Ok != (err = yang_decode_header_rtcpCommon(comm, buffer))) {
		return yang_error_wrap(err, "decode header");
	}

	comm->sr->ntp = yang_read_8bytes(buffer);
	comm->sr->rtp_ts = yang_read_4bytes(buffer);
	comm->sr->send_rtp_packets = yang_read_4bytes(buffer);
	comm->sr->send_rtp_bytes = yang_read_4bytes(buffer);

	if (comm->header.rc > 0) {
		char buf[1500];
		yang_read_bytes(buffer, buf, comm->header.rc * 24);
	}

	return err;
}
int32_t yang_encode_rtcpSR(YangRtcpCommon *comm, YangBuffer *buffer) {
	if (comm == NULL || buffer == NULL)
		return 1;
	int32_t err = Yang_Ok;
	/* @doc: https://tools.ietf.org/html/rfc3550#section-6.4.1
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 header |V=2|P|    RC   |   PT=SR=200   |             length            |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                         SSRC of sender                        |
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 sender |              NTP timestamp, most significant word             |
	 info   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |             NTP timestamp, least significant word             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                         RTP timestamp                         |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                     sender's packet count                     |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                      sender's octet count                     |
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 report |                 SSRC_1 (SSRC of first source)                 |
	 block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 1    | fraction lost |       cumulative number of packets lost       |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |           extended highest sequence number received           |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                      interarrival jitter                      |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                         last SR (LSR)                         |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                   delay since last SR (DLSR)                  |
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 report |                 SSRC_2 (SSRC of second source)                |
	 block  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 2    :                               ...                             :
	 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
	 |                  profile-specific extensions                  |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	if (!yang_buffer_require(buffer, yang_get_rtcpSR_nb_bytes(comm))) {
		return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes",
				yang_get_rtcpSR_nb_bytes(comm));
	}

	if (Yang_Ok != (err = yang_encode_header_rtcpCommon(comm, buffer))) {
		return yang_error_wrap(err, "encode header");
	}

	yang_write_8bytes(buffer, comm->sr->ntp);
	yang_write_4bytes(buffer, comm->sr->rtp_ts);
	yang_write_4bytes(buffer, comm->sr->send_rtp_packets);
	yang_write_4bytes(buffer, comm->sr->send_rtp_bytes);

	return err;
}

uint64_t yang_get_rtcpSR_nb_bytes(YangRtcpCommon *comm) {
	return (comm->header.length + 1) * 4;
}

