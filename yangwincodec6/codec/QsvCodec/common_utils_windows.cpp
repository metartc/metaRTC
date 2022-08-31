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
#include <atlstr.h>
#include <Windows.h>
#include <guiddef.h>
#include <versionhelpers.h>

#include "common_utils.h"

// ATTENTION: If D3D surfaces are used, DX9_D3D or DX11_D3D must be set in project settings or hardcoded here

#define DX9_D3D 1

//#ifdef DX9_D3D
#include "common_directx9.h"
//#elif DX11_D3D
#include "common_directx11.h"
//#endif

/* =======================================================
 * Windows implementation of OS-specific utility functions
 */

static bool g_use_d3d11 = false;
static bool g_use_d3d9 = false;

mfxStatus Initialize(mfxIMPL impl, mfxVersion ver, MFXVideoSession* pSession, mfxFrameAllocator* pmfxAllocator, bool bCreateSharedHandles)
{
	mfxHandleType mfx_handle_type = MFX_HANDLE_D3D11_DEVICE;

	if (IsWindows8OrGreater()) {
		g_use_d3d11 = true;
		impl |= MFX_IMPL_VIA_D3D11;
		mfx_handle_type = MFX_HANDLE_D3D11_DEVICE;
	}
	else {
		g_use_d3d9 = true;
		mfx_handle_type = MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9;
	}

    mfxStatus sts = MFX_ERR_NONE;

    // Initialize Intel Media SDK Session
    sts = pSession->Init(impl, &ver);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

#if defined(DX9_D3D) || defined(DX11_D3D)
    // If mfxFrameAllocator is provided it means we need to setup DirectX device and memory allocator
    if (pmfxAllocator) {
        // Create DirectX device context
        mfxHDL deviceHandle = NULL;

		if (g_use_d3d11) {
			sts = d3d11::CreateHWDevice(*pSession, &deviceHandle, NULL, bCreateSharedHandles);
		}
		else if (g_use_d3d9) {
			sts = d3d9::CreateHWDevice(*pSession, &deviceHandle, NULL, bCreateSharedHandles);
		}

        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // Provide device manager to Media SDK
        sts = pSession->SetHandle(mfx_handle_type, deviceHandle);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        pmfxAllocator->pthis  = *pSession; // We use Media SDK session ID as the allocation identifier

		if (g_use_d3d11) {
			pmfxAllocator->Alloc = d3d11::simple_alloc;
			pmfxAllocator->Free = d3d11::simple_free;
			pmfxAllocator->Lock = d3d11::simple_lock;
			pmfxAllocator->Unlock = d3d11::simple_unlock;
			pmfxAllocator->GetHDL = d3d11::simple_gethdl;
		}
		else if (g_use_d3d9) {
			pmfxAllocator->Alloc = d3d9::simple_alloc;
			pmfxAllocator->Free = d3d9::simple_free;
			pmfxAllocator->Lock = d3d9::simple_lock;
			pmfxAllocator->Unlock = d3d9::simple_unlock;
			pmfxAllocator->GetHDL = d3d9::simple_gethdl;
		}

        // Since we are using video memory we must provide Media SDK with an external allocator
        sts = pSession->SetFrameAllocator(pmfxAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }
#else
    (void)bCreateSharedHandles;
    (void)pmfxAllocator;
#endif

    return sts;
}

void Release()
{
	if (g_use_d3d11) {
		d3d11::CleanupHWDevice();
	}
	else if (g_use_d3d9) {
		d3d9::CleanupHWDevice();
	}
}

void mfxGetTime(mfxTime* timestamp)
{
    QueryPerformanceCounter(timestamp);
}

double TimeDiffMsec(mfxTime tfinish, mfxTime tstart)
{
    static LARGE_INTEGER tFreq = { 0 };

    if (!tFreq.QuadPart) QueryPerformanceFrequency(&tFreq);

    double freq = (double)tFreq.QuadPart;
    return 1000.0 * ((double)tfinish.QuadPart - (double)tstart.QuadPart) / freq;
}

void ClearYUVSurfaceVMem(mfxMemId memId)
{
#if defined(DX9_D3D) || defined(DX11_D3D)
	if (g_use_d3d11) {
		d3d11::ClearYUVSurfaceD3D(memId);
	}
	else if (g_use_d3d9) {
		d3d9::ClearYUVSurfaceD3D(memId);
	}
#else
    (void)memId;
#endif
}

void ClearRGBSurfaceVMem(mfxMemId memId)
{
#if defined(DX9_D3D) || defined(DX11_D3D)
	if (g_use_d3d11) {
		d3d11::ClearRGBSurfaceD3D(memId);
	}
	else if (g_use_d3d9) {
		d3d9::ClearRGBSurfaceD3D(memId);
	}
#else
    (void)memId;
#endif
}

