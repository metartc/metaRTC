//
// Copyright (c) 2019-2022 JerryQiu
//
#include <yangipc/YangIpcEncoder.h>
#include <yangipc_rk/YangRkCommon.h>

#include <yangutil/yang_unistd.h>

#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>
#include <yangavutil/video/YangCMeta.h>
#include <stdlib.h>
#include <malloc.h>

#include "rkmedia/rkmedia_buffer.h"


void yang_rkEncoder_sendMsgToEncoder(YangEncoderSession* session,YangRequestType request){

    if(request==Yang_Req_Sendkeyframe){
        yang_trace("RK_MPI_SYS_GetMediaBuffer rel\n");
    }else if(request==Yang_Req_HighLostPacketRate){

    }else if(request==Yang_Req_LowLostPacketRate){

    }
}

int yang_rkEncoder_save_stream(YangVideoEncoderBuffer2* buf,YangFrame* frame, MEDIA_BUFFER stream)
{
    int ret;
    int32_t isKeyframe = YANG_Frametype_P;
    int32_t dataLength;
    uint8_t *data = NULL, *vbuffer = NULL;
    data = RK_MPI_MB_GetPtr(stream);
    dataLength = RK_MPI_MB_GetSize(stream);

    if ( RK_MPI_MB_GetFlag(stream) == VENC_NALU_IDRSLICE) {
        isKeyframe = YANG_Frametype_I;
        frame->frametype = YANG_Frametype_I;
        // yang_trace("put I frame, size:%d\n", dataLength);
    } else {
        isKeyframe = YANG_Frametype_P;
        frame->frametype = YANG_Frametype_P;
        // yang_trace("put P frame, size:%d\n", dataLength);
    }

    frame->dts = frame->pts = RK_MPI_MB_GetTimestamp(stream);
    if (isKeyframe == YANG_Frametype_I) {
        int32_t spsLen=0,ppsLen=0,spsPos=0,ppsPos=0,ipos=0;

        memcpy(frame->payload, data, dataLength);
        vbuffer = frame->payload;
        spsPos=yang_find_pre_start_code(vbuffer,dataLength);
        if(spsPos<0) return 1;
        ppsPos=yang_find_pre_start_code(vbuffer+4+spsPos,dataLength-4-spsPos);
        if(ppsPos<0) return 1;
        ppsPos+=4+spsPos;
        ipos=yang_find_pre_start_code(vbuffer+4+ppsPos,dataLength-4-ppsPos);
        if(ipos<0) return 1;
        ipos+=4+ppsPos;
        spsLen=ppsPos-spsPos-4;
        ppsLen=ipos-ppsPos-4;
        yang_put_be32((char*)vbuffer,(uint32_t)spsLen);
        yang_put_be32((char*)(vbuffer+4+spsLen),(uint32_t)ppsLen);
        frame->nb = dataLength;
    } else {
        int pFramePos = 0;
        pFramePos = yang_find_pre_start_code(data,dataLength);
        memcpy(frame->payload, data + 4, dataLength - 4);
        frame->nb = dataLength - 4;
    }
    buf->putEVideo(&buf->mediaBuffer,frame);
    // yang_trace("\n frame pts:%lld:", frame->pts);
    // for(int k=0;k<60;k++){
    //     yang_trace("%02x,",frame->payload[k]);
    // }
    // yang_trace("\n");

    return 0;
}

