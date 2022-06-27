//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifdef _MSC_VER

#include "YangDXGIManager.h"
#include <gdiplus.h>
#include <yangutil/sys/YangLog.h>
#ifdef _MSC_VER
#pragma comment (lib,"D3D11.lib")
#pragma comment (lib,"dxgi.lib")
#pragma comment (lib,"gdiplus.lib")
#endif
using namespace Gdiplus;

YangDXGIPointerInfo::YangDXGIPointerInfo(BYTE* pPointerShape, UINT uiPointerShapeBufSize, DXGI_OUTDUPL_FRAME_INFO fi, DXGI_OUTDUPL_POINTER_SHAPE_INFO psi)
	:	m_pPointerShape(pPointerShape),
		m_uiPointerShapeBufSize(uiPointerShapeBufSize),
		m_FI(fi),
		m_PSI(psi)
{

}

YangDXGIPointerInfo::~YangDXGIPointerInfo()
{
	if(m_pPointerShape)
	{
		delete [] m_pPointerShape;
	}
}

BYTE* YangDXGIPointerInfo::GetBuffer()
{
	return m_pPointerShape;
}

UINT YangDXGIPointerInfo::GetBufferSize()
{
	return m_uiPointerShapeBufSize;
}

DXGI_OUTDUPL_FRAME_INFO& YangDXGIPointerInfo::GetFrameInfo()
{
	return m_FI;
}

DXGI_OUTDUPL_POINTER_SHAPE_INFO& YangDXGIPointerInfo::GetShapeInfo()
{
	return m_PSI;
}

YangDXGIOutputDuplication::YangDXGIOutputDuplication(IDXGIAdapter1* pAdapter,
	ID3D11Device* pD3DDevice,
	ID3D11DeviceContext* pD3DDeviceContext,
	IDXGIOutput1* pDXGIOutput1,
	IDXGIOutputDuplication* pDXGIOutputDuplication)
	:	m_Adapter(pAdapter),
		m_D3DDevice(pD3DDevice),
		m_D3DDeviceContext(pD3DDeviceContext),
		m_DXGIOutput1(pDXGIOutput1),
		m_DXGIOutputDuplication(pDXGIOutputDuplication)
{
}

HRESULT YangDXGIOutputDuplication::GetDesc(DXGI_OUTPUT_DESC& desc)
{
	m_DXGIOutput1->GetDesc(&desc);
	return S_OK;
}

HRESULT YangDXGIOutputDuplication::AcquireNextFrame(IDXGISurface1** pDXGISurface, YangDXGIPointerInfo*& pDXGIPointer)
{
	DXGI_OUTDUPL_FRAME_INFO fi;
	CComPtr<IDXGIResource> spDXGIResource;
	HRESULT hr = m_DXGIOutputDuplication->AcquireNextFrame(0, &fi, &spDXGIResource);
	if(FAILED(hr))
	{
       // yang_error("m_DXGIOutputDuplication->AcquireNextFrame failed with hr=0x%08x", hr);
		return hr;
	}

	CComQIPtr<ID3D11Texture2D> spTextureResource = spDXGIResource;

	D3D11_TEXTURE2D_DESC desc;
	spTextureResource->GetDesc(&desc);

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory( &texDesc, sizeof(texDesc) );
	texDesc.Width = desc.Width;
	texDesc.Height = desc.Height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.Format = desc.Format;
	texDesc.BindFlags = 0;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	texDesc.MiscFlags = 0;

	CComPtr<ID3D11Texture2D> spD3D11Texture2D = NULL;
	hr = m_D3DDevice->CreateTexture2D(&texDesc, NULL, &spD3D11Texture2D);
	if(FAILED(hr))
		return hr;

	m_D3DDeviceContext->CopyResource(spD3D11Texture2D, spTextureResource);

	CComQIPtr<IDXGISurface1> spDXGISurface = spD3D11Texture2D;

	*pDXGISurface = spDXGISurface.Detach();

	// Updating mouse pointer, if visible
	if(fi.PointerPosition.Visible)
	{
		BYTE* pPointerShape = new BYTE[fi.PointerShapeBufferSize];

		DXGI_OUTDUPL_POINTER_SHAPE_INFO psi = {};
		UINT uiPointerShapeBufSize = fi.PointerShapeBufferSize;
		hr = m_DXGIOutputDuplication->GetFramePointerShape(uiPointerShapeBufSize, pPointerShape, &uiPointerShapeBufSize, &psi);
		if(hr == DXGI_ERROR_MORE_DATA)
		{
			pPointerShape = new BYTE[uiPointerShapeBufSize];

			hr = m_DXGIOutputDuplication->GetFramePointerShape(uiPointerShapeBufSize, pPointerShape, &uiPointerShapeBufSize, &psi);
		}

		if(hr == S_OK)
		{
         //   yang_trace("\nPointerPosition Visible=%d x=%d y=%d w=%d h=%d type=%d\n", fi.PointerPosition.Visible, fi.PointerPosition.Position.x, fi.PointerPosition.Position.y, psi.Width, psi.Height, psi.Type);

			if((psi.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME ||
			    psi.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR ||
			    psi.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR) &&
				psi.Width <= 128 && psi.Height <= 128)
			{
				// Here we can obtain pointer shape
				if(pDXGIPointer)
				{
					delete pDXGIPointer;
				}

				pDXGIPointer = new YangDXGIPointerInfo(pPointerShape, uiPointerShapeBufSize, fi, psi);

				pPointerShape = NULL;
			}

			DXGI_OUTPUT_DESC outDesc;
			GetDesc(outDesc);

			if(pDXGIPointer)
			{
				pDXGIPointer->GetFrameInfo().PointerPosition.Position.x = outDesc.DesktopCoordinates.left + fi.PointerPosition.Position.x;
				pDXGIPointer->GetFrameInfo().PointerPosition.Position.y = outDesc.DesktopCoordinates.top + fi.PointerPosition.Position.y;
			}
		}

		if(pPointerShape)
		{
			yang_deleteA(pPointerShape);
		}
	}

	return hr;
}

