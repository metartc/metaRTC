//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangcapture/win/YangVideoCaptureWindows.h>
#include <yangutil/sys/YangLog.h>
#if Yang_OS_WIN
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <wmcodecdsp.h>
#include <initguid.h>

#include <amvideo.h>
#include <wmcodecdsp.h>
#include <initguid.h>
#include <Propsys.h>
#include <uuids.h>
#include <avrt.h>


#define yang_release(x) { if (x) x->Release(); x = NULL; }



#include <combaseapi.h>

HRESULT WINAPI CopyMediaType(AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource)
{

    *pmtTarget = *pmtSource;
    if (pmtSource->cbFormat != 0) {
   ;
        pmtTarget->pbFormat = (PBYTE)CoTaskMemAlloc(pmtSource->cbFormat);
        if (pmtTarget->pbFormat == NULL) {
            pmtTarget->cbFormat = 0;
            return E_OUTOFMEMORY;
        } else {
            CopyMemory((PVOID)pmtTarget->pbFormat, (PVOID)pmtSource->pbFormat,
                       pmtTarget->cbFormat);
        }
    }
    if (pmtTarget->pUnk != NULL) {
        pmtTarget->pUnk->AddRef();
    }

    return S_OK;
}

YangVideoCaptureWindows::YangVideoCaptureWindows(YangVideoInfo *pcontext) {
	m_para = pcontext;

    m_vhandle = new YangWinVideoCaptureHandle(pcontext);
	cameraIndex = pcontext->vIndex;
	m_width = m_para->width;
	m_height = m_para->height;
	m_vd_id = 0;

	m_isloop = 0;
	m_isFirstFrame = 0;
	m_buffer_count = 0;
	m_timestatmp = 0;
	m_pg = NULL;
	m_pb = NULL;
	m_pm = NULL;
	m_videoSrc = NULL;
	m_grabberF=NULL;
	m_grabber=NULL;
	m_event = NULL;

	m_nullRender=NULL;
	m_t_time=0;


	cameraIndex = 1;
	hasVideo = 1;
	m_preframe = 12;

	m_isOpAddMinus = 0;
	memset(&m_yuy2,0,sizeof(m_yuy2));
	memset(&m_i420,0,sizeof(m_i420));
	memset(&m_nv12,0,sizeof(m_nv12));
	memset(&m_yv12,0,sizeof(m_yv12));
	memset(&m_p010,0,sizeof(m_p010));
	memset(&m_p016,0,sizeof(m_p016));
	m_yuy2.yuvType = YangYuy2;
	m_i420.yuvType = YangI420;
	m_nv12.yuvType = YangNv12;
	m_yv12.yuvType = YangYv12;
	m_p010.yuvType=YangP010;
	m_p016.yuvType=YangP016;


}

YangVideoCaptureWindows::~YangVideoCaptureWindows() {
	if (m_isloop) {
		stop();
		while (m_isStart) {
			yang_usleep(1000);
		}
	}
	yang_release(m_grabber);
	yang_release(m_grabberF);

	yang_release( m_event);
	yang_release( m_pm);
	yang_release( m_pg);
	yang_release( m_pb);
	yang_release( m_videoSrc);
	delete m_vhandle;
	m_vhandle = NULL;


}

int32_t YangVideoCaptureWindows::getVideoCaptureState() {
    return m_vhandle->m_isCapture;
}
int32_t YangVideoCaptureWindows::getLivingVideoCaptureState() {
    return m_vhandle->m_isLivingCaptrue;
}
int32_t YangVideoCaptureWindows::getFilmVideoCaptureState() {
    return m_vhandle->m_isFilm;
}

void YangVideoCaptureWindows::setVideoCaptureStart() {
    m_vhandle->m_isCapture = 1;
}
void YangVideoCaptureWindows::setVideoCaptureStop() {
    m_vhandle->m_isCapture = 0;
}
void YangVideoCaptureWindows::setLivingVideoCaptureStart() {
    m_vhandle->m_isLivingCaptrue = 1;
}
void YangVideoCaptureWindows::setLivingVideoCaptureStop() {
    m_vhandle->m_isLivingCaptrue = 0;
}

void YangVideoCaptureWindows::setFilmVideoCaptureStart() {
    m_vhandle->m_isFilm = 1;
}
void YangVideoCaptureWindows::setFilmVideoCaptureStop() {
    m_vhandle->m_isFilm = 0;
}

