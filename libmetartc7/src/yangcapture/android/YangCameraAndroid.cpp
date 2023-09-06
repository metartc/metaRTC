//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangcapture/android/YangCameraAndroid.h>
#include <yangcapture/YangVideoCaptureHandle.h>
#include <yangutil/sys/YangLog.h>

#if Yang_OS_ANDROID
#include <android/native_window_jni.h>
#include <assert.h>
#include <jni.h>
#include <thread>


void printCamProps(ACameraManager *cameraManager, const char *id)
{
    // exposure range
    ACameraMetadata *metadataObj;
    ACameraManager_getCameraCharacteristics(cameraManager, id, &metadataObj);

    ACameraMetadata_const_entry entry = {0};

    // cam facing
    ACameraMetadata_getConstEntry(metadataObj,
                                  ACAMERA_SENSOR_ORIENTATION, &entry);

    int32_t orientation = entry.data.i32[0];
    yang_trace("camProps: %d", orientation);
}
 std::string getFacingCamId(YangCameraType pcam,ACameraManager *cameraManager)
 {
     ACameraIdList *cameraIds = nullptr;
     ACameraManager_getCameraIdList(cameraManager, &cameraIds);

     std::string backId;

     yang_trace("found camera count %d", cameraIds->numCameras);

     for (int i = 0; i < cameraIds->numCameras; ++i)
     {
         const char *id = cameraIds->cameraIds[i];

         ACameraMetadata *metadataObj;
         ACameraManager_getCameraCharacteristics(cameraManager, id, &metadataObj);

         ACameraMetadata_const_entry lensInfo = {0};
         ACameraMetadata_getConstEntry(metadataObj, ACAMERA_LENS_FACING, &lensInfo);

         auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(
                 lensInfo.data.u8[0]);

         // Found a back-facing camera?
         if (facing == (pcam==YangCameraFront?ACAMERA_LENS_FACING_FRONT:ACAMERA_LENS_FACING_BACK))
             {
                 backId = id;
                 break;
             }
     }

     ACameraManager_deleteCameraIdList(cameraIds);

     return backId;
 }


 void imageCallback(void* context, AImageReader* reader)
  {
	 AImage *image = nullptr;
	 YangVideoCaptureHandle* ch=(YangVideoCaptureHandle*)context;
	 media_status_t status = AImageReader_acquireNextImage(reader, &image);

	 if(AMEDIA_OK!=status||image==nullptr) return;

	 // Try to process data without blocking the callback
	 std::thread processor([=](){
		 uint8_t *data_y = nullptr;
		 uint8_t *data_u = nullptr;
		 uint8_t *data_v = nullptr;
		 int len=0;
		 int ylen=0;
		 int ulen=0;
		 AImage_getPlaneData(image, 0, &data_y, &ylen);
		 AImage_getPlaneData(image, 1, &data_u, &ulen);

		 if(ulen>ylen>>2){
			 ch->putBufferNv21(0, data_y, data_u);
		 }else{
			 AImage_getPlaneData(image, 2, &data_v, &len);
			 ch->putBufferAndroid2(0, data_y, data_u,data_v);
		 }

		 AImage_delete(image);
	 });
	 processor.detach();
  }

  AImageReader* g_yang_createReader(void* user,int width,int height)
  {
	  AImageReader* reader = nullptr;
	  //AIMAGE_FORMAT_YUV_420_888 AIMAGE_FORMAT_JPEG
	  media_status_t status = AImageReader_new(width, height, AIMAGE_FORMAT_YUV_420_888,4, &reader);

	  AImageReader_ImageListener listener;
	  listener.context=user;
	  listener.onImageAvailable=imageCallback;
	  AImageReader_setImageListener(reader, &listener);

	  return reader;
  }

  ANativeWindow* g_yang_createSurface(AImageReader* reader)
  {
      ANativeWindow *nativeWindow;
      AImageReader_getWindow(reader, &nativeWindow);

      return nativeWindow;
  }


void camera_device_on_disconnected(void *context, ACameraDevice *device) {
	yang_trace("Camera(id: %s) is diconnected.\n", ACameraDevice_getId(device));
}

void camera_device_on_error(void *context, ACameraDevice *device, int error) {
	yang_trace("Error(code: %d) on Camera(id: %s).\n", error, ACameraDevice_getId(device));
}

void capture_session_on_ready(void *context, ACameraCaptureSession *session) {
	yang_trace("Session is ready. %p\n", session);
}

void capture_session_on_active(void *context, ACameraCaptureSession *session) {
	yang_trace("Session is activated. %p\n", session);
}

void capture_session_on_closed(void *context, ACameraCaptureSession *session) {
    yang_trace("Session is closed. %p\n", session);
}

/**
 * Capture callbacks
 */

void onCaptureFailed(void* context, ACameraCaptureSession* session,
                     ACaptureRequest* request, ACameraCaptureFailure* failure)
{
    yang_error("onCaptureFailed ");
}

void onCaptureSequenceCompleted(void* context, ACameraCaptureSession* session,
                                int sequenceId, int64_t frameNumber)
{

}