HRESULT YangDXGIOutputDuplication::ReleaseFrame()
{
	m_DXGIOutputDuplication->ReleaseFrame();
	return S_OK;
}

bool YangDXGIOutputDuplication::IsPrimary()
{
	DXGI_OUTPUT_DESC outdesc;
	m_DXGIOutput1->GetDesc(&outdesc);

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(outdesc.Monitor, &mi);
	if(mi.dwFlags & MONITORINFOF_PRIMARY)
	{
		return true;
	}
	return false;
}

YangDXGIManager::YangDXGIManager()
{
	m_CaptureSource = CSUndefined;
	SetRect(&m_rcCurrentOutput, 0, 0, 0, 0);
	m_pBuf = NULL;
	m_pDXGIPointer = NULL;
	m_bInitialized = false;
    m_isDrawmouse=false;
}

YangDXGIManager::~YangDXGIManager()
{
    GdiplusShutdown(m_gdiplusToken);

	if(m_pBuf)
	{
		delete [] m_pBuf;
		m_pBuf = NULL;
	}

	if(m_pDXGIPointer)
	{
		delete m_pDXGIPointer;
		m_pDXGIPointer = NULL;
	}
}

HRESULT YangDXGIManager::SetCaptureSource(YangCaptureSource cs)
{
	m_CaptureSource = cs;
	return S_OK;
}

YangCaptureSource YangDXGIManager::GetCaptureSource()
{
	return m_CaptureSource;
}

