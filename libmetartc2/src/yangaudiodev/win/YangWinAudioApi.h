#ifndef YangWinAudioApi_H
#define YangWinAudioApi_H
#ifdef _WIN32
#include "windows.h"
#include <mmdeviceapi.h>

#include <stdint.h>

class YangWinAudioApi
{
public:
    YangWinAudioApi();
virtual ~YangWinAudioApi();
protected:
    HRESULT enumEndpointDevicesAll(IMMDeviceEnumerator* penum,EDataFlow dataFlow);
    int getDefaultDeviceID( IMMDeviceEnumerator* penum,EDataFlow dir,ERole role,LPWSTR szBuffer,int bufferLen);
    int getDeviceID(IMMDevice* pDevice, LPWSTR pszBuffer,int bufferLen);
    int getDefaultDeviceIndex(IMMDeviceEnumerator* penum,EDataFlow dir,  ERole role, int* index);
    int getDeviceListCount(IMMDeviceCollection* pcollection,EDataFlow dir);
    int getListDevice(IMMDeviceEnumerator* penum,EDataFlow dir,int index,IMMDevice** ppDevice);
protected:
      IMMDeviceEnumerator* m_enum;
};
#endif
#endif // YangWinAudioApi_H
