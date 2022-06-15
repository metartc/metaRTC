// Copyright (c) 2019 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "common_directx9.h"

#include<map>

#define D3DFMT_NV12                     (D3DFORMAT)MAKEFOURCC('N','V','1','2')
#define D3DFMT_YV12                     (D3DFORMAT)MAKEFOURCC('Y','V','1','2')

namespace d3d9 {

	IDirect3DDeviceManager9*                pDeviceManager9 = NULL;
	IDirect3DDevice9Ex*                     pD3DD9 = NULL;
	IDirect3D9Ex*                           pD3D9 = NULL;
	HANDLE                                  pDeviceHandle = NULL;
	IDirectXVideoAccelerationService*       pDXVAServiceDec = NULL;
	IDirectXVideoAccelerationService*       pDXVAServiceVPP = NULL;

	bool                                    g_bCreateSharedHandles = false;

	std::map<mfxMemId*, mfxHDL>             allocResponses;
	std::map<mfxHDL, mfxFrameAllocResponse> allocDecodeResponses;
	std::map<mfxHDL, int>                   allocDecodeRefCount;

	const struct {
		mfxIMPL impl;       // actual implementation
		mfxU32  adapterID;  // device adapter number
	} implTypes[] = {
		{MFX_IMPL_HARDWARE, 0},
		{MFX_IMPL_HARDWARE2, 1},
		{MFX_IMPL_HARDWARE3, 2},
		{MFX_IMPL_HARDWARE4, 3}
	};

	// =================================================================
	// DirectX functionality required to manage D3D surfaces
	//

	mfxU32 GetIntelDeviceAdapterNum(mfxSession session)
	{
		mfxU32  adapterNum = 0;
		mfxIMPL impl;

		MFXQueryIMPL(session, &impl);

		mfxIMPL baseImpl = MFX_IMPL_BASETYPE(impl); // Extract Media SDK base implementation type

		// get corresponding adapter number
		for (mfxU8 i = 0; i < sizeof(implTypes) / sizeof(implTypes[0]); i++) {
			if (implTypes[i].impl == baseImpl) {
				adapterNum = implTypes[i].adapterID;
				break;
			}
		}

		return adapterNum;
	}

	// Create HW device context
	mfxStatus CreateHWDevice(mfxSession session, mfxHDL* deviceHandle, HWND window, bool bCreateSharedHandles)
	{
		// If window handle is not supplied, get window handle from coordinate 0,0
		if (window == NULL) {
			POINT point = { 0, 0 };
			window = WindowFromPoint(point);
		}

		g_bCreateSharedHandles = bCreateSharedHandles;

		HRESULT hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D9);
		if (!pD3D9 || FAILED(hr)) return MFX_ERR_DEVICE_FAILED;

		RECT rc;
		GetClientRect(window, &rc);

		D3DPRESENT_PARAMETERS D3DPP;
		memset(&D3DPP, 0, sizeof(D3DPP));
		D3DPP.Windowed = true;
		D3DPP.hDeviceWindow = window;
		D3DPP.Flags = D3DPRESENTFLAG_VIDEO;
		D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		D3DPP.BackBufferCount = 1;
		D3DPP.BackBufferFormat = D3DFMT_A8R8G8B8;
		D3DPP.BackBufferWidth = rc.right - rc.left;
		D3DPP.BackBufferHeight = rc.bottom - rc.top;
		D3DPP.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;

		hr = pD3D9->CreateDeviceEx(GetIntelDeviceAdapterNum(session),
			D3DDEVTYPE_HAL,
			window,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
			&D3DPP,
			NULL,
			&pD3DD9);
		if (FAILED(hr)) return MFX_ERR_NULL_PTR;

		hr = pD3DD9->ResetEx(&D3DPP, NULL);
		if (FAILED(hr)) return MFX_ERR_UNDEFINED_BEHAVIOR;

		hr = pD3DD9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		if (FAILED(hr)) return MFX_ERR_UNDEFINED_BEHAVIOR;

