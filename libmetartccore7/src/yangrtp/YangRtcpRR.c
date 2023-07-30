//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangLog.h>

#include <yangrtp/YangRtcpRR.h>
#include <yangrtp/YangRtpConstant.h>

int32_t yang_decode_rtcpRR(YangRtcpCommon *comm, YangBuffer *buffer) {
	if (comm == NULL || comm->rb == NULL)
		return yang_error_wrap(1, "RtcpRB is null");
	/*
	 @doc: https://tools.ietf.org/html/rfc3550#section-6.4.2

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 header |V=2|P|    RC   |   PT=RR=201   |             length            |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                     SSRC of packet sender                     |
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

	// @doc https://tools.ietf.org/html/rfc3550#section-6.4.2
	// An empty RR packet (RC = 0) MUST be put at the head of a compound
	// RTCP packet when there is no data transmission or reception to
	// report. e.g. {80 c9 00 01 00 00 00 01}
	if (comm->header.rc == 0) {
        //return yang_error_wrap(ERROR_RTC_RTCP_EMPTY_RR, "rc=0");
        return ERROR_RTC_RTCP_EMPTY_RR;
	}

	// TODO: FIXME: Security check for read.
	comm->rb->ssrc = yang_read_4bytes(buffer);
	comm->rb->fraction_lost = yang_read_1bytes(buffer);
	comm->rb->lost_packets = yang_read_3bytes(buffer);
	comm->rb->highest_sn = yang_read_4bytes(buffer);
	comm->rb->jitter = yang_read_4bytes(buffer);
	comm->rb->lsr = yang_read_4bytes(buffer);
	comm->rb->dlsr = yang_read_4bytes(buffer);

	// TODO: FIXME: Security check for read.
	if (comm->header.rc > 1) {
		char buf[1500];
		yang_read_bytes(buffer, buf, (comm->header.rc - 1) * 24);
	}

	return err;
}
int32_t yang_encode_rtcpRR(YangRtcpCommon *comm, YangBuffer *buffer) {
	if (comm == NULL || comm->rb == NULL)
		return yang_error_wrap(1, "RtcpRB is null");
	/*
	 @doc: https://tools.ietf.org/html/rfc3550#section-6.4.2

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 header |V=2|P|    RC   |   PT=RR=201   |             length            |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                     SSRC of packet sender                     |
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

	if (!yang_buffer_require(buffer, yang_rtcpRR_nb_bytes(comm))) {
		return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes",
				yang_rtcpRR_nb_bytes(comm));
	}

	comm->header.rc = 1;
	if (Yang_Ok != (err = yang_encode_header_rtcpCommon(comm, buffer))) {
		return yang_error_wrap(err, "encode header");
	}

	yang_write_4bytes(buffer, comm->rb->ssrc);
	yang_write_1bytes(buffer, comm->rb->fraction_lost);
	yang_write_3bytes(buffer, comm->rb->lost_packets);
	yang_write_4bytes(buffer, comm->rb->highest_sn);
	yang_write_4bytes(buffer, comm->rb->jitter);
	yang_write_4bytes(buffer, comm->rb->lsr);
	yang_write_4bytes(buffer, comm->rb->dlsr);

	return err;
}
uint64_t yang_rtcpRR_nb_bytes(YangRtcpCommon *comm) {
	return (comm->header.length + 1) * 4;
}
void yang_create_rtcpRR(YangRtcpCommon *comm) {
	if (comm == NULL)	return;
	comm->header.padding = 0;
	comm->header.type = YangRtcpType_rr;
	comm->header.rc = 0;
	comm->header.version = kRtcpVersion;
	comm->header.length = 7;
	comm->ssrc = 0;
	if(comm->rb==NULL) comm->rb=(YangRtcpRB*)yang_calloc(sizeof(YangRtcpRB),1);
}

void yang_destroy_rtcpRR(YangRtcpCommon *comm) {
	if (comm == NULL || comm->rb == NULL)	return;
	yang_free(comm->rb);
}

void yang_rtcpRR_set_sender_ntp(YangRtcpCommon *comm, uint64_t ntp) {
	if (comm == NULL || comm->rb == NULL)
		return;
	uint32_t lsr = (uint32_t) ((ntp >> 16) & 0x00000000FFFFFFFF);
	comm->rb->lsr = lsr;
}
void yang_set_sender_ntp(YangRtcpCommon *comm, uint64_t ntp) {
	if (comm == NULL || comm->rb == NULL)
		return;
	uint32_t lsr = (uint32_t) ((ntp >> 16) & 0x00000000FFFFFFFF);
	comm->rb->lsr = lsr;
}

