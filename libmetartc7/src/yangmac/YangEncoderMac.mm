//
// Copyright (c) 2019-2025 yanggaofeng
//

#import "YangEncoderMac.h"

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CoreVideo.h>
#import <CoreMedia/CoreMedia.h>
#import <VideoToolbox/VideoToolbox.h>

#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

#define yang_success(a,b) if(a!=noErr){yang_error("%s(%d)",b,(int)a);return 1;}

typedef struct{
	yangbool initHeader;
	yangbool sendKeyframe;

	uint32_t width;
	uint32_t height;
	uint32_t fps;
	uint32_t headerLen;
	uint64_t frameCount;

	uint8_t* yuvData;
	uint8_t* data;
	uint8_t* buffer;
	YangCodecCallback* callback;

	VTCompressionSessionRef encoder;
	YangFrame videoFrame;
}YangEncoderMacSession;

static void yang_initHeader(YangEncoderMacSession *session,CMSampleBufferRef sampleBuffer){
	size_t spsLen=0,ppsLen=0;
	size_t spsCount, ppsCount;
	const uint8_t *sps=NULL;
	const uint8_t *pps=NULL;

	CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);

	CMVideoFormatDescriptionGetH264ParameterSetAtIndex(formatDesc, 0, &sps, &spsLen, &spsCount,NULL);
	CMVideoFormatDescriptionGetH264ParameterSetAtIndex(formatDesc, 1, &pps, &ppsLen, &ppsCount,NULL);

	session->headerLen=(uint32_t)spsLen+4+4+(uint32_t)ppsLen;

    yang_put_be32((char*)session->buffer,spsLen);
	yang_memcpy(session->buffer+4,sps, spsLen);
    yang_put_be32((char*)session->buffer+spsLen+4,ppsLen);
	yang_memcpy(session->buffer+spsLen+4+4,pps, ppsLen);
    yang_put_be32((char*)session->buffer+session->headerLen,1);
	session->data=session->buffer+session->headerLen;

	session->initHeader=yangtrue;  
}

static void yang_encodeCallback(void * CM_NULLABLE outputCallbackRefCon,
		void * CM_NULLABLE sourceFrameRefCon,
		OSStatus ret_status,
		VTEncodeInfoFlags infoFlags,
		CM_NULLABLE CMSampleBufferRef sampleBuffer) {
	uint8_t naluType;
	OSStatus status;
	uint32_t nalLength,offset=0;
	size_t len, totalLen;

	char* data;
	CMBlockBufferRef blockBuffer;
	YangEncoderMacSession *session = (YangEncoderMacSession*)outputCallbackRefCon;

	if (ret_status != noErr)
		return;

	if (infoFlags == kVTEncodeInfo_FrameDropped)
		return;

	if (!CMSampleBufferDataIsReady(sampleBuffer)) {
		return;
	}

	if(!session->initHeader)
		yang_initHeader(session,sampleBuffer);

	blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
	status=CMBlockBufferGetDataPointer(blockBuffer, 0, &len, &totalLen, &data);

	while (offset < totalLen) {

		nalLength=yang_get_be32((uint8_t*)data + offset);
		naluType=*(data + offset + 4) & kNalTypeMask;

		if(naluType==YangAvcNaluTypeNonIDR){
			yang_memcpy(session->data+4 ,data + offset + 4, nalLength);
			session->videoFrame.nb=nalLength;
			session->videoFrame.payload=session->data+4;
			session->videoFrame.frametype=YANG_Frametype_P;
			if(session->callback)
				session->callback->onVideoData(session->callback->session,&session->videoFrame);

		}else if(naluType == YangAvcNaluTypeIDR){
			yang_memcpy(session->data+4 ,data + offset + 4, nalLength);
			session->videoFrame.nb=nalLength+4+session->headerLen;
			session->videoFrame.payload=session->buffer;
			session->videoFrame.frametype=YANG_Frametype_I;
			if(session->callback)
				session->callback->onVideoData(session->callback->session,&session->videoFrame);

		}
		offset += 4 + nalLength;
	}
	// yang_trace("\n%d:",session->videoFrame.nb);
	// for(int i=0;i<60;i++)
		// 	yang_trace("%02x,",session->videoFrame.payload[i]);
}

