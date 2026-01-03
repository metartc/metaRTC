//
// Copyright (c) 2019-2026 yanggaofeng
//

#include "YangDecoderMac.h"
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#import <VideoToolbox/VideoToolbox.h>
#import <CoreImage/CoreImage.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

#include <yangvideo/YangMeta.h>
#include <yangvideo/YangSpspps.h>

#define yang_success(a,b) if(a!=noErr){yang_error("%s(%d)",b,(int)a);return 1;}

typedef struct{
    YangVideoCodec encDecType;
    uint32_t yLen;

    uint32_t videoLen;
    uint32_t width;
    uint32_t height;
    
    uint32_t vpsLen;
    uint32_t spsLen;
    uint32_t ppsLen;

    uint8_t* buffer;
    YangCodecCallback* callback;
    CMVideoFormatDescriptionRef desc;
    VTDecompressionSessionRef decoder;
    YangFrame videoFrame;
}YangDecoderMacSession;

static void yang_decodeCallback(void * CM_NULLABLE decompressionOutputRefCon,
        void * CM_NULLABLE sourceFrameRefCon,
        OSStatus status,
        VTDecodeInfoFlags infoFlags,
        CM_NULLABLE CVImageBufferRef imageBuffer,
        CMTime presentationTimeStamp,
        CMTime presentationDuration) {
    CVPixelBufferRef buffer;
    uint8_t* yData;
    uint8_t* uvData;
    YangDecoderMacSession *session = (YangDecoderMacSession *)decompressionOutputRefCon;
    if (status != noErr) {
        yang_error("decode callback received error(%d)", status);
        return;
    }

    if (!imageBuffer) {
        yang_error("decode callback image buffer is null");
        return;
    }


    buffer = (CVPixelBufferRef)imageBuffer;
    CVPixelBufferLockBaseAddress(buffer, kCVPixelBufferLock_ReadOnly);
    yData = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(buffer, 0);
    uvData = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(buffer, 1);
    CVPixelBufferUnlockBaseAddress(buffer, kCVPixelBufferLock_ReadOnly);

    if(session->callback && yData && uvData){
        yang_memcpy(session->buffer,yData,session->yLen);
        yang_memcpy(session->buffer+session->yLen,uvData,session->yLen/2);
        session->videoFrame.payload=session->buffer;
        session->videoFrame.nb=session->videoLen;
        session->callback->onVideoData(session->callback->session,&session->videoFrame);
    }
}

static int32_t yang_ios_dec_init(void* psession,YangCodecCallback* callback){
    YangDecoderMacSession* session=(YangDecoderMacSession*)psession;
    if(session==NULL)
        return 1;

    session->callback=callback;
    return Yang_Ok;
}

static int32_t yang_ios_initDec(YangDecoderMacSession* session,YangH2645Conf *h2645Conf){
    const uint8_t* headers[] = {h2645Conf->sps, h2645Conf->pps};
    const size_t headerLens[] = {(size_t)h2645Conf->spsLen, (size_t)h2645Conf->ppsLen};
    OSStatus status = CMVideoFormatDescriptionCreateFromH264ParameterSets(
            NULL, 2, headers, headerLens, 4, &session->desc);
    yang_success(status,"VideoToolBox decoder set sps/pps error");

    uint32_t format = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
    const void *keys[] = { kCVPixelBufferPixelFormatTypeKey };
    const void *values[] = { CFNumberCreate(NULL, kCFNumberSInt32Type, &format) };
    CFDictionaryRef property = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);

    VTDecompressionOutputCallbackRecord callback=  {
            .decompressionOutputCallback = yang_decodeCallback,
            .decompressionOutputRefCon = session,
    };

    status = VTDecompressionSessionCreate(NULL, session->desc,
                                          property, NULL,&callback, &session->decoder);
    if(property)
        CFRelease(property);
        
    yang_success(status,"VideoToolBox create decoder session error");

    yang_trace("\nvideoToolBox decoder initialized successfully\n");
    return Yang_Ok;
}