		UINT resetToken = 0;

		hr = DXVA2CreateDirect3DDeviceManager9(&resetToken, &pDeviceManager9);
		if (FAILED(hr)) return MFX_ERR_NULL_PTR;

		hr = pDeviceManager9->ResetDevice(pD3DD9, resetToken);
		if (FAILED(hr)) return MFX_ERR_UNDEFINED_BEHAVIOR;

		*deviceHandle = (mfxHDL)pDeviceManager9;

		return MFX_ERR_NONE;
	}

	IDirect3DDevice9Ex* GetDevice()
	{
		return pD3DD9;
	}


	// Free HW device context
	void CleanupHWDevice()
	{
		if (pDeviceManager9)
			pDeviceManager9->CloseDeviceHandle(pDeviceHandle);
		MSDK_SAFE_RELEASE(pDXVAServiceDec);
		MSDK_SAFE_RELEASE(pDXVAServiceVPP);
		MSDK_SAFE_RELEASE(pDeviceManager9);
		MSDK_SAFE_RELEASE(pD3DD9);
		MSDK_SAFE_RELEASE(pD3D9);
	}

	void ClearYUVSurfaceD3D(mfxMemId memId)
	{
		IDirect3DSurface9* pSurface;
		D3DSURFACE_DESC desc;
		D3DLOCKED_RECT locked;
		pSurface = (IDirect3DSurface9*)memId;
		pSurface->GetDesc(&desc);
		pSurface->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
		memset((mfxU8*)locked.pBits, 100, desc.Height*locked.Pitch);   // Y plane
		memset((mfxU8*)locked.pBits + desc.Height * locked.Pitch, 50, (desc.Height*locked.Pitch) / 2);   // UV plane
		pSurface->UnlockRect();
	}

	void ClearRGBSurfaceD3D(mfxMemId memId)
	{
		IDirect3DSurface9* pSurface;
		D3DSURFACE_DESC desc;
		D3DLOCKED_RECT locked;
		pSurface = (IDirect3DSurface9*)memId;
		pSurface->GetDesc(&desc);
		pSurface->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
		memset((mfxU8*)locked.pBits, 100, desc.Height*locked.Pitch);   // RGBA
		pSurface->UnlockRect();
	}

	//
	// Media SDK memory allocator entrypoints....
	//
	mfxStatus _simple_alloc(mfxFrameAllocRequest* request, mfxFrameAllocResponse* response)
	{
		DWORD   DxvaType;
		HRESULT hr = S_OK;

		// Determine surface format (current simple implementation only supports NV12 and RGB4(32))
		D3DFORMAT format;
		if (MFX_FOURCC_NV12 == request->Info.FourCC)
			format = D3DFMT_NV12;
		else if (MFX_FOURCC_RGB4 == request->Info.FourCC)
			format = D3DFMT_A8R8G8B8;
		else if (MFX_FOURCC_YUY2 == request->Info.FourCC)
			format = D3DFMT_YUY2;
		else if (MFX_FOURCC_YV12 == request->Info.FourCC)
			format = D3DFMT_YV12;
		else
			format = (D3DFORMAT)request->Info.FourCC;


		// Determine render target
		if (MFX_MEMTYPE_DXVA2_PROCESSOR_TARGET & request->Type)
			DxvaType = DXVA2_VideoProcessorRenderTarget;
		else
			DxvaType = DXVA2_VideoDecoderRenderTarget;

		// Force use of video processor if color conversion is required (with this simple set of samples we only illustrate RGB4 color converison via VPP)
		if (D3DFMT_A8R8G8B8 == format) // must use processor service
			DxvaType = DXVA2_VideoProcessorRenderTarget;

		mfxMemId* mids = NULL;
		if (!g_bCreateSharedHandles) {
			mids = new mfxMemId[request->NumFrameSuggested];
			if (!mids) return MFX_ERR_MEMORY_ALLOC;
		}
		else {
			mids = new mfxMemId[request->NumFrameSuggested * 2];
			if (!mids) return MFX_ERR_MEMORY_ALLOC;

			memset(mids, 0, sizeof(mfxMemId)*request->NumFrameSuggested * 2);
		}

		if (!pDeviceHandle) {
			hr = pDeviceManager9->OpenDeviceHandle(&pDeviceHandle);
			if (FAILED(hr)) return MFX_ERR_MEMORY_ALLOC;
		}

		IDirectXVideoAccelerationService* pDXVAServiceTmp = NULL;

		if (DXVA2_VideoDecoderRenderTarget == DxvaType) { // for both decode and encode
			if (pDXVAServiceDec == NULL)
				hr = pDeviceManager9->GetVideoService(pDeviceHandle, IID_IDirectXVideoDecoderService, (void**)&pDXVAServiceDec);

			pDXVAServiceTmp = pDXVAServiceDec;
		}
		else { // DXVA2_VideoProcessorRenderTarget ; for VPP
			if (pDXVAServiceVPP == NULL)
				hr = pDeviceManager9->GetVideoService(pDeviceHandle, IID_IDirectXVideoProcessorService, (void**)&pDXVAServiceVPP);

			pDXVAServiceTmp = pDXVAServiceVPP;
		}
		if (FAILED(hr)) return MFX_ERR_MEMORY_ALLOC;

		if (g_bCreateSharedHandles && !(MFX_MEMTYPE_INTERNAL_FRAME & request->Type)) {
			// Allocate surfaces with shared handles. Commonly used for OpenCL interoperability
			for (int i = 0; i < request->NumFrameSuggested; ++i) {
				mfxMemId* tmpptr = mids + i + request->NumFrameSuggested;

				hr = pDXVAServiceTmp->CreateSurface(request->Info.Width,
					request->Info.Height,
					0,
					format,
					D3DPOOL_DEFAULT,
					0,
					DxvaType,
					(IDirect3DSurface9**)mids + i,
					(HANDLE*)(tmpptr));
			}
		}
		else {
			// Allocate surfaces
			hr = pDXVAServiceTmp->CreateSurface(request->Info.Width,
				request->Info.Height,
				request->NumFrameSuggested - 1,
				format,
				D3DPOOL_DEFAULT,
				0,
				DxvaType,
				(IDirect3DSurface9**)mids,
				NULL);
		}
		if (FAILED(hr)) return MFX_ERR_MEMORY_ALLOC;

		response->mids = mids;
		response->NumFrameActual = request->NumFrameSuggested;

		return MFX_ERR_NONE;
	}

	mfxStatus simple_alloc(mfxHDL pthis, mfxFrameAllocRequest* request, mfxFrameAllocResponse* response)
	{
		mfxStatus sts = MFX_ERR_NONE;

		if (request->Type & MFX_MEMTYPE_SYSTEM_MEMORY)
			return MFX_ERR_UNSUPPORTED;

		if (allocDecodeResponses.find(pthis) != allocDecodeResponses.end() &&
			MFX_MEMTYPE_EXTERNAL_FRAME & request->Type &&
			MFX_MEMTYPE_FROM_DECODE & request->Type) {
			// Memory for this request was already allocated during manual allocation stage. Return saved response
			//   When decode acceleration device (DXVA) is created it requires a list of D3D surfaces to be passed.
			//   Therefore Media SDK will ask for the surface info/mids again at Init() stage, thus requiring us to return the saved response
			//   (No such restriction applies to Encode or VPP)
			*response = allocDecodeResponses[pthis];
			allocDecodeRefCount[pthis]++;
		}
		else {
			sts = _simple_alloc(request, response);

			if (MFX_ERR_NONE == sts) {
				if (MFX_MEMTYPE_EXTERNAL_FRAME & request->Type &&
					MFX_MEMTYPE_FROM_DECODE & request->Type) {
					// Decode alloc response handling
					allocDecodeResponses[pthis] = *response;
					allocDecodeRefCount[pthis]++;
				}
				else {
					// Encode and VPP alloc response handling
					allocResponses[response->mids] = pthis;
				}
			}
		}

		return sts;
	}

	mfxStatus simple_lock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr)
	{
		pthis; // To suppress warning for this unused parameter

		IDirect3DSurface9* pSurface = (IDirect3DSurface9*)mid;

		if (pSurface == 0) return MFX_ERR_INVALID_HANDLE;
		if (ptr == 0) return MFX_ERR_LOCK_MEMORY;

		D3DSURFACE_DESC desc;
		HRESULT hr = pSurface->GetDesc(&desc);
		if (FAILED(hr)) return MFX_ERR_LOCK_MEMORY;

		D3DLOCKED_RECT locked;
		hr = pSurface->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
		if (FAILED(hr)) return MFX_ERR_LOCK_MEMORY;

		// In these simple set of samples we only illustrate usage of NV12 and RGB4(32)
		if (D3DFMT_NV12 == desc.Format) {
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8*)locked.pBits;
			ptr->U = (mfxU8*)locked.pBits + desc.Height * locked.Pitch;
			ptr->V = ptr->U + 1;
		}
		else if (D3DFMT_A8R8G8B8 == desc.Format) {
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->B = (mfxU8*)locked.pBits;
			ptr->G = ptr->B + 1;
			ptr->R = ptr->B + 2;
			ptr->A = ptr->B + 3;
		}
		else if (D3DFMT_YUY2 == desc.Format) {
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8*)locked.pBits;
			ptr->U = ptr->Y + 1;
			ptr->V = ptr->Y + 3;
		}
		else if (D3DFMT_YV12 == desc.Format) {
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8*)locked.pBits;
			ptr->V = ptr->Y + desc.Height * locked.Pitch;
			ptr->U = ptr->V + (desc.Height * locked.Pitch) / 4;
		}
		else if (D3DFMT_P8 == desc.Format) {
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8*)locked.pBits;
			ptr->U = 0;
			ptr->V = 0;
		}

		return MFX_ERR_NONE;
	}

	mfxStatus simple_unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr)
	{
		pthis; // To suppress warning for this unused parameter

		IDirect3DSurface9* pSurface = (IDirect3DSurface9*)mid;

		if (pSurface == 0) return MFX_ERR_INVALID_HANDLE;

		pSurface->UnlockRect();

		if (NULL != ptr) {
			ptr->Pitch = 0;
			ptr->R = 0;
			ptr->G = 0;
			ptr->B = 0;
			ptr->A = 0;
		}

		return MFX_ERR_NONE;
	}

	mfxStatus simple_gethdl(mfxHDL pthis, mfxMemId mid, mfxHDL* handle)
	{
		pthis; // To suppress warning for this unused parameter

		if (handle == 0) return MFX_ERR_INVALID_HANDLE;

		*handle = mid;
		return MFX_ERR_NONE;
	}


	mfxStatus _simple_free(mfxFrameAllocResponse* response)
	{
		if (response->mids) {
			for (mfxU32 i = 0; i < response->NumFrameActual; i++) {
				if (response->mids[i]) {
					IDirect3DSurface9* handle = (IDirect3DSurface9*)response->mids[i];
					handle->Release();
				}
			}
		}

		delete[] response->mids;
		response->mids = 0;

		return MFX_ERR_NONE;
	}

	mfxStatus simple_free(mfxHDL pthis, mfxFrameAllocResponse* response)
	{
		if (!response) return MFX_ERR_NULL_PTR;

		if (allocResponses.find(response->mids) == allocResponses.end()) {
			// Decode free response handling
			if (--allocDecodeRefCount[pthis] == 0) {
				_simple_free(response);
				allocDecodeResponses.erase(pthis);
				allocDecodeRefCount.erase(pthis);
			}
		}
		else {
			// Encode and VPP free response handling
			allocResponses.erase(response->mids);
			_simple_free(response);
		}

		return MFX_ERR_NONE;
	}

}