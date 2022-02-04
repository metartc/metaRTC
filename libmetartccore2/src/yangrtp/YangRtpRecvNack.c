//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangrtp/YangRtpRecvNack.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>
#include <stdlib.h>



int32_t yang_nackinfo_insert(YangRtpNackInfoVec* vec,uint16_t value){
	if(vec==NULL) return 1;
	int32_t alen=vec->vsize;//palen;
	if(alen>=vec->capacity){

		YangRtpNackInfo** tmp=(YangRtpNackInfo**)calloc(1,sizeof(YangRtpNackInfo*)*(vec->capacity+50));
		memcpy((char*)tmp,(char*)vec->nackinfos,sizeof(YangRtpNackInfo*)*vec->capacity);
		vec->capacity+=50;
		yang_free(vec->nackinfos);
		vec->nackinfos=tmp;
	}
	YangRtpNackInfo** a=vec->nackinfos;
	YangRtpNackInfo* nackinfo=NULL;
	if(alen==0){
		nackinfo=(YangRtpNackInfo*)calloc(1,sizeof(YangRtpNackInfo));
		yang_init_nackinfo(nackinfo,value);
		a[0]=nackinfo;
		vec->vsize++;
		return Yang_Ok;
	}
	for(int i=0;i<alen;i++){
		if(i==alen-1){
			if(value==a[i]->sn) return 1;
			//if(value<a[i]){
			nackinfo=(YangRtpNackInfo*)calloc(1,sizeof(YangRtpNackInfo));
			yang_init_nackinfo(nackinfo,value);
			if(yang_rtp_seq_distance(value,a[i]->sn)){
				a[alen]=a[i];
				a[i]=nackinfo;
			}else{
				a[alen]=nackinfo;
			}
			vec->vsize=alen+1;
			return Yang_Ok;
		}
		if(a[i]->sn==value||a[i+1]->sn==value) return 1;

		nackinfo=(YangRtpNackInfo*)calloc(1,sizeof(YangRtpNackInfo));
		yang_init_nackinfo(nackinfo,value);
		if(yang_rtp_seq_distance(a[i]->sn,value)&&yang_rtp_seq_distance(value,a[i+1]->sn)){
			int left=alen-i;
			memmove((char*)a+(i+2)*sizeof(YangRtpNackInfo*),(char*)a+(i+1)*sizeof(YangRtpNackInfo*),sizeof(YangRtpNackInfo*)*left);
			a[i+1]=nackinfo;
			vec->vsize=vec->vsize+1;
			return Yang_Ok;
		}
	}
	return Yang_Ok;
}
int32_t yang_nackinfo_remove(YangRtpNackInfo** a,uint16_t value,uint32_t* palen){
	if(a==NULL||*palen==0) return 1;
	int32_t alen=*palen;
	for(int32_t i=0;i<alen;i++){
		if(a[i]&&a[i]->sn==value) {
			yang_free(a[i]);
			int left=alen-1;
			if(left>0) memmove((char*)a+i*sizeof(YangRtpNackInfo*),(char*)a+(i+1)*sizeof(YangRtpNackInfo*),sizeof(YangRtpNackInfo*)*left);
			*palen=alen-1;
			return Yang_Ok;
		}


	}

	return Yang_Ok;
}


void yang_init_nackinfo(YangRtpNackInfo* nackinfo,uint16_t seq){
	if(nackinfo==NULL) return;
	 nackinfo->generate_time_ = yang_get_system_time();//yang_update_system_time();
	 nackinfo->pre_req_nack_time_ = 0;
	 nackinfo->req_nack_count_ = 0;
	 nackinfo->sn=seq;
}

void yang_nackinfo_erase(YangRtpRecvNack* nack,uint16_t seq){

}


void yang_init_nackinfovec(YangRtpNackInfoVec* nackvec,uint32_t capacity){
	if(nackvec==NULL || capacity==0) return;
	nackvec->capacity=capacity;
	nackvec->nackinfos=(YangRtpNackInfo**)calloc(1,capacity*sizeof(YangRtpNackInfo*));
}
void yang_destroy_nackinfovec(YangRtpNackInfoVec* nackvec){

}