static void yang_ios_dec_parseHeader(void* psession,uint8_t *p, int32_t  pLen, int32_t  *pwid,
        int32_t  *phei, int32_t  *pfps){
    uint8_t spsHeader[128];
    YangH2645Conf h2645Conf;

    YangDecoderMacSession* session=(YangDecoderMacSession*)psession;
    if(session==NULL)
        return;

    yang_memset(spsHeader, 0, 128);
    yang_memset(&h2645Conf,0,sizeof(h2645Conf));

    if(session->encDecType==Yang_VED_H264){
       yang_meta_createH264(&h2645Conf,p,pLen);
       yang_memcpy(spsHeader,h2645Conf.sps,h2645Conf.spsLen);
       yang_h264_decode_spspps(spsHeader,h2645Conf.spsLen,pwid,phei,pfps);
    }

    session->spsLen=h2645Conf.spsLen;
    session->ppsLen=h2645Conf.ppsLen;
    session->width=*pwid;
    session->height=*phei;
    session->videoLen=session->width*session->height*3/2;
    session->yLen=session->width*session->height;
    if(session->buffer==NULL)
        session->buffer=(uint8_t*)malloc(session->videoLen);
    yang_ios_initDec(session,&h2645Conf);
}

static int32_t yang_ios_dec_decode(void* psession,YangFrame* videoFrame){
    OSStatus status;
    CMBlockBufferRef blockBuffer = NULL;
    CMBlockBufferFlags blockFlags = 0;

    YangDecoderMacSession* session=(YangDecoderMacSession*)psession;
    if(session==NULL)
        return 1;

    if(videoFrame->frametype==YANG_Frametype_P){
            yang_put_be32((char*)videoFrame->payload,(uint32_t)videoFrame->nb-4);
    }else if (videoFrame->frametype==YANG_Frametype_I){
        if(session->encDecType==Yang_VED_H264){
            yang_put_be32((char*)videoFrame->payload,session->spsLen);
            yang_put_be32((char*)videoFrame->payload+session->spsLen+4,session->ppsLen);
            yang_put_be32((char*)videoFrame->payload+session->spsLen+session->ppsLen+8,videoFrame->nb-session->spsLen-session->ppsLen-12);
        }
    }

    status = CMBlockBufferCreateWithMemoryBlock(NULL,
            (void*)videoFrame->payload, // data
            videoFrame->nb, // block length
            kCFAllocatorNull, // **Important! can't be Null**
            NULL,
            0, // Offset
            videoFrame->nb, // data length
            blockFlags,
            &blockBuffer);

    if (status != kCMBlockBufferNoErr || blockBuffer == NULL) {
        yang_error("create block buffer failed(%d)", status);
        CFRelease(blockBuffer);
        return 1;
    }

    CMSampleBufferRef sampleBuffer = NULL;
    const size_t sampleSizeArray[] = { (size_t)videoFrame->nb };
    status = CMSampleBufferCreateReady(NULL,
            blockBuffer,
            session->desc,
            1, // Sample num
            0,
            NULL,
            1,
            sampleSizeArray,
            &sampleBuffer);

    if (status != kCMBlockBufferNoErr || sampleBuffer == NULL) {
        yang_error("create sample buffer failed(%d)", status);
        CFRelease(blockBuffer);
        CFRelease(sampleBuffer);
        return 1;
    }

    VTDecodeFrameFlags frameFlags = 0; // 默认是同步回调
    VTDecodeInfoFlags outFlags = 0; // 输出的 flags
    status = VTDecompressionSessionDecodeFrame(session->decoder,
            sampleBuffer,
            frameFlags,
            NULL,
            &outFlags);

    if (status != noErr || sampleBuffer == NULL) {
        yang_error("decode sample buffer failed(%d)", status);
        CFRelease(blockBuffer);
        CFRelease(sampleBuffer);
        return 1;
    }

    CFRelease(blockBuffer);
    CFRelease(sampleBuffer);
    return Yang_Ok;
}


int32_t yang_create_ios_decoder(YangCVideoDecoder* decoder){
    YangDecoderMacSession* session;
    if(decoder==NULL)
        return 1;

    session=(YangDecoderMacSession*)yang_calloc(sizeof(YangDecoderMacSession),1);
    decoder->session=session;

    session->buffer=NULL;
    session->desc=NULL;
    session->decoder=NULL;

    session->encDecType=Yang_VED_H264;

    decoder->init=yang_ios_dec_init;
    decoder->parseHeader=yang_ios_dec_parseHeader;
    decoder->decode=yang_ios_dec_decode;
    return Yang_Ok;
}

void yang_destroy_ios_decoder(YangCVideoDecoder* decoder){
    YangDecoderMacSession* session;
    if(decoder==NULL || decoder->session==NULL)
        return;

    session=(YangDecoderMacSession*)decoder->session;

    if (session->decoder) {
        VTDecompressionSessionInvalidate(session->decoder);
        CFRelease(session->decoder);
        session->decoder=NULL;
    }

    if (session->desc) {
        CFRelease(session->desc);
        session->desc=NULL;
    }
    yang_free(session->buffer);
    yang_free(decoder->session);
}


