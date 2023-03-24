//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtcpNack.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangMath.h>
#include <yangrtp/YangRtpConstant.h>



void yang_rtcpNack_clear(YangRtcpNack *nack) {
	if(nack->vsize==0) return;
	yang_memset(nack->nacks,0,nack->vsize * sizeof(uint16_t));
	nack->vsize=0;
}

void yang_rtcpNack_addSn(YangRtcpNack *nack, uint16_t sn) {
	if (nack == NULL)
		return;
	if (nack->vsize >= nack->capacity - 1) {
		uint16_t *tmp = (uint16_t*) yang_calloc((nack->capacity + 50) * sizeof(uint16_t), 1);
		yang_memcpy(tmp, nack->nacks, nack->vsize * sizeof(uint16_t));
		yang_free(nack->nacks);
		nack->nacks = tmp;
		nack->capacity += 50;
	}
	yang_insert_uint16_sort(nack->nacks, sn, &nack->vsize);

}

int32_t yang_decode_rtcpNack(YangRtcpCommon *comm, YangBuffer *buffer) {
	if (comm == NULL)
		return 1;
	/*
	 @doc: https://tools.ietf.org/html/rfc4585#section-6.1
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |V=2|P|   FMT   |       PT      |          length               |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                  SSRC of packet sender                        |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                  SSRC of media source                         |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 :            Feedback Control Information (FCI)                 :
	 :                                                               :

	 Generic NACK
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |            PID                |             BLP               |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	int32_t err = Yang_Ok;
	comm->data = buffer->head;
	comm->nb_data = yang_buffer_left(buffer);

	if (Yang_Ok != (err = yang_decode_header_rtcpCommon(comm, buffer))) {
		return yang_error_wrap(err, "decode header");
	}

    comm->nack->mediaSsrc = yang_read_4bytes(buffer);
	char bitmask[20];
	int32_t i = 0;
	for (i = 0; i < (comm->header.length - 2); i++) {
		uint16_t pid = yang_read_2bytes(buffer);
		uint16_t blp = yang_read_2bytes(buffer);
		yang_rtcpNack_addSn(comm->nack, pid);

		yang_memset(bitmask, 0, 20);
		for (int32_t j = 0; j < 16; j++) {
			bitmask[j] = (blp & (1 << j)) >> j ? '1' : '0';

			if ((blp & (1 << j)) >> j)
				yang_rtcpNack_addSn(comm->nack, pid + j + 1);
		}
		bitmask[16] = '\n';

	}

	return err;
}
int32_t yang_encode_rtcpNack(YangRtcpCommon *comm, YangBuffer *buffer) {
	if (comm == NULL || buffer == NULL)
		return 1;
	/*
	 @doc: https://tools.ietf.org/html/rfc4585#section-6.1
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |V=2|P|   FMT   |       PT      |          length               |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                  SSRC of packet sender                        |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                  SSRC of media source                         |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 :            Feedback Control Information (FCI)                 :
	 :                                                               :

	 Generic NACK
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |            PID                |             BLP               |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	int32_t err = Yang_Ok;
	if (!yang_buffer_require(buffer, yang_rtcpNack_nb_bytes())) {
		return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes",
				yang_rtcpNack_nb_bytes());
	}

	YangPidBlp *chunks = (YangPidBlp*) yang_malloc(comm->nack->vsize * sizeof(YangPidBlp));
	int32_t vsize = 0;

	do {
		YangPidBlp chunk;
		chunk.in_use = yangfalse;
		uint16_t pid = 0;
		for (int32_t i = 0; i < comm->nack->vsize; i++) {
			uint16_t sn = comm->nack->nacks[i];
			if (!chunk.in_use) {
				chunk.pid = sn;
				chunk.blp = 0;
				chunk.in_use = yangtrue;
				pid = sn;
				continue;
			}
			if ((sn - pid) < 1) {
				yang_info("skip seq %d", sn);
			} else if ((sn - pid) > 16) {
				// add new chunk

				chunks[vsize].blp = chunk.blp;
				chunks[vsize++].pid = chunk.pid;
				chunk.in_use = yangfalse;
			} else {
				chunk.blp |= 1 << (sn - pid - 1);
			}
		}
		if (chunk.in_use) {

			chunks[vsize].blp = chunk.blp;
			chunks[vsize++].pid = chunk.pid;
		}

		comm->header.length = 2 + vsize;
		if (Yang_Ok != (err = yang_encode_header_rtcpCommon(comm, buffer))) {
			err = yang_error_wrap(err, "encode header");
			break;
		}

        yang_write_4bytes(buffer, comm->nack->mediaSsrc);

		for (int j = 0; j < vsize; j++) {
			yang_write_2bytes(buffer, chunks[j].pid);
			yang_write_2bytes(buffer, chunks[j].blp);
		}
	} while (0);
	yang_free(chunks);
	return err;
}

uint64_t yang_rtcpNack_nb_bytes() {
	return kRtcpPacketSize;
}


void yang_rtcpNack_init(YangRtcpCommon *comm, uint32_t pssrc) {
	if (comm == NULL)	return;
	comm->header.padding = 0;
	comm->header.type = YangRtcpType_rtpfb;
	comm->header.rc = 1;
	comm->header.version = kRtcpVersion;
	comm->header.length=0;
	comm->ssrc = pssrc;

    if(comm->nack) comm->nack->mediaSsrc = pssrc;
}


void yang_create_rtcpNack(YangRtcpCommon *comm, uint32_t pssrc) {
	if (comm == NULL)	return;	
	if (comm->nack == NULL)
		comm->nack = (YangRtcpNack*) yang_calloc(1, sizeof(YangRtcpNack));
	comm->nack->capacity = 50;
	if (comm->nack->nacks == NULL)
		comm->nack->nacks = (uint16_t*) yang_malloc(sizeof(uint16_t) * comm->nack->capacity);

    yang_rtcpNack_init(comm,pssrc);

}

void yang_destroy_rtcpNack(YangRtcpCommon *comm) {
	if (comm == NULL || comm->nack == NULL)		return;

	yang_free(comm->nack->nacks);
	comm->nack->vsize = 0;
	yang_free(comm->nack);
}


