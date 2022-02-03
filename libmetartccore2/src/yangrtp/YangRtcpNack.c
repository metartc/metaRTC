#include <yangrtp/YangRtcpNack.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangCMath.h>
#include <yangrtp/YangRtpConstant.h>
#include <stdlib.h>
#include <memory.h>


void yang_init_rtcpNack(YangRtcpCommon* comm,uint32_t pssrc){
	if(comm==NULL) return;

	   comm->header.padding = 0;
	    comm->header.type = YangRtcpType_rtpfb;
	    comm->header.rc = 1;
	    comm->header.version = kRtcpVersion;
	    comm->ssrc = pssrc;
	    if(comm->nack==NULL) comm->nack=(YangRtcpNack*)calloc(1,sizeof(YangRtcpNack));
	    comm->nack->capacity=50;
	    if(comm->nack->nacks==NULL) comm->nack->nacks=(uint16_t*)malloc(sizeof(uint16_t)* comm->nack->capacity);
	    comm->nack->media_ssrc = pssrc;
}
void yang_destroy_rtcpNack(YangRtcpCommon* comm){
	if(comm||comm->nack==NULL) return;
	yang_free(comm->nack->nacks);
	comm->nack->vlen=0;
	yang_free(comm->nack);
}
void yang_rtcpNack_addSn(YangRtcpNack* nack,uint16_t sn){
	if(nack==NULL) return;
	if(nack->vlen>=nack->capacity-1){
		uint16_t* tmp=(uint16_t*)malloc(nack->vlen*sizeof(uint16_t));
		memcpy(tmp,nack->nacks,nack->vlen*sizeof(uint16_t));
		yang_free(nack->nacks);
		nack->nacks=(uint16_t*)calloc((nack->capacity+50)*sizeof(uint16_t),1);
		memcpy(nack->nacks,tmp,nack->vlen*sizeof(uint16_t));
		yang_free(tmp);
		nack->capacity+=50;
	}
	yang_insert_uint16(nack->nacks,sn,&nack->vlen);

}


int32_t yang_decode_rtcpNack(YangRtcpCommon* comm,YangBuffer *buffer){
	if(comm==NULL) return 1;
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
	uint16_t tmp[750];
	uint32_t vlen = 0;
	comm->nack->media_ssrc = yang_read_4bytes(buffer);
	char bitmask[20];
	for (int32_t i = 0; i < (comm->header.length - 2); i++) {
		uint16_t pid = yang_read_2bytes(buffer);
		uint16_t blp = yang_read_2bytes(buffer);
		if (vlen == 0) {
			tmp[vlen++] = pid;
		} else {
			yang_insert_uint16(tmp, pid, &vlen);
		}
		memset(bitmask, 0, 20);
		for (int32_t j = 0; j < 16; j++) {
			bitmask[j] = (blp & (1 << j)) >> j ? '1' : '0';
			if ((blp & (1 << j)) >> j)
				yang_insert_uint16(tmp, pid + j + 1, &vlen);

		}
		bitmask[16] = '\n';
		//yang_info("[%d] %d / %s", i, pid, bitmask);
	}
	comm->nack->vlen = vlen;
	if (vlen)	memcpy(comm->nack->nacks, tmp, vlen * sizeof(uint16_t));

	return err;
}
int32_t yang_encode_rtcpNack(YangRtcpCommon* comm,YangBuffer *buffer){
	if(comm==NULL||buffer==NULL) return 1;
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
	    if(!yang_buffer_require(buffer,yang_rtcpNack_nb_bytes())) {
	        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", yang_rtcpNack_nb_bytes());
	    }

        YangPidBlp* chunks=(YangPidBlp*)malloc(comm->nack->vlen*sizeof(YangPidBlp));
	    int32_t vlen=0;

	    do {
	        YangPidBlp chunk;
	        chunk.in_use = false;
	        uint16_t pid = 0;
	        for(int32_t i=0;i<comm->nack->vlen;i++) {
	            uint16_t sn = comm->nack->nacks[i];
	            if(!chunk.in_use) {
	                chunk.pid = sn;
	                chunk.blp = 0;
	                chunk.in_use = true;
	                pid = sn;
	                continue;
	            }
	            if((sn - pid) < 1) {
	                yang_info("skip seq %d", sn);
	            } else if( (sn - pid) > 16) {
	                // add new chunk

	            	chunks[vlen].blp=chunk.blp;
	            	chunks[vlen++].pid=chunk.pid;
	                chunk.in_use = false;
	            } else {
	                chunk.blp |= 1 << (sn-pid-1);
	            }
	        }
	        if(chunk.in_use) {

	        	chunks[vlen].blp=chunk.blp;
	        	chunks[vlen++].pid=chunk.pid;
	        }

	        comm->header.length = 2 + vlen;
	        if(Yang_Ok != (err = yang_encode_header_rtcpCommon(comm,buffer))) {
	            err = yang_error_wrap(err, "encode header");
	            break;
	        }

	        yang_write_4bytes(buffer,comm->nack->media_ssrc);

	        for(int j=0;j<vlen;j++) {
	            yang_write_2bytes(buffer,chunks[j].pid);
	            yang_write_2bytes(buffer,chunks[j].blp);
	        }
	    } while(0);
        yang_free(chunks);
	    return err;
}
uint64_t yang_rtcpNack_nb_bytes(){
	return 1500;
}

