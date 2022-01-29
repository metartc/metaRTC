#include <yangaudiodev/win/YangWinAudioApi.h>
#ifdef _WIN32
#include <assert.h>

#include <endpointvolume.h>
#include <audioclient.h>
#include <uuids.h>
#include <yangutil/sys/YangLog.h>
#define SAFE_RELEASE(x) if(x){x->Release();x=NULL;}

#define Yang_Release(x) if(x){x->Release();x=NULL;}
#define EXIT_ON_ERROR(hres) \
    do {                      \
    if (FAILED(hres))       \
    goto Exit;            \
    } while (0)


YangWinAudioApi::YangWinAudioApi()
{
    m_enum=NULL;
    CoInitialize(NULL); //初始化COM库
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                     __uuidof(IMMDeviceEnumerator),
                     reinterpret_cast<void**>(&m_enum));

}
YangWinAudioApi::~YangWinAudioApi()
{
     Yang_Release(m_enum);

}

int YangWinAudioApi::getListDevice(IMMDeviceEnumerator* penum,EDataFlow dir,int index,IMMDevice** ppDevice) {
    HRESULT hr(S_OK);
    IMMDeviceCollection* pCollection = NULL;

    hr = penum->EnumAudioEndpoints(
                dir,
                DEVICE_STATE_ACTIVE,  // only active endpoints are OK
                &pCollection);
    if (FAILED(hr)) {

        SAFE_RELEASE(pCollection);
        return 1;
    }

    hr = pCollection->Item(index, ppDevice);
    if (FAILED(hr)) {

        SAFE_RELEASE(pCollection);
        return 1;
    }

    return 0;
}
int YangWinAudioApi::getDeviceListCount(IMMDeviceCollection* pcollection,EDataFlow dir){
    HRESULT hr = S_OK;
    UINT count = 0;

        hr = pcollection->GetCount(&count);


    return static_cast<int>(count);
}
int YangWinAudioApi::getDeviceID(IMMDevice* pDevice, LPWSTR pszBuffer,int bufferLen) {


  static const WCHAR szDefault[] = L"<Device not available>";

  HRESULT hr = E_FAIL;
  LPWSTR pwszID = NULL;

  assert(pszBuffer != NULL);
  assert(bufferLen > 0);

  if (pDevice != NULL) {
    hr = pDevice->GetId(&pwszID);
  }

  if (hr == S_OK) {
    // Found the device ID.
    wcsncpy_s(pszBuffer, bufferLen, pwszID, _TRUNCATE);
  } else {
    // Failed to find the device ID.
    wcsncpy_s(pszBuffer, bufferLen, szDefault, _TRUNCATE);
  }

  CoTaskMemFree(pwszID);
  return 0;
}
int YangWinAudioApi::getDefaultDeviceID( IMMDeviceEnumerator* _ptrEnumerator,EDataFlow dir,ERole role,LPWSTR szBuffer,int bufferLen) {


  HRESULT hr = S_OK;
  IMMDevice* pDevice = NULL;

  hr = _ptrEnumerator->GetDefaultAudioEndpoint(dir, role, &pDevice);

  if (FAILED(hr)) {

    SAFE_RELEASE(pDevice);
    return 1;
  }

  int32_t res = getDeviceID(pDevice, szBuffer, bufferLen);
  SAFE_RELEASE(pDevice);
  return res;
}

int YangWinAudioApi::getDefaultDeviceIndex(IMMDeviceEnumerator* penum,EDataFlow dir, ERole role,int* index) {
     HRESULT hr = S_OK;


  WCHAR szDefaultDeviceID[MAX_PATH] = {0};
  WCHAR szDeviceID[MAX_PATH] = {0};

  const size_t kDeviceIDLength = sizeof(szDeviceID) / sizeof(szDeviceID[0]);
  assert(kDeviceIDLength ==
         sizeof(szDefaultDeviceID) / sizeof(szDefaultDeviceID[0]));

  if (getDefaultDeviceID(penum,dir, role, szDefaultDeviceID, kDeviceIDLength)) {
	  yang_error ( "getDefaultDeviceI r failed hr==%ld",hr);
    return 1;
  }

  IMMDeviceCollection* collection;
  hr = penum->EnumAudioEndpoints(dir, DEVICE_STATE_ACTIVE,
                                            &collection);

  if (!collection) {
	  yang_error ( "Device collection not valid");
    return 1;
  }

  UINT count = 0;
  hr = collection->GetCount(&count);
  if (FAILED(hr)) {
	  yang_error("..........collection GetCount failed================hr==%ld",hr);
    return 1;
  }

  *index = -1;
  for (UINT i = 0; i < count; i++) {
    memset(szDeviceID, 0, sizeof(szDeviceID));

    IMMDevice* device;
      {
      IMMDevice* ptrDevice = NULL;
      hr = collection->Item(i, &ptrDevice);
      if (FAILED(hr) || ptrDevice == NULL) {
          yang_error("..........collection Item failed==================hr==%ld",hr);
        return 1;
      }
      device = ptrDevice;
      SAFE_RELEASE(ptrDevice);
    }

    if (getDeviceID(device, szDeviceID, kDeviceIDLength)) {
        yang_error("..........getDeviceID failed====================hr==%ld",hr);
      return 1;
    }

    if (wcsncmp(szDefaultDeviceID, szDeviceID, kDeviceIDLength) == 0) {
      // Found a match.
      *index = i;
      break;
    }
  }

  if (*index == -1) {
	  yang_error( "Unable to find collection index for default device=hr==%ld",hr);
    return 1;
  }
   SAFE_RELEASE(collection);


  return 0;
}
#endif