void* yang_rkEncoder_start_thread(void *obj)
{
    int val, i, chnNum, ret;
    char stream_path[64];
    MEDIA_BUFFER mb;
    int vencChannel = 0;
    int sendStart = 0;
    CODEC_TYPE_E encType = 0;
    YangEncoderSession* session=(YangEncoderSession*)obj;
    if (ret < 0) {
        yang_error( "IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
        return NULL;
    }

    // 提高线程优先级
    int policy = -1;
    struct sched_param param;
    pthread_getschedparam(pthread_self(),&policy,&param);
    if(policy == SCHED_OTHER)
        yang_trace("SCHED_OTHER 2\n");
    if(policy == SCHED_RR);
        yang_trace("SCHED_RR 2\n");
    if(policy==SCHED_FIFO)
        yang_trace("SCHED_FIFO 2\n");
    yang_trace("pthread priority :%d", param.sched_priority);
    param.sched_priority = 99;
    ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    if(ret == 0) {
        yang_trace("GetVencBuffer(%lu)dynamic set policy %d prio = %d \n", pthread_self(), SCHED_FIFO, param.sched_priority);
    } else {
        yang_trace("GetVencBuffer(%lu)pthread_setschedparam failed.\n", pthread_self());
    }
    ret = pthread_getschedparam(pthread_self(), &policy, &param);
    if(ret == 0) {
        yang_trace("GetVencBuffer(%lu) policy %d prio = %d \n", pthread_self(), policy, param.sched_priority);
    } else {
        yang_trace("GetVencBuffer(%lu)pthread_getschedparam failed.\n",pthread_self());
    }

    YangFrame videoFrame;
    memset(&videoFrame,0,sizeof(YangFrame));
    uint8_t buffer[1024*1024]={0};
    videoFrame.payload=buffer;

    vencChannel = video_get_vencChan();
    encType = video_get_enc_type();
    yang_trace("yang_rkEncoder_start_thread \n");
    while (session->isConvert == 1) {
        /* Get H264 or H265 Stream */
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VENC, vencChannel, 2000);
        if (!mb) {
            yang_error("ERROR: RK_MPI_SYS_GetAIBuffer get null buffer!\n");
            continue;
        }
        if (RK_MPI_MB_GetFlag(mb) == VENC_NALU_IDRSLICE) {
            // yang_trace("IDR frame\n");
            sendStart = 1;
        }
        if (sendStart == 1) {
            if (encType == RK_CODEC_TYPE_H264) {
                ret = yang_rkEncoder_save_stream(session->out_videoBuffer,&videoFrame, mb);
            } else if (encType == RK_CODEC_TYPE_H265) {
                // todo
            }
        }
        RK_MPI_MB_ReleaseBuffer(mb);
    }

    if (ret < 0) {
        yang_error( "RK_MPI_SYS_GetMediaBuffer(%d) failed\n", vencChannel);
        return NULL;
    }
    return NULL;
}

void yang_rkEncoder_start(YangEncoderSession* session)  {
    if(session->isStart) return;
    session->isStart = 1;


    if (pthread_create(&session->threadId, 0, yang_rkEncoder_start_thread, session)) {
        yang_error("YangThread::start could not start thread");
    }
    yang_trace("yang_rkEncoder_start \n");
    session->isStart = 0;
}

void yang_rkEncoder_stop(YangEncoderSession* session) {
    yang_trace("yang_rkEncoder_stop\n");
    session->isConvert = 0;
    video_stop();
}
int32_t yang_rkEncoder_init(YangEncoderSession* session) {
    int32_t ret=0;

    return Yang_Ok;

}

void yang_create_videoEncoder(YangEncoderVideo* encoder){
    YangEncoderSession* session=&encoder->session;

    session->isStart = 0;
    session->isConvert = 1;

    session->out_videoBuffer = NULL;

    encoder->init=yang_rkEncoder_init;
    encoder->start=yang_rkEncoder_start;
    encoder->stop=yang_rkEncoder_stop;
    encoder->sendMsgToEncoder=yang_rkEncoder_sendMsgToEncoder;
    video_init("720");
}

void yang_destroy_videoEncoder(YangEncoderVideo* encoder){
    YangEncoderSession* session=&encoder->session;
    if (session->isConvert) {
    yang_rkEncoder_stop(session);
        while (session->isStart) {
            yang_usleep(1000);
        }
    }

    session->out_videoBuffer = NULL;

}