void onCaptureSequenceAborted(void* context, ACameraCaptureSession* session,
                              int sequenceId)
{

}

void onCaptureCompleted (
        void* context, ACameraCaptureSession* session,
        ACaptureRequest* request, const ACameraMetadata* result)
{
    //yang_trace("Capture completed");
}


YangCameraAndroid::YangCameraAndroid(ANativeWindow* pwindow,YangCameraType pcam){
	m_window=pwindow;
	m_cameraType=pcam;
	m_cameraManager=NULL;
	m_cameraDevice=NULL;
	m_captureRequest=NULL;
	m_cameraOutputTarget=NULL;
	m_sessionOutput=NULL;
	m_captureSessionOutputContainer=NULL;
	m_captureSession=NULL;

	m_imageReader=NULL;
	m_imageWindow=NULL;
	m_imageTarget=NULL;
	m_imageOutput=NULL;

	m_width=640;
	m_height=480;
	m_user=NULL;
}
YangCameraAndroid::~YangCameraAndroid(){
	closeCamera();
}

void YangCameraAndroid::setSize(int width,int height){
	m_width=width;
	m_height=height;
}
void YangCameraAndroid::setUser(void* user){
	m_user=user;
}

void YangCameraAndroid::initCamera(){

	ACameraManager *cameraManager = ACameraManager_create();

	std::string id = getFacingCamId(m_cameraType,cameraManager);

	m_deviceStateCallbacks.onDisconnected = camera_device_on_disconnected;
	m_deviceStateCallbacks.onError = camera_device_on_error;
	m_deviceStateCallbacks.context=this;

	ACameraManager_openCamera(cameraManager, id.c_str(), &m_deviceStateCallbacks, &m_cameraDevice);

	ACameraDevice_createCaptureRequest(m_cameraDevice, TEMPLATE_PREVIEW, &m_captureRequest);


	// Prepare outputs for session
	ACaptureSessionOutput_create(m_window, &m_sessionOutput);
	ACaptureSessionOutputContainer_create(&m_captureSessionOutputContainer);
	ACaptureSessionOutputContainer_add(m_captureSessionOutputContainer, m_sessionOutput);

	m_imageReader = g_yang_createReader(m_user,m_width,m_height);
	m_imageWindow = g_yang_createSurface(m_imageReader);
	ANativeWindow_acquire(m_imageWindow);
	ACameraOutputTarget_create(m_imageWindow, &m_imageTarget);
	ACaptureRequest_addTarget(m_captureRequest, m_imageTarget);
	ACaptureSessionOutput_create(m_imageWindow, &m_imageOutput);
	ACaptureSessionOutputContainer_add(m_captureSessionOutputContainer, m_imageOutput);


	// Prepare target surface
	ANativeWindow_acquire(m_window);
	ACameraOutputTarget_create(m_window, &m_cameraOutputTarget);
	ACaptureRequest_addTarget(m_captureRequest, m_cameraOutputTarget);

	m_captureSessionStateCallbacks.onReady = capture_session_on_ready;
	m_captureSessionStateCallbacks.onActive = capture_session_on_active;
	m_captureSessionStateCallbacks.onClosed = capture_session_on_closed;

	// Create the session
	ACameraDevice_createCaptureSession(m_cameraDevice, m_captureSessionOutputContainer, &m_captureSessionStateCallbacks, &m_captureSession);


	m_captureSessionCaptureCallbacks.context = nullptr,
	m_captureSessionCaptureCallbacks.onCaptureStarted = nullptr,
	m_captureSessionCaptureCallbacks.onCaptureProgressed = nullptr,
	m_captureSessionCaptureCallbacks.onCaptureCompleted = onCaptureCompleted,
	m_captureSessionCaptureCallbacks.onCaptureFailed = onCaptureFailed,
	m_captureSessionCaptureCallbacks.onCaptureSequenceCompleted = onCaptureSequenceCompleted,
	m_captureSessionCaptureCallbacks.onCaptureSequenceAborted = onCaptureSequenceAborted,
	m_captureSessionCaptureCallbacks.onCaptureBufferLost = nullptr,
	// Start capturing continuously
	ACameraCaptureSession_setRepeatingRequest(m_captureSession, &m_captureSessionCaptureCallbacks, 1, &m_captureRequest, nullptr);



}

 void YangCameraAndroid::closeCamera(void)
{
	 if (m_cameraManager)
	    {
	        // Stop recording to SurfaceTexture and do some cleanup
	        ACameraCaptureSession_stopRepeating(m_captureSession);
	        ACameraCaptureSession_close(m_captureSession);
	        ACaptureSessionOutputContainer_free(m_captureSessionOutputContainer);
	        ACaptureSessionOutput_free(m_sessionOutput);

	        ACameraDevice_close(m_cameraDevice);
	        ACameraManager_delete(m_cameraManager);
	        m_cameraManager = nullptr;


	        AImageReader_delete(m_imageReader);
	        m_imageReader = nullptr;

	        // Capture request for SurfaceTexture
	        ANativeWindow_release(m_window);
	        ACaptureRequest_free(m_captureRequest);

	}
    yang_info("Close Camera\n");
}


#endif