HRESULT YangDXGIManager::Init()
{
	if(m_bInitialized)
		return S_OK;

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&m_spDXGIFactory1) );
	if( FAILED(hr) )
	{
		yang_error("Failed to CreateDXGIFactory1 hr=%08x", hr);
		return hr;
	}

	// Getting all adapters
	vector<CComPtr<IDXGIAdapter1>> vAdapters;

	CComPtr<IDXGIAdapter1> spAdapter;
	for(int i=0; m_spDXGIFactory1->EnumAdapters1(i, &spAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		vAdapters.push_back(spAdapter);
		spAdapter.Release();
	}

	// Iterating over all adapters to get all outputs
	for(vector<CComPtr<IDXGIAdapter1>>::iterator AdapterIter = vAdapters.begin();
		AdapterIter != vAdapters.end();
		AdapterIter++)
	{
		vector<CComPtr<IDXGIOutput>> vOutputs;

		CComPtr<IDXGIOutput> spDXGIOutput;
		for(int i=0; (*AdapterIter)->EnumOutputs(i, &spDXGIOutput) != DXGI_ERROR_NOT_FOUND; i++)
		{
			DXGI_OUTPUT_DESC outputDesc;
			spDXGIOutput->GetDesc(&outputDesc);

          /**  yang_trace("\nDisplay output found. DeviceName=%ls  AttachedToDesktop=%d Rotation=%d DesktopCoordinates={(%d,%d),(%d,%d)}",
				outputDesc.DeviceName,
				outputDesc.AttachedToDesktop,
				outputDesc.Rotation,
				outputDesc.DesktopCoordinates.left,
				outputDesc.DesktopCoordinates.top,
				outputDesc.DesktopCoordinates.right,
				outputDesc.DesktopCoordinates.bottom);
                **/
			if(outputDesc.AttachedToDesktop)
			{
				vOutputs.push_back(spDXGIOutput);
			}

			spDXGIOutput.Release();
		}

		if(vOutputs.size() == 0)
			continue;

		// Creating device for each adapter that has the output
		CComPtr<ID3D11Device> spD3D11Device;
		CComPtr<ID3D11DeviceContext> spD3D11DeviceContext;
		D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_9_1;
		hr = D3D11CreateDevice((*AdapterIter), D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &spD3D11Device, &fl, &spD3D11DeviceContext);
		if( FAILED(hr) )
		{
            yang_error("Failed to create D3D11CreateDevice hr=%08x", hr);
			return hr;
		}

		for(std::vector<CComPtr<IDXGIOutput>>::iterator OutputIter = vOutputs.begin();
			OutputIter != vOutputs.end();
			OutputIter++)
		{
			CComQIPtr<IDXGIOutput1> spDXGIOutput1 = *OutputIter;
      if (!spDXGIOutput1)
      {
        yang_error("spDXGIOutput1 is NULL");
        continue;
      }

      CComQIPtr<IDXGIDevice1> spDXGIDevice = spD3D11Device;
      if (!spDXGIDevice)
      {
        yang_error("spDXGIDevice is NULL");
        continue;
      }

			CComPtr<IDXGIOutputDuplication> spDXGIOutputDuplication;
			hr = spDXGIOutput1->DuplicateOutput(spDXGIDevice, &spDXGIOutputDuplication);
      if (FAILED(hr))
      {
        yang_error("Failed to duplicate output hr=%08x", hr);
        continue;
      }

			m_vOutputs.push_back(
				YangDXGIOutputDuplication((*AdapterIter),
					spD3D11Device,
					spD3D11DeviceContext,
					spDXGIOutput1,
					spDXGIOutputDuplication));
		}
	}

	hr = m_spWICFactory.CoCreateInstance(CLSID_WICImagingFactory);
	if( FAILED(hr) )
	{
        yang_error("Failed to create WICImagingFactory hr=%08x", hr);
		return hr;
	}

	m_bInitialized = true;

	return S_OK;
}

HRESULT YangDXGIManager::GetOutputRect(RECT& rc)
{
	// Nulling rc just in case...
	SetRect(&rc, 0, 0, 0, 0);

	HRESULT hr = Init();
	if(hr != S_OK)
		return hr;

	vector<YangDXGIOutputDuplication> vOutputs = GetOutputDuplication();

	RECT rcShare;
	SetRect(&rcShare, 0, 0, 0, 0);

	for(vector<YangDXGIOutputDuplication>::iterator iter = vOutputs.begin();
		iter != vOutputs.end();
		iter++)
	{
		YangDXGIOutputDuplication& out = *iter;

		DXGI_OUTPUT_DESC outDesc;
		out.GetDesc(outDesc);
		RECT rcOutCoords = outDesc.DesktopCoordinates;

		UnionRect(&rcShare, &rcShare, &rcOutCoords);
	}

	CopyRect(&rc, &rcShare);

	return S_OK;
}

