//
// Copyright (c) 2019-2023 yanggaofeng
//
#include "YangVideoDeviceMac.h"
#if Yang_OS_APPLE

#import <CoreFoundation/CFString.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>
//#include <CoreMediaIO/CMIOHardware.h>
#include <yangutil/sys/YangLog.h>

struct YangMacVideoSession;


@interface YangAVCaptureDelegate
: NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
      @public
      YangMacVideoSession *m_videoSession;
}
- (void)captureOutput:(AVCaptureOutput *)out
  didDropSampleBuffer:(CMSampleBufferRef)sampleBuffer
  fromConnection:(AVCaptureConnection *)connection;
- (void)captureOutput:(AVCaptureOutput *)captureOutput
  didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
  fromConnection:(AVCaptureConnection *)connection;
@end

struct YangMacVideoSession{
   AVCaptureSession *session;
   AVCaptureVideoDataOutput *out;
   AVCaptureInput* capture_input;
   AVCaptureDevice *video_device;
   yangbool isInited;
   uint32_t width;
   uint32_t height;
   uint32_t framerate;

   uint32_t vindex;

   YangAVCaptureDelegate*    delegate;
   uint32_t yLen;
   uint32_t uvLen;
   uint32_t bufferLen;
   uint8_t* buffer;
   YangMacVideoCallback callback;
};