void yang_init_recvnack(YangRtpRecvNack* nack, YangReceiveNackBuffer* rtp,size_t queue_size){
	if(nack==NULL) return;
	nack->opts.max_req_count = 10;
	nack->opts.max_alive_time = 1000 * YANG_UTIME_MILLISECONDS;
	nack->opts.first_nack_interval = 10 * YANG_UTIME_MILLISECONDS;
	nack->opts.nack_interval = 50 * YANG_UTIME_MILLISECONDS;
	nack->opts.max_nack_interval = 500 * YANG_UTIME_MILLISECONDS;
	nack->opts.min_nack_interval = 15 * YANG_UTIME_MILLISECONDS;

	nack->opts.nack_check_interval = 15 * YANG_UTIME_MILLISECONDS;

	yang_init_nackinfovec(&nack->queue, 100);

	nack->max_queue_size = queue_size;
	nack->rtp = rtp;
	nack->pre_check_time = 0;
	nack->rtt = 0;

}
void yang_destroy_recvnack(YangRtpRecvNack* nack){
	if(nack==NULL) return;
	yang_free(nack->queue.nackinfos);
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
	for(int i=0;i<nack->queue.vsize;i++){
		if(nack->queue.nackinfos[i]&&nack->queue.nackinfos[i]->sn==seq) return nack->queue.nackinfos[i];
	}
	return NULL;

}

void yang_recvnack_check_queue_size(YangRtpRecvNack* nack)
{
    if (nack->queue.vsize >= nack->max_queue_size) {
    	yang_nackbuffer_notify_nack_list_full(nack->rtp);

    	for(int32_t i=0;i<nack->queue.vsize;i++){
    		yang_free(nack->queue.nackinfos[i]);
    	}
    	nack->queue.vsize=0;

    }
}
int32_t yang_recvnack_isEmpty(YangRtpRecvNack* nack){
	return nack->queue.vsize==0?1:0;
}
void yang_recvnack_get_nack_seqs(YangRtpRecvNack* recvnack,YangRtcpNack* nack, uint32_t* timeout_nacks)
{

    int64_t now = yang_get_system_time();

    //int64_t interval = now - nack->pre_check_time;

    if (now - recvnack->pre_check_time < recvnack->opts.nack_check_interval) {
        return;
    }
    recvnack->pre_check_time = now;




   for(int i=0;i<recvnack->queue.vsize;i++){
        const uint16_t seq = recvnack->queue.nackinfos[i]->sn;
        YangRtpNackInfo* nack_info = recvnack->queue.nackinfos[i];

        int32_t alive_time = now - nack_info->generate_time_;
        if (alive_time > recvnack->opts.max_alive_time || nack_info->req_nack_count_ > recvnack->opts.max_req_count) {
            *timeout_nacks=*timeout_nacks+1;
            yang_nackbuffer_notify_drop_seq(recvnack->rtp,seq);
            yang_recvnack_remove(recvnack,seq);
            continue;
        }

        // TODO:Statistics unorder packet.
        if (now - nack_info->generate_time_ < recvnack->opts.first_nack_interval) {
            break;
        }

        int64_t nack_interval = yang_max(recvnack->opts.min_nack_interval, recvnack->opts.nack_interval / 3);
        if(recvnack->opts.nack_interval < 50 * YANG_UTIME_MILLISECONDS){
            nack_interval = yang_max(recvnack->opts.min_nack_interval, recvnack->opts.nack_interval);
        }

        if (now - nack_info->pre_req_nack_time_ >= nack_interval ) {
            ++nack_info->req_nack_count_;
            nack_info->pre_req_nack_time_ = now;
            yang_rtcpNack_addSn(nack,seq);
        }


    }


}

void yang_recvnack_update_rtt(YangRtpRecvNack* nack,int32_t rtt)
{
    nack->rtt = rtt * YANG_UTIME_MILLISECONDS;

    if (nack->rtt > nack->opts.nack_interval) {
        nack->opts.nack_interval = nack->opts.nack_interval  * 0.8 + nack->rtt * 0.2;
    } else {
        nack->opts.nack_interval = nack->rtt;
    }

    nack->opts.nack_interval = yang_min(nack->opts.nack_interval, nack->opts.max_nack_interval);
}