void YangVideoCaptureWindows::setOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setVideoBuffer(pbuf);
}
void YangVideoCaptureWindows::setLivingOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setLivingVideoBuffer(pbuf);
}
void YangVideoCaptureWindows::setFilmOutVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setFilmVideoBuffer(pbuf);
}
void YangVideoCaptureWindows::setPreVideoBuffer(YangVideoBuffer *pbuf) {
	m_vhandle->setPreVideoBuffer(pbuf);
}
void YangVideoCaptureWindows::initstamp() {
	m_vhandle->initstamp();
}
int32_t YangVideoCaptureWindows::init() {
	CoInitialize(NULL);

	ICreateDevEnum *devEnum = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
			CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&devEnum));


	IEnumMoniker *classEnum = NULL;

	ULONG cFetched;
    IMoniker  *moniker=NULL;
	int32_t cco = 0;
	devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &classEnum,0);
	while (classEnum->Next(1, &moniker, &cFetched) == S_OK) {
		cco++;
		if (cco != cameraIndex)
			continue;
		moniker->BindToObject(0, 0, IID_IBaseFilter, (void**) &m_videoSrc);
		yang_release(moniker);
		break;
	}
	yang_release(classEnum);
	yang_release(devEnum);

	if (m_videoSrc == NULL)
		return ERROR_SYS_Win_VideoDeveceOpenFailure;

	CoCreateInstance(CLSID_CaptureGraphBuilder2, 0, CLSCTX_INPROC_SERVER,IID_ICaptureGraphBuilder2, (void**) &m_pb);
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,IID_IGraphBuilder, (void**) &m_pg);
	m_pb->SetFiltergraph(m_pg);
	m_pg->QueryInterface(IID_IMediaControl, (void**) &m_pm);
	m_pg->AddFilter(m_videoSrc, L"mp4");
	CLSID CLSID_NullRenderer1 = { 0xC1F400A4, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
	CLSID CLSID_SampleGrabber1 = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
	 hr=CoCreateInstance(CLSID_SampleGrabber1, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&m_grabberF));
		hr = m_pg->AddFilter(m_grabberF, L"Sample Grabber");

		hr = m_grabberF->QueryInterface(IID_ISampleGrabber,(void **)&m_grabber);
        hr=m_pg->QueryInterface(IID_IMediaEventEx,(void **)&m_event);

		m_nullRender=NULL;
		hr=CoCreateInstance(CLSID_NullRenderer1, NULL, CLSCTX_INPROC, IID_IBaseFilter,(void **)&m_nullRender);

		setRevolutionPara();

			//m_yuy2, m_i420 , m_nv12, m_yv12
			if(m_i420.state>1||m_nv12.state>1||m_yv12.state>1||m_yuy2.state>1){
					if(m_i420.state>1) {
						m_para->videoCaptureFormat=YangI420;
					}else if(m_nv12.state>1){
						m_para->videoCaptureFormat=YangNv12;
					}else if(m_yv12.state>1){
						m_para->videoCaptureFormat=YangYv12;
					}else if(m_yuy2.state>1){
						m_para->videoCaptureFormat=YangYuy2;
					}
					#if Yang10bit
					if(m_para->videoCaptureFormat==YangP010) format=V4L2_PIX_FMT_P010;
					if(m_para->videoCaptureFormat==YangP016) format=V4L2_PIX_FMT_P016;
					#endif

				}else if(m_i420.state||m_nv12.state||m_yv12.state||m_yuy2.state){
					if(m_i420.state) {
						setWH(&m_i420);
					}else if(m_nv12.state){
						setWH(&m_nv12);
					}else if(m_yv12.state){
						setWH(&m_yv12);
					}else if(m_yuy2.state){
						setWH(&m_yuy2);
					}

				}else{
					return ERROR_SYS_Win_NoVideoDriver;
				}

			setRevolution();

			if(m_vhandle) m_vhandle->setCaptureFormat(m_para->videoCaptureFormat);
			return Yang_Ok;
}