static int32_t yang_ios_enc_init(void* psession,YangVideoInfo* video,YangVideoEncInfo* enc,YangCodecCallback* callback){
	OSStatus status;
	YangEncoderMacSession* session=(YangEncoderMacSession*)psession;
	if(session==NULL)
		return 1;

	session->width=video->outWidth;
	session->height=video->outHeight;
	session->fps=video->frame;
	session->callback=callback;

	status = VTCompressionSessionCreate(NULL,session->width,session->height,kCMVideoCodecType_H264,
			NULL,NULL, NULL, yang_encodeCallback,session, &session->encoder);
	yang_success(status,"VideoToolbox Encoder Session create failed");

    VTSessionSetProperty(session->encoder, kVTCompressionPropertyKey_MaxKeyFrameInterval, (__bridge CFTypeRef)@(enc->gop));
	// 设置码率
    VTSessionSetProperty(session->encoder, kVTCompressionPropertyKey_AverageBitRate, (__bridge CFTypeRef)@(video->rate*1024));
    VTSessionSetProperty(session->encoder, kVTCompressionPropertyKey_DataRateLimits, (__bridge CFTypeRef)@[@(video->rate*1024/8), @1.0]);
	// 设置帧率
	VTSessionSetProperty(session->encoder, kVTCompressionPropertyKey_ExpectedFrameRate, (__bridge CFTypeRef)@(video->frame));
	VTSessionSetProperty(session->encoder, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
	// 禁用 B 帧（防止 Picture Timing SEI）
	VTSessionSetProperty(session->encoder, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
	// 禁用硬件内部码率控制 SEI（部分系统支持）
	VTSessionSetProperty(session->encoder, CFSTR("EnableSEIMessage"), kCFBooleanFalse);

	// 如果上面参数不支持，可以使用 undocumented key 方式：
	CFBooleanRef disableSEI = kCFBooleanTrue;
	VTSessionSetProperty(session->encoder, CFSTR("DisableSEI"), disableSEI);
	VTSessionSetProperty(session->encoder, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_ConstrainedBaseline_AutoLevel);

	status = VTCompressionSessionPrepareToEncodeFrames(session->encoder);
	yang_success(status,"PrepareToEncodeFrames error");

	yang_trace("\nvideoToolBox encoder initialized successfully\n");
	return Yang_Ok;
}

static int32_t yang_ios_enc_encode(void* psession,YangFrame* videoFrame){

	OSStatus status;
	CMTime timeStamp;
	size_t row,strideY,strideUV;

	uint8_t *srcY,*srcUV;
	uint8_t *dstY,*dstUV;
	CVPixelBufferRef buffer=NULL;
	YangEncoderMacSession* session=(YangEncoderMacSession*)psession;

	if(session==NULL || videoFrame==NULL || videoFrame->payload==NULL)
		return 1;

	timeStamp = CMTimeMake(session->frameCount++, session->fps);
	/***
    CVReturn ret = CVPixelBufferCreateWithBytes(NULL, session->width,session->height,
        kCVPixelFormatType_420YpCbCr8Planar,videoFrame->payload, session->width,NULL, NULL,NULL,&buffer);

	 */

	CVReturn ret  = CVPixelBufferCreate(NULL, session->width, session->height, kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, NULL, &buffer);
	if (ret != kCVReturnSuccess) {
		yang_error("create pixel buffer error(%d)", ret);
		return 1;
	}

	CVPixelBufferLockBaseAddress(buffer, 0);

	dstY = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(buffer, 0);
	strideY = CVPixelBufferGetBytesPerRowOfPlane(buffer, 0);
	srcY = videoFrame->payload;
	for (row = 0; row < session->height; ++row) {
		yang_memcpy(dstY + row * strideY, srcY + row * session->width, session->width);
	}

	dstUV = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(buffer, 1);
	strideUV = CVPixelBufferGetBytesPerRowOfPlane(buffer, 1);
	srcUV = (videoFrame->payload + session->width * session->height);
	for (row = 0; row < session->height / 2; ++row) {
		yang_memcpy(dstUV + row * strideUV, srcUV + row * session->width, session->width);
	}

	CVPixelBufferUnlockBaseAddress(buffer, 0);

	CFDictionaryRef frameProperties = NULL;
	if (session->sendKeyframe) {
		const void* keys[] = { kVTEncodeFrameOptionKey_ForceKeyFrame };
		const void* values[] = { kCFBooleanTrue };
		frameProperties = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);
		session->sendKeyframe=yangfalse;
	}

	status = VTCompressionSessionEncodeFrame(session->encoder,  buffer, timeStamp,  kCMTimeInvalid,
			frameProperties, NULL,NULL);

	if (frameProperties) {
		CFRelease(frameProperties);
	}

	if (buffer) {
		CVPixelBufferRelease(buffer);
	}

	yang_success(status,"VideoToolBox encode error");

	return Yang_Ok;
}

static void yang_ios_enc_sendMsgToEncoder(void* psession,YangRequestType request){
	YangEncoderMacSession* session=(YangEncoderMacSession*)psession;
	if(session==NULL)
		return;

	session->sendKeyframe=yangtrue;
}

int32_t yang_create_ios_encoder(YangCVideoEncoder* encoder){
	YangEncoderMacSession* session;
	if(encoder==NULL)
		return 1;

	session=(YangEncoderMacSession*)yang_calloc(sizeof(YangEncoderMacSession),1);
	encoder->session=session;
	session->initHeader=yangfalse;
	session->sendKeyframe=yangfalse;
	session->width=640;
	session->height=480;
	session->fps=25;

	session->data=NULL;
	session->buffer=(uint8_t*)malloc(1024*1024*2);

	encoder->init=yang_ios_enc_init;
	encoder->encode=yang_ios_enc_encode;
	encoder->sendMsgToEncoder=yang_ios_enc_sendMsgToEncoder;
	return Yang_Ok;
}

void yang_destroy_ios_encoder(YangCVideoEncoder* encoder){
	YangEncoderMacSession* session;
	if(encoder==NULL || encoder->session==NULL)
		return;

	session=(YangEncoderMacSession*)encoder->session;
	VTCompressionSessionCompleteFrames(session->encoder, kCMTimeInvalid);
	VTCompressionSessionInvalidate(session->encoder);
	CFRelease(session->encoder);
	session->encoder = NULL;
	yang_free(session->buffer);
	yang_free(encoder->session);
}