HRESULT YangDXGIManager::GetOutputBits(BYTE* pBits, RECT& rcDest)
{
	HRESULT hr = S_OK;

	DWORD dwDestWidth = rcDest.right - rcDest.left;
	DWORD dwDestHeight = rcDest.bottom - rcDest.top;

	RECT rcOutput;
	hr = GetOutputRect(rcOutput);
	if( FAILED(hr) )
		return hr;

	DWORD dwOutputWidth = rcOutput.right - rcOutput.left;
	DWORD dwOutputHeight = rcOutput.bottom - rcOutput.top;

	BYTE* pBuf = NULL;
	if(rcOutput.right > (LONG)dwDestWidth || rcOutput.bottom > (LONG)dwDestHeight)
	{
		// Output is larger than pBits dimensions
		if(!m_pBuf || !EqualRect(&m_rcCurrentOutput, &rcOutput))
		{
			DWORD dwBufSize = dwOutputWidth*dwOutputHeight*4;

			if(m_pBuf)
			{
				delete [] m_pBuf;
				m_pBuf = NULL;
			}

			m_pBuf = new BYTE[dwBufSize];

			CopyRect(&m_rcCurrentOutput, &rcOutput);
		}

		pBuf = m_pBuf;
	}
	else
	{
		// Output is smaller than pBits dimensions
		pBuf = pBits;
		dwOutputWidth = dwDestWidth;
		dwOutputHeight = dwDestHeight;
	}

	vector<YangDXGIOutputDuplication> vOutputs = GetOutputDuplication();

	for(vector<YangDXGIOutputDuplication>::iterator iter = vOutputs.begin();
		iter != vOutputs.end();
		iter++)
	{
		YangDXGIOutputDuplication& out = *iter;

		DXGI_OUTPUT_DESC outDesc;
		out.GetDesc(outDesc);
		RECT rcOutCoords = outDesc.DesktopCoordinates;

		CComPtr<IDXGISurface1> spDXGISurface1;
		hr = out.AcquireNextFrame(&spDXGISurface1, m_pDXGIPointer);
		if( FAILED(hr) )
			break;

		DXGI_MAPPED_RECT map;
		spDXGISurface1->Map(&map, DXGI_MAP_READ);

		RECT rcDesktop = outDesc.DesktopCoordinates;
		DWORD dwWidth = rcDesktop.right - rcDesktop.left;
		DWORD dwHeight = rcDesktop.bottom - rcDesktop.top;

		OffsetRect(&rcDesktop, -rcOutput.left, -rcOutput.top);

		DWORD dwMapPitchPixels = map.Pitch/4;

		switch(outDesc.Rotation)
		{
			case DXGI_MODE_ROTATION_IDENTITY:
				{
					// Just copying
					DWORD dwStripe = dwWidth*4;
					for(unsigned int i=0; i<dwHeight; i++)
					{
						memcpy_s(pBuf + (rcDesktop.left + (i + rcDesktop.top)*dwOutputWidth)*4, dwStripe, map.pBits + i*map.Pitch, dwStripe);

					}
				}
				break;
			case DXGI_MODE_ROTATION_ROTATE90:
				{
					// Rotating at 90 degrees
					DWORD* pSrc = (DWORD*)map.pBits;
					DWORD* pDst = (DWORD*)pBuf;
					for(unsigned int j=0; j<dwHeight; j++)
					{
						for(unsigned int i=0; i<dwWidth; i++)
						{
							*(pDst + (rcDesktop.left + (j + rcDesktop.top)*dwOutputWidth) + i) = *(pSrc + j + dwMapPitchPixels*(dwWidth - i - 1));
						}
					}
				}
				break;
			case DXGI_MODE_ROTATION_ROTATE180:
				{
					// Rotating at 180 degrees
					DWORD* pSrc = (DWORD*)map.pBits;
					DWORD* pDst = (DWORD*)pBuf;
					for(unsigned int j=0; j<dwHeight; j++)
					{
						for(unsigned int i=0; i<dwWidth; i++)
						{
							*(pDst + (rcDesktop.left + (j + rcDesktop.top)*dwOutputWidth) + i) = *(pSrc + (dwWidth - i - 1) + dwMapPitchPixels*(dwHeight - j - 1));
						}
					}
				}
				break;
			case DXGI_MODE_ROTATION_ROTATE270:
				{
					// Rotating at 270 degrees
					DWORD* pSrc = (DWORD*)map.pBits;
					DWORD* pDst = (DWORD*)pBuf;
					for(unsigned int j=0; j<dwHeight; j++)
					{
						for(unsigned int i=0; i<dwWidth; i++)
						{
							*(pDst + (rcDesktop.left + (j + rcDesktop.top)*dwOutputWidth) + i) = *(pSrc + (dwHeight - j - 1) + dwMapPitchPixels*i);
						}
					}
				}
				break;
		}

		spDXGISurface1->Unmap();

		out.ReleaseFrame();
	}

	if(FAILED(hr))
		return hr;

	// Now pBits have the desktop. Let's paint mouse pointer!
	if(m_isDrawmouse){
		if(pBuf != pBits)
		{
			DrawMousePointer(pBuf, rcOutput, rcOutput);
		}
		else
		{
			DrawMousePointer(pBuf, rcOutput, rcDest);
		}
	}
	// We have the pBuf filled with current desktop/monitor image.
	if(pBuf != pBits)
	{
		// pBuf contains the image that should be resized
		CComPtr<IWICBitmap> spBitmap = NULL;
		hr = m_spWICFactory->CreateBitmapFromMemory(dwOutputWidth, dwOutputHeight, GUID_WICPixelFormat32bppBGRA, dwOutputWidth*4, dwOutputWidth*dwOutputHeight*4, (BYTE*)pBuf, &spBitmap);
		if( FAILED(hr) )
			return hr;

		CComPtr<IWICBitmapScaler> spBitmapScaler = NULL;
		hr = m_spWICFactory->CreateBitmapScaler(&spBitmapScaler);
		if( FAILED(hr) )
			return hr;

		dwOutputWidth = rcOutput.right - rcOutput.left;
		dwOutputHeight = rcOutput.bottom - rcOutput.top;

		double aspect = (double)dwOutputWidth/(double)dwOutputHeight;

		DWORD scaledWidth = dwDestWidth;
		DWORD scaledHeight = dwDestHeight;

		if(aspect > 1)
		{
			scaledWidth = dwDestWidth;
			scaledHeight = (DWORD)(dwDestWidth/aspect);
		}
		else
		{
			scaledWidth = (DWORD)(aspect*dwDestHeight);
			scaledHeight = dwDestHeight;
		}
		yang_trace("\nscaledWidth==%ld,scaledHeight=%ld",scaledWidth,scaledHeight);
		spBitmapScaler->Initialize(
			spBitmap, scaledWidth, scaledHeight, WICBitmapInterpolationModeNearestNeighbor);

		spBitmapScaler->CopyPixels(NULL, scaledWidth*4, dwDestWidth*dwDestHeight*4, pBits);
	}
	return hr;
}

