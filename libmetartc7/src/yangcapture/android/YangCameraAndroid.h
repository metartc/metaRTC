//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef METAPUSHSTREAM3_YANGCAMERAANDROID_H
#define METAPUSHSTREAM3_YANGCAMERAANDROID_H
#if Yang_OS_ANDROID
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraManager.h>
#include <media/NdkImageReader.h>
#include <EGL/egl.h>

typedef enum{
	YangCameraFront,
	YangCameraBack
}YangCameraType;

class YangCameraAndroid{
public:
    YangCameraAndroid(ANativeWindow* pwindow,YangCameraType pcam);
    ~YangCameraAndroid();
    void setSize(int width,int height);
    void setUser(void* user);
    void initCamera();
private:
    YangCameraType m_cameraType;

    void closeCamera(void);

private:
    int32_t m_width;
    int32_t m_height;
    void* m_user;

    ACameraManager* m_cameraManager;
    ANativeWindow *m_window;
    ACameraDevice *m_cameraDevice;
    ACaptureRequest *m_captureRequest;
    ACameraOutputTarget *m_cameraOutputTarget;
    ACaptureSessionOutput *m_sessionOutput;
    ACaptureSessionOutputContainer *m_captureSessionOutputContainer;
    ACameraCaptureSession *m_captureSession;

    ACameraDevice_StateCallbacks m_deviceStateCallbacks;
    ACameraCaptureSession_stateCallbacks m_captureSessionStateCallbacks;
    ACameraCaptureSession_captureCallbacks m_captureSessionCaptureCallbacks;

    AImageReader* m_imageReader;
    ANativeWindow* m_imageWindow;
    ACameraOutputTarget* m_imageTarget;
    ACaptureSessionOutput* m_imageOutput;


};
#endif
#endif //METAPUSHSTREAM3_YANGCAMERAANDROID_H
