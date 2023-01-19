//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtpRecvNack.h>
#include <yangrtp/YangRtpConstant.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangMath.h>




int yang_get_nackinfo_index(YangRtpNackInfo* a, uint16_t value,int32_t alen){
	int low,high,mid;
	low = 0;
	high =alen;
	while(low<=high){
		mid = (high+low)/2;
		if(value==a[mid].sn) return -1;
		if(yang_rtp_seq_distance(value,a[mid].sn)>0)
			high = mid-1;
		else if(yang_rtp_seq_distance(a[mid].sn,value)>0)
			low = mid+1;
		else
			return mid;
	}
	return low;
}

int32_t yang_nackinfo_insert(YangRtpNackInfoList* vec,uint16_t value){
	if(vec==NULL) return 1;
	int32_t alen=vec->vsize;
	int32_t index=-1;
	if(alen>=vec->capacity){
		YangRtpNackInfo* tmp=(YangRtpNackInfo*)yang_calloc(1,sizeof(YangRtpNackInfo)*(vec->capacity+50));
		yang_memcpy((char*)tmp,(char*)vec->nackinfos,sizeof(YangRtpNackInfo)*vec->capacity);
		vec->capacity+=50;
		yang_free(vec->nackinfos);
		vec->nackinfos=tmp;
	}
	YangRtpNackInfo* nacks=vec->nackinfos;

	if(alen==0){
		yang_create_nackinfo(&nacks[0],value);
		vec->vsize++;
		return Yang_Ok;
	}
	if (yang_rtp_seq_distance(nacks[alen-1].sn, value)>0){
		yang_create_nackinfo(&nacks[alen],value);
		vec->vsize++;
		return Yang_Ok;
	}

	index=yang_get_nackinfo_index(nacks,value,alen);
	if(index==-1) return 1;
	yang_memmove((char*) nacks + (index + 1) * sizeof(YangRtpNackInfo),
				(char*) nacks + index * sizeof(YangRtpNackInfo),
				sizeof(YangRtpNackInfo) * (alen - index));
	yang_create_nackinfo(&nacks[index],value);
	vec->vsize++;

	return Yang_Ok;
}

int32_t yang_nackinfo_remove(YangRtpNackInfo* a,uint16_t value,uint32_t* palen){
	if(a==NULL||*palen==0) return 1;

	int32_t alen=*palen;

	if(alen==1){
        if(a[0].sn!=value) return Yang_Ok;
		*palen=alen-1;
		return Yang_Ok;
	}

	for(int32_t i=0;i<alen;i++){
		if(a[i].sn==value) {
			if(i!=alen-1)
				yang_memmove((char*)a+i*sizeof(YangRtpNackInfo),(char*)a+(i+1)*sizeof(YangRtpNackInfo),sizeof(YangRtpNackInfo)*(alen-i-1));

			*palen=alen-1;
			return Yang_Ok;
		}
	}

	return Yang_Ok;
}


void yang_create_nackinfo(YangRtpNackInfo* nackinfo,uint16_t seq){
	if(nackinfo==NULL) return;
	 nackinfo->generateTime = yang_get_system_time();
	 nackinfo->preReqNackTime = 0;
	 nackinfo->reqNackCount = 0;
	 nackinfo->sn=seq;
}

void yang_nackinfo_erase(YangRtpRecvNack* nack,uint16_t seq){

}


void yang_create_nackinfovec(YangRtpNackInfoList* nackvec,uint32_t capacity){
	if(nackvec==NULL || capacity==0) return;
	nackvec->capacity=capacity;
	nackvec->nackinfos=(YangRtpNackInfo*)yang_calloc(1,capacity*sizeof(YangRtpNackInfo));
}
void yang_destroy_nackinfovec(YangRtpNackInfoList* nackvec){
	if(nackvec==NULL) return;
	yang_free(nackvec->nackinfos);
}

void yang_recvnack_initvec(YangRtpRecvNack* nack,uint32_t ssrc){
	if(nack==NULL) return;
	if(nack->rtcp.nack==NULL){
		yang_create_rtcpNack(&nack->rtcp, ssrc);
	}

}

