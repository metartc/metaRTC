//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifdef _MSC_VER

#include <windows.h>
#include <atlbase.h>
#include <DXGITYPE.h>
#include <DXGI1_2.h>
#include <d3d11.h>
#include <Wincodec.h>
#include <vector>

using namespace std;

class YangDXGIPointerInfo;

enum YangCaptureSource
{
	CSUndefined,
	CSMonitor1,
	CSMonitor2,
	CSDesktop
};

class YangDXGIPointerInfo
{
public:
	YangDXGIPointerInfo(BYTE* pPointerShape, UINT uiPointerShapeBufSize, DXGI_OUTDUPL_FRAME_INFO fi, DXGI_OUTDUPL_POINTER_SHAPE_INFO psi);
	~YangDXGIPointerInfo();
	BYTE* GetBuffer();
	UINT GetBufferSize();
	DXGI_OUTDUPL_FRAME_INFO& GetFrameInfo();
	DXGI_OUTDUPL_POINTER_SHAPE_INFO& GetShapeInfo();

private:
	BYTE* m_pPointerShape;
	UINT m_uiPointerShapeBufSize;
	DXGI_OUTDUPL_POINTER_SHAPE_INFO m_PSI;
	DXGI_OUTDUPL_FRAME_INFO m_FI;
};

class YangDXGIOutputDuplication
{
public:
	YangDXGIOutputDuplication(IDXGIAdapter1* pAdapter,
		ID3D11Device* pD3DDevice,
		ID3D11DeviceContext* pD3DDeviceContext,
		IDXGIOutput1* pDXGIOutput1,
		IDXGIOutputDuplication* pDXGIOutputDuplication);

	HRESULT GetDesc(DXGI_OUTPUT_DESC& desc);
	HRESULT AcquireNextFrame(IDXGISurface1** pD3D11Texture2D, YangDXGIPointerInfo*& pDXGIPointer);
	HRESULT ReleaseFrame();

	bool IsPrimary();


private:
	CComPtr<IDXGIAdapter1> m_Adapter;
	CComPtr<ID3D11Device> m_D3DDevice;
	CComPtr<ID3D11DeviceContext> m_D3DDeviceContext;
	CComPtr<IDXGIOutput1> m_DXGIOutput1;
	CComPtr<IDXGIOutputDuplication> m_DXGIOutputDuplication;
};

class YangDXGIManager
{
public:
	YangDXGIManager();
	~YangDXGIManager();
	HRESULT SetCaptureSource(YangCaptureSource type);
	YangCaptureSource GetCaptureSource();

	HRESULT GetOutputRect(RECT& rc);
	HRESULT GetOutputBits(BYTE* pBits, RECT& rcDest);
    bool m_isDrawmouse;
private:
	HRESULT Init();
	int GetMonitorCount();
	vector<YangDXGIOutputDuplication> GetOutputDuplication();
	void DrawMousePointer(BYTE* pDesktopBits, RECT rcDesktop, RECT rcDest);
private:
	CComPtr<IDXGIFactory1> m_spDXGIFactory1;
	vector<YangDXGIOutputDuplication> m_vOutputs;
	bool m_bInitialized;
	YangCaptureSource m_CaptureSource;
	RECT m_rcCurrentOutput;
	BYTE* m_pBuf;

	CComPtr<IWICImagingFactory> m_spWICFactory;
	ULONG_PTR m_gdiplusToken;
	YangDXGIPointerInfo* m_pDXGIPointer;
};
#endif