void YangDXGIManager::DrawMousePointer(BYTE* pDesktopBits, RECT rcDesktop, RECT rcDest)
{
	if(!m_pDXGIPointer)
		return;



	DWORD dwDestWidth = rcDest.right - rcDest.left;
	DWORD dwDestHeight = rcDest.bottom - rcDest.top;

	int PtrX = m_pDXGIPointer->GetFrameInfo().PointerPosition.Position.x - rcDesktop.left;
	int PtrY = m_pDXGIPointer->GetFrameInfo().PointerPosition.Position.y - rcDesktop.top;
	switch(m_pDXGIPointer->GetShapeInfo().Type)
	{
		case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR:
			{
                Bitmap bmpBitmap(dwDestWidth, dwDestHeight, dwDestWidth*4, PixelFormat32bppARGB, pDesktopBits);
                Graphics* graphics=Graphics::FromImage(&bmpBitmap);
                Bitmap bmpPointer(m_pDXGIPointer->GetShapeInfo().Width, m_pDXGIPointer->GetShapeInfo().Height, m_pDXGIPointer->GetShapeInfo().Width*4, PixelFormat32bppARGB, m_pDXGIPointer->GetBuffer());

                graphics->DrawImage(&bmpPointer, PtrX, PtrY);
                yang_delete(graphics);
			}
			break;
		case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME:
		case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR:
			{
				RECT rcPointer;

				if(m_pDXGIPointer->GetShapeInfo().Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
				{
					SetRect(&rcPointer, PtrX, PtrY, PtrX + m_pDXGIPointer->GetShapeInfo().Width, PtrY + m_pDXGIPointer->GetShapeInfo().Height/2);
				}
				else
				{
					SetRect(&rcPointer, PtrX, PtrY, PtrX + m_pDXGIPointer->GetShapeInfo().Width, PtrY + m_pDXGIPointer->GetShapeInfo().Height);
				}

				RECT rcDesktopPointer;
				IntersectRect(&rcDesktopPointer, &rcPointer, &rcDesktop);

				CopyRect(&rcPointer, &rcDesktopPointer);
				OffsetRect(&rcPointer, -PtrX, -PtrY);

				BYTE* pShapeBuffer = m_pDXGIPointer->GetBuffer();
				UINT* pDesktopBits32 = (UINT*)pDesktopBits;

				if(m_pDXGIPointer->GetShapeInfo().Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
				{
					for(int j = rcPointer.top, jDP = rcDesktopPointer.top;
						j<rcPointer.bottom && jDP<rcDesktopPointer.bottom;
						j++, jDP++)
					{
						for(int i = rcPointer.left, iDP = rcDesktopPointer.left;
							i<rcPointer.right && iDP<rcDesktopPointer.right;
							i++, iDP++)
						{
							BYTE Mask = 0x80 >> (i % 8);
							BYTE AndMask = pShapeBuffer[i/8 + (m_pDXGIPointer->GetShapeInfo().Pitch)*j] & Mask;
							BYTE XorMask = pShapeBuffer[i/8 + (m_pDXGIPointer->GetShapeInfo().Pitch)*(j + m_pDXGIPointer->GetShapeInfo().Height / 2)] & Mask;

							UINT AndMask32 = (AndMask) ? 0xFFFFFFFF : 0xFF000000;
							UINT XorMask32 = (XorMask) ? 0x00FFFFFF : 0x00000000;

							pDesktopBits32[jDP*dwDestWidth + iDP] = (pDesktopBits32[jDP*dwDestWidth + iDP] & AndMask32) ^ XorMask32;
						}
					}
				}
				else
				{
					UINT* pShapeBuffer32 = (UINT*)pShapeBuffer;
					for(int j = rcPointer.top, jDP = rcDesktopPointer.top;
						j<rcPointer.bottom && jDP<rcDesktopPointer.bottom;
						j++, jDP++)
					{
						for(int i = rcPointer.left, iDP = rcDesktopPointer.left;
							i<rcPointer.right && iDP<rcDesktopPointer.right;
							i++, iDP++)
						{
							// Set up mask
							UINT MaskVal = 0xFF000000 & pShapeBuffer32[i + (m_pDXGIPointer->GetShapeInfo().Pitch/4)*j];
							if (MaskVal)
							{
								// Mask was 0xFF
								pDesktopBits32[jDP*dwDestWidth + iDP] = (pDesktopBits32[jDP*dwDestWidth + iDP] ^ pShapeBuffer32[i + (m_pDXGIPointer->GetShapeInfo().Pitch/4)*j]) | 0xFF000000;
							}
							else
							{
								// Mask was 0x00 - replacing pixel
								pDesktopBits32[jDP*dwDestWidth + iDP] = pShapeBuffer32[i + (m_pDXGIPointer->GetShapeInfo().Pitch/4)*j];
							}
						}
					}
				}
			}
			break;
	}
}

vector<YangDXGIOutputDuplication> YangDXGIManager::GetOutputDuplication()
{
	vector<YangDXGIOutputDuplication> outputs;
	switch(m_CaptureSource)
	{
		case CSMonitor1:
		{
			// Return the one with IsPrimary
			for(vector<YangDXGIOutputDuplication>::iterator iter = m_vOutputs.begin();
				iter != m_vOutputs.end();
				iter++)
			{
				YangDXGIOutputDuplication& out = *iter;
				if(out.IsPrimary())
				{
					outputs.push_back(out);
					break;
				}
			}
		}
		break;

		case CSMonitor2:
		{
			// Return the first with !IsPrimary
			for(vector<YangDXGIOutputDuplication>::iterator iter = m_vOutputs.begin();
				iter != m_vOutputs.end();
				iter++)
			{
				YangDXGIOutputDuplication& out = *iter;
				if(!out.IsPrimary())
				{
					outputs.push_back(out);
					break;
				}
			}
		}
		break;

		case CSDesktop:
		{
			// Return all outputs
			for(vector<YangDXGIOutputDuplication>::iterator iter = m_vOutputs.begin();
				iter != m_vOutputs.end();
				iter++)
			{
				YangDXGIOutputDuplication& out = *iter;
				outputs.push_back(out);
			}
		}
		break;
	}
	return outputs;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    int *Count = (int*)dwData;
    (*Count)++;
    return TRUE;
}

int YangDXGIManager::GetMonitorCount()
{
    int Count = 0;
    if (EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&Count))
        return Count;
    return -1;
}
#endif