void yang_create_recvnack(YangRtpRecvNack* nack, YangReceiveNackBuffer* rtp,size_t queue_size){
	if(nack==NULL) return;
	nack->opts.maxReqCount = 10;
	nack->opts.maxAliveTime = 1000 * YANG_UTIME_MILLISECONDS;
	nack->opts.firstNackInterval = 10 * YANG_UTIME_MILLISECONDS;
	nack->opts.nackInterval = 50 * YANG_UTIME_MILLISECONDS;
	nack->opts.maxNackInterval = 500 * YANG_UTIME_MILLISECONDS;
	nack->opts.minNackInterval = 15 * YANG_UTIME_MILLISECONDS;

	nack->opts.nackCheckInterval = 15 * YANG_UTIME_MILLISECONDS;

	yang_create_nackinfovec(&nack->queue, 100);

	nack->maxQueueSize = queue_size;
	nack->rtp = rtp;
	nack->preCheckTime = 0;
	nack->rtt = 0;

	yang_memset(&nack->rtcp, 0, sizeof(YangRtcpCommon));

}


void yang_destroy_recvnack(YangRtpRecvNack* nack){
	if(nack==NULL) return;
	yang_destroy_rtcpNack(&nack->rtcp);
	yang_destroy_nackinfovec(&nack->queue);
}


void yang_recvnack_insert(YangRtpRecvNack* nack,uint16_t first, uint16_t last)
{
	if(nack==NULL) return;
   for (uint16_t s = first; s != last; ++s) {
	   yang_nackinfo_insert(&nack->queue,s);
    }
}

void yang_recvnack_remove(YangRtpRecvNack* nack,uint16_t seq)
{
	if(nack==NULL) return;
	yang_nackinfo_remove(nack->queue.nackinfos,seq,&nack->queue.vsize);
}

YangRtpNackInfo* yang_recvnack_find(YangRtpRecvNack* nack,uint16_t seq)
{
	if(nack==NULL) return NULL;
	for(int i=0;i<nack->queue.vsize;i++){
		if(nack->queue.nackinfos[i].sn==seq) return &nack->queue.nackinfos[i];
	}
	return NULL;

}

void yang_recvnack_check_queue_size(YangRtpRecvNack* nack)
{
    if (nack->queue.vsize >= nack->maxQueueSize) {
    	yang_nackbuffer_notify_nack_list_full(nack->rtp);
    	nack->queue.vsize=0;
    }
}
int32_t yang_recvnack_isEmpty(YangRtpRecvNack* nack){
	return nack->queue.vsize==0?1:0;
}
void yang_recvnack_get_nack_seqs(YangRtpRecvNack* recvnack,YangRtcpNack* nack, uint32_t* timeout_nacks)
{

    int64_t now = yang_get_system_time();
    if (now - recvnack->preCheckTime < recvnack->opts.nackCheckInterval) {
        return;
    }
    recvnack->preCheckTime = now;

   for(int i=0;i<recvnack->queue.vsize;i++){
        const uint16_t seq = recvnack->queue.nackinfos[i].sn;
        YangRtpNackInfo* nack_info = &recvnack->queue.nackinfos[i];

        int32_t alive_time = now - nack_info->generateTime;
        if (alive_time > recvnack->opts.maxAliveTime || nack_info->reqNackCount > recvnack->opts.maxReqCount) {
            *timeout_nacks=*timeout_nacks+1;
            yang_nackbuffer_notify_drop_seq(recvnack->rtp,seq);
            yang_recvnack_remove(recvnack,seq);
            continue;
        }

        // TODO:Statistics unorder packet.
        if (now - nack_info->generateTime < recvnack->opts.firstNackInterval) {
            break;
        }

        int64_t nack_interval = yang_max(recvnack->opts.minNackInterval, recvnack->opts.nackInterval / 3);
        if(recvnack->opts.nackInterval < 50 * YANG_UTIME_MILLISECONDS){
            nack_interval = yang_max(recvnack->opts.minNackInterval, recvnack->opts.nackInterval);
        }

        if (now - nack_info->preReqNackTime >= nack_interval ) {
            ++nack_info->reqNackCount;
            nack_info->preReqNackTime = now;

            yang_rtcpNack_addSn(nack,seq);
        }


    }


}

void yang_recvnack_update_rtt(YangRtpRecvNack* nack,int32_t rtt)
{
    nack->rtt = rtt * YANG_UTIME_MILLISECONDS;

    if (nack->rtt > nack->opts.nackInterval) {
        nack->opts.nackInterval = nack->opts.nackInterval  * 0.8 + nack->rtt * 0.2;
    } else {
        nack->opts.nackInterval = nack->rtt;
    }

    nack->opts.nackInterval = yang_min(nack->opts.nackInterval, nack->opts.maxNackInterval);
}

