//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGWINAUDIODEVICE_H
#define YANGWINAUDIODEVICE_H
#ifdef _WIN32
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <dmo.h>
#include <mutex>
const float MAX_SPEAKER_VOLUME = 255.0f;
const float MIN_SPEAKER_VOLUME = 0.0f;
const float MAX_MICROPHONE_VOLUME = 255.0f;
const float MIN_MICROPHONE_VOLUME = 0.0f;
class YangWinAudioDevice
{
public:
    YangWinAudioDevice();
    virtual  ~YangWinAudioDevice();
    void init();
    void startBuiltInAec();
    int32_t getDeviceListCount(EDataFlow dir);
   int32_t playoutDeviceCount();
    int32_t enumEndpointDevicesAll(EDataFlow dataFlow);
    int32_t getDeviceID(IMMDevice* pDevice,LPWSTR pszBuffer, int32_t bufferLen);
    int32_t getDefaultDeviceID(EDataFlow dir, ERole role,LPWSTR szBuffer,int32_t bufferLen);
    int32_t getDefaultDevice(EDataFlow dir,ERole role,IMMDevice** ppDevice);
    int32_t getDefaultDeviceIndex(EDataFlow dir,ERole role,int* index);

    int32_t initCapture();
    int32_t initMicrophone(int32_t pind);
    int32_t initSpeaker(int32_t pind);
    int32_t initPlay();
    int32_t getListDevice(EDataFlow dir,int32_t index,IMMDevice** ppDevice);

    int32_t getMicrophoneVolume(int& volume);
    int32_t setMicrophoneVolume(int32_t volume);
    int32_t setMicrophoneMute(bool enable);

    int32_t getSpeakerVolume(int& volume);
    int32_t setSpeakerVolume(int32_t volume);
    int32_t getSpeakerMute(bool& enabled);
    int32_t setSpeakerMute(bool enable);

    void captureThread();
    int32_t playThread();
private:
    IMMDeviceEnumerator* m_enum;
    IMMDeviceCollection* m_renderCollection;
    IMMDeviceCollection* m_captureCollection;
    IMMDevice* m_deviceOut;
    IMMDevice* m_deviceIn;

    IMediaObject* m_dmo;

    IAudioClient* m_clientOut;
    IAudioClient* m_clientIn;
    IAudioRenderClient* m_renderClient;
    IAudioCaptureClient* m_captureClient;
    IAudioEndpointVolume* m_captureVolume;
    ISimpleAudioVolume* m_renderSimpleVolume;

   boolean _builtInAecEnabled;
double _perfCounterFactor;
uint32_t _sndCardPlayDelay;
int32_t _inputDeviceIndex=0,_outputDeviceIndex=0;
UINT64 _writtenSamples;
UINT64 _readSamples;

 UINT _playAudioFrameSize;
uint32_t _playSampleRate;
uint32_t _devicePlaySampleRate;
uint32_t _playBlockSize;
uint32_t _devicePlayBlockSize;
uint32_t _playChannels;

UINT _recAudioFrameSize;
uint32_t _recSampleRate;
uint32_t _recBlockSize;
uint32_t _recChannels;
    std::mutex m_lock;
private:
    int32_t initRecordingDMO();
    int32_t setDMOProperties();
    int32_t setBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key,VARIANT_BOOL value);
    int32_t setVtI4Property(IPropertyStore* ptrPS, REFPROPERTYKEY key,LONG value);

    HANDLE _hRenderSamplesReadyEvent;
    HANDLE _hPlayThread;
    HANDLE _hRenderStartedEvent;
    HANDLE _hShutdownRenderEvent;

    HANDLE _hCaptureSamplesReadyEvent;
    HANDLE _hRecThread;
    HANDLE _hCaptureStartedEvent;
    HANDLE _hShutdownCaptureEvent;

LARGE_INTEGER _perfCounterFreq;
      uint16_t _recChannelsPrioList[3];
       uint16_t _playChannelsPrioList[2];

       bool _initialized;
       bool _recording;
       bool _playing;
       bool _recIsInitialized;
       bool _playIsInitialized;
       bool _speakerIsInitialized;
       bool _microphoneIsInitialized;

};
#endif
#endif // YANGWINAUDIODEVICE_H