@implementation YangAVCaptureDelegate
- (void)captureOutput:(AVCaptureOutput *)out
  didDropSampleBuffer:(CMSampleBufferRef)sampleBuffer
  fromConnection:(AVCaptureConnection *)connection
{
    (void)out;
    (void)sampleBuffer;
    (void)connection;
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput
  didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
  fromConnection:(AVCaptureConnection *)connection
{
    (void)captureOutput;
    (void)connection;
    int32_t kFlags = 0;

    CMItemCount count = CMSampleBufferGetNumSamples(sampleBuffer);

    if (count < 1 || !m_videoSession)
        return;

    CMTime pts =
            CMSampleBufferGetOutputPresentationTimeStamp(sampleBuffer);
    CMTime pts_micro = CMTimeConvertScale(
                pts, 1000*1000, kCMTimeRoundingMethod_Default);

    CVImageBufferRef img = CMSampleBufferGetImageBuffer(sampleBuffer);
    if (CVPixelBufferLockBaseAddress(img, kFlags) != kCVReturnSuccess) {
        return;
    }

   // uint8_t* baseAddress = (uint8_t*)CVPixelBufferGetBaseAddress(img);
  //  uint32_t width = CVPixelBufferGetWidth(img);
    //uint32_t height = CVPixelBufferGetHeight(img);


    // uint32_t ylen = CVPixelBufferGetBytesPerRowOfPlane(img, 0);
    // uint32_t uvlen = CVPixelBufferGetBytesPerRowOfPlane(img, 1);

     uint8_t *y_data = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(img, 0);
     uint8_t *uv_data = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(img, 1);

     memcpy(m_videoSession->buffer,y_data,m_videoSession->yLen);
     memcpy(m_videoSession->buffer+m_videoSession->yLen,uv_data,m_videoSession->uvLen);

    if(m_videoSession->callback.on_video)
        m_videoSession->callback.on_video(m_videoSession->buffer,m_videoSession->bufferLen,pts_micro.value,m_videoSession->callback.user);

    CVPixelBufferUnlockBaseAddress(img, kFlags);

}
@end



void yang_macv_enumdevices(YangMacVideoSession* session){
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    int32_t index=0;
    for (AVCaptureDevice *device in devices) {
        const char *name = [[device localizedName] UTF8String];
        index            = [devices indexOfObject:device];
        yang_trace("[%d] %s\n", index, name);
    }
}

void yang_macv_enumpresets(YangMacVideoSession* session){

    if([session->session canSetSessionPreset:AVCaptureSessionPreset320x240]){
        yang_trace("video capture session support 320x240");
    }
    if([session->session canSetSessionPreset:AVCaptureSessionPreset352x288]){
        yang_trace("video capture session support 352x288");
    }
    if([session->session canSetSessionPreset:AVCaptureSessionPreset640x480]){
        yang_trace("video capture session support 640x480");
    }

    if([session->session canSetSessionPreset:AVCaptureSessionPreset960x540]){
        yang_trace("video capture session support 960x540");
    }
    if([session->session canSetSessionPreset:AVCaptureSessionPreset1280x720]){
        yang_trace("video capture session support 1280x720");
    }
    if([session->session canSetSessionPreset:AVCaptureSessionPreset1920x1080]){
        yang_trace("video capture session support 1920x1080");
    }
    if([session->session canSetSessionPreset:AVCaptureSessionPreset3840x2160]){
        yang_trace("video capture session support 3840x2160");
    }

}
void yang_macv_device_enumpresets(YangMacVideoSession* session){

    if([session->video_device supportsAVCaptureSessionPreset:AVCaptureSessionPreset320x240]){
        yang_trace("video capture device support 320x240");
    }
    if([session->video_device supportsAVCaptureSessionPreset:AVCaptureSessionPreset352x288]){
        yang_trace("video capture device support 352x288");
    }
    if([session->video_device supportsAVCaptureSessionPreset:AVCaptureSessionPreset640x480]){
        yang_trace("video capture device support 640x480");
    }

    if([session->video_device supportsAVCaptureSessionPreset:AVCaptureSessionPreset960x540]){
        yang_trace("video capture device support 960x540");
    }
    if([session->video_device supportsAVCaptureSessionPreset:AVCaptureSessionPreset1280x720]){
        yang_trace("video capture device support 1280x720");
    }
    if([session->video_device supportsAVCaptureSessionPreset:AVCaptureSessionPreset1920x1080]){
        yang_trace("video capture device support 1920x1080");
    }
    if([session->video_device supportsAVCaptureSessionPreset:AVCaptureSessionPreset3840x2160]){
        yang_trace("video capture device support 3840x2160");
    }


}

AVCaptureSessionPreset yang_macv_getPreset(uint32_t width,uint32_t height){
    if(width==320&&height==240) return AVCaptureSessionPreset320x240;
    if(width==640&&height==480) return AVCaptureSessionPreset640x480;
    if(width==352&&height==288) return AVCaptureSessionPreset352x288;
    if(width==960&&height==540) return AVCaptureSessionPreset960x540;
    if(width==1280&&height==720) return AVCaptureSessionPreset1280x720;
    if(width==1920&&height==1080) return AVCaptureSessionPreset1920x1080;
    if(width==3840&&height==2160) return AVCaptureSessionPreset3840x2160;
    return AVCaptureSessionPresetHigh;

}


void yang_macv_getFormat(YangMacVideoSession* session){
    AVCaptureDeviceFormat *format = session->video_device.activeFormat;
    CMVideoDimensions vm=CMVideoFormatDescriptionGetDimensions(format.formatDescription);
    yang_trace("get active format width=%u,height=%u",vm.width,vm.height);
}

int32_t yang_macv_uninit(YangMacVideoSession* session){
    if(!session->isInited) return 1;
    [session->session removeInput:session->capture_input];
    return Yang_Ok;
}

int32_t yang_macv_init(void* psession){
    YangMacVideoSession* session=(YangMacVideoSession*)psession;
    if(session==NULL||session->isInited) return 1;
/**
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (authStatus == AVAuthorizationStatusRestricted ||authStatus == AVAuthorizationStatusDenied) {
        yang_error("camera is restricted!");
        return 1;
    }**/

    session->session = [[AVCaptureSession alloc] init];

    NSError* error=nil;
    yang_macv_enumpresets(session);

    AVCaptureSessionPreset preset=yang_macv_getPreset(session->width,session->height);

    if([session->session canSetSessionPreset:preset]){
        [session->session setSessionPreset:preset];
        yang_trace("video capture set %ux%u sucess!",session->width,session->height);
    }else{
        [session->session setSessionPreset:AVCaptureSessionPresetLow];
        yang_error("video capture device not support %ux%u",session->width,session->height);
    }

    session->video_device=[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    if(!session->video_device){
        session->isInited=yangfalse;
        yang_error("create AV capture  device create fail: %s",
                   [[error localizedDescription] UTF8String]);
        return 1;
    }

    yang_macv_device_enumpresets(session);


    session->capture_input = (AVCaptureInput*) [[[AVCaptureDeviceInput alloc] initWithDevice: session->video_device error:&error] autorelease];


    if (!session->capture_input) {
        yang_error("create AV capture input device fail: %s\n",
                   [[error localizedDescription] UTF8String]);
        return 1;
    }

    if ([session->session canAddInput:session->capture_input]) {
        [session->session addInput:session->capture_input];
    } else {
        yang_error("can't add video input to capture session");
        return 1;
    }

    session->out = [[AVCaptureVideoDataOutput alloc] init];

    if (!session->out) {
        yang_error("init AV video output fail");
        return 1;
    }




    session->delegate = [[YangAVCaptureDelegate alloc] init];
    session->delegate->m_videoSession = session;

    dispatch_queue_t queue = dispatch_queue_create("metaRTC", NULL);
    [session->out setSampleBufferDelegate:session->delegate
                                          queue:queue];
    dispatch_release(queue);


   /** NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
    //kCVPixelFormatType_422YpCbCr8BiPlanarFullRange kCVPixelFormatType_420YpCbCr8BiPlanarFullRange
    NSNumber* val = [NSNumber
            numberWithUnsignedInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange];
    NSDictionary* videoSettings =
            [NSDictionary dictionaryWithObject:val forKey:key];
    session->out.videoSettings = videoSettings;**/

    NSDictionary *videoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                  [NSNumber numberWithDouble:session->width], (id)kCVPixelBufferWidthKey,
                                  [NSNumber numberWithDouble:session->height], (id)kCVPixelBufferHeightKey,
                                  [NSNumber numberWithUnsignedInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange], (id)kCVPixelBufferPixelFormatTypeKey,
                                  nil];
    [session->out setVideoSettings:videoSettings];

    if ([session->session canAddOutput:session->out]) {
        [session->session addOutput:session->out];

    } else {
        yang_error("video output to capture session fail");
        return 1;
    }
    session->isInited=yangtrue;
    return Yang_Ok;
}


int32_t yang_macv_start(void* psession){
    YangMacVideoSession* session=(YangMacVideoSession*)psession;
    if(session==NULL) return 1;
    if(!session->isInited) return 1;
    [session->session startRunning];
    return Yang_Ok;
}


int32_t yang_macv_stop(void* psession){
    YangMacVideoSession* session=(YangMacVideoSession*)psession;
    if(session==NULL) return 1;
      if(!session->isInited) return 1;
    [session->session stopRunning];
    return Yang_Ok;
}

int32_t yang_create_macVideo(YangVideoDeviceMac* video,uint32_t width,uint32_t height,uint32_t framerate,YangMacVideoCallback* callback){
    if(video==NULL) return 1;
    YangMacVideoSession* session=(YangMacVideoSession*)calloc(sizeof(YangMacVideoSession),1);
    video->session=session;

    session->width=width;
    session->height=height;
    session->framerate=framerate;
    session->yLen=width*height;
    session->uvLen=width*height/2;
    session->bufferLen=width*height*3/2;
    session->buffer=(uint8_t*)yang_malloc(session->bufferLen);

    session->callback.user=callback->user;
    session->callback.on_video=callback->on_video;

    video->init=yang_macv_init;
    video->start=yang_macv_start;
    video->stop=yang_macv_stop;
    return Yang_Ok;
}
void yang_destroy_macVideo(YangVideoDeviceMac* video){
    YangMacVideoSession* session;
    if(video==NULL||video->session==NULL)
        return;
    session=(YangMacVideoSession*)video->session;
    yang_macv_uninit(session);
    yang_free(session->buffer);
    yang_free(video->session);
}

#endif