IPin* YangVideoCaptureWindows::FindPin(IBaseFilter *pFilter, PIN_DIRECTION dir) {
	IEnumPins *pEnumPins;
	IPin *pOutpin;
	PIN_DIRECTION pDir;
	pFilter->EnumPins(&pEnumPins);
	while (pEnumPins->Next(1, &pOutpin, NULL) == S_OK) {
		pOutpin->QueryDirection(&pDir);

		if (pDir == dir) {
            yang_release(pEnumPins);
            return pOutpin;
		}
	}
	return 0;
}
REFGUID YangVideoCaptureWindows::getUID(){
	YangYuvType format=(YangYuvType)m_para->videoCaptureFormat;
	  if(format==YangYuy2) return MEDIASUBTYPE_YUY2;
	  if(format==YangI420) return MEDIASUBTYPE_I420;
	  if(format==YangNv12) return MEDIASUBTYPE_NV12;
	  if(format==YangYv12) return MEDIASUBTYPE_YV12;
	return MEDIASUBTYPE_I420;

}
void YangVideoCaptureWindows::setRevolutionPara() {
	IAMStreamConfig *config1 = 0;
	m_pb->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_videoSrc,
			IID_IAMStreamConfig, (void**) &config1);
	int32_t iCount = 0, iSize = 0;
	HRESULT hr = config1->GetNumberOfCapabilities(&iCount, &iSize);



	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
		for (int32_t iFormat = 0; iFormat < iCount; iFormat++) {
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE *pmtConfig;

			hr = config1->GetStreamCaps(iFormat, &pmtConfig, (BYTE*) &scc);
			VIDEOINFOHEADER* pvih1=(VIDEOINFOHEADER*)pmtConfig->pbFormat;

			if(IsEqualGUID(pmtConfig->subtype, MEDIASUBTYPE_I420)){
				if(!m_i420.state) m_i420.state=1;
				if(pvih1->bmiHeader.biWidth==m_para->width&&pvih1->bmiHeader.biHeight==m_para->height) m_i420.state=2;
				m_i420.width=pvih1->bmiHeader.biWidth;
				m_i420.height=pvih1->bmiHeader.biHeight;
			}else if(IsEqualGUID(pmtConfig->subtype, MEDIASUBTYPE_NV12)){
				if(!m_nv12.state) m_nv12.state=1;
				if(pvih1->bmiHeader.biWidth==m_para->width&&pvih1->bmiHeader.biHeight==m_para->height) m_nv12.state=2;
				m_nv12.width=pvih1->bmiHeader.biWidth;
				m_nv12.height=pvih1->bmiHeader.biHeight;
			}else if(IsEqualGUID(pmtConfig->subtype, MEDIASUBTYPE_YV12)){
				if(!m_yv12.state) m_yv12.state=1;
				if(pvih1->bmiHeader.biWidth==m_para->width&&pvih1->bmiHeader.biHeight==m_para->height) m_yv12.state=2;
				m_yv12.width=pvih1->bmiHeader.biWidth;
				m_yv12.height=pvih1->bmiHeader.biHeight;
			}else if(IsEqualGUID(pmtConfig->subtype, MEDIASUBTYPE_YUY2)){
				if(!m_yuy2.state) m_yuy2.state=1;
				if(pvih1->bmiHeader.biWidth==m_para->width&&pvih1->bmiHeader.biHeight==m_para->height) m_yuy2.state=2;
				m_yuy2.width=pvih1->bmiHeader.biWidth;
				m_yuy2.height=pvih1->bmiHeader.biHeight;
			}
			pvih1=NULL;
		}
	}
}
void YangVideoCaptureWindows::setRevolution() {
	IAMStreamConfig *config1 = 0;

	m_pb->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_videoSrc,IID_IAMStreamConfig, (void**) &config1);
	int32_t iCount = 0, iSize = 0;
	HRESULT hr = config1->GetNumberOfCapabilities(&iCount, &iSize);

// Check the size to make sure we pass in the correct structure.
	YangYuvType format=(YangYuvType)m_para->videoCaptureFormat;

	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
		// Use the video capabilities structure.

		for (int32_t iFormat = 0; iFormat < iCount; iFormat++) {
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE *pmtConfig;
			AM_MEDIA_TYPE amt;
			hr = config1->GetStreamCaps(iFormat, &pmtConfig, (BYTE*) &scc);
			VIDEOINFOHEADER* pvih1=(VIDEOINFOHEADER*)pmtConfig->pbFormat;

				if (IsEqualGUID(pmtConfig->subtype,getUID())&&pvih1->bmiHeader.biWidth==m_para->width&&pvih1->bmiHeader.biHeight==m_para->height) {

						CopyMediaType(&amt, pmtConfig);
						VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER*) amt.pbFormat;
						BITMAPINFOHEADER bmi = pvih->bmiHeader;
						pvih->bmiHeader.biWidth = m_para->width;
						pvih->bmiHeader.biHeight = m_para->height;
						pvih->bmiHeader.biBitCount = format==YangYuy2?16:12;//config.Video_Bit_Count;
						pvih->bmiHeader.biSizeImage = pvih->bmiHeader.biWidth
								* pvih->bmiHeader.biHeight
								* pvih->bmiHeader.biBitCount / 8;
                        yang_info("\nset %d Camera Revolution Sucess!width=%d,height=%d..\n",cameraIndex,m_para->width,m_para->height);
						config1->SetFormat(&amt);
						pvih = NULL;

						return;


				}
				pvih1=NULL;
			}
			//
		}
	//}


}


void YangVideoCaptureWindows::stopLoop() {
	m_isloop = 0;
	if(m_pm!=NULL) m_pm->Stop();
}
void YangVideoCaptureWindows::setWH(YangVideoCaptureType *pct){
	m_width=pct->width;
	m_height=pct->height;
	m_para->width=pct->width;
	m_para->height=pct->height;
	m_para->videoCaptureFormat=pct->yuvType;
}
void YangVideoCaptureWindows::startLoop() {
	if (m_videoSrc == NULL)
		return;

	IPin *pVideoOut = FindPin(m_videoSrc, PINDIR_OUTPUT);
	IPin *pGrabin = FindPin(m_grabberF, PINDIR_INPUT);
	IPin *pGrabout = FindPin(m_grabberF, PINDIR_OUTPUT);
	IPin *pNullIn = FindPin(m_nullRender, PINDIR_INPUT);
	HRESULT hr = m_pg->Connect(pVideoOut, pGrabin);
	hr = m_pg->Connect(pGrabout, pNullIn);
	yang_release(pVideoOut);
	yang_release(pGrabin);
	yang_release(pGrabout);
	yang_release(pNullIn);
	hr = m_grabber->SetBufferSamples(TRUE);
	hr = m_grabber->SetOneShot(FALSE);
	m_grabber->SetCallback(m_vhandle, 1);
	m_pm->Run();
	long eventCode;
	m_event->WaitForCompletion(INFINITE, &eventCode);
}
#endif
