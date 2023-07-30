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

#pragma once

#include "common_utils.h"

#include <initguid.h>
#pragma warning(disable : 4201) // Disable annoying DX warning
#include <d3d9.h>
#include <dxva2api.h>

//#define DEVICE_MGR_TYPE MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9

// =================================================================
// DirectX functionality required to manage D3D surfaces
//

// Create DirectX 9 device context
// - Required when using D3D surfaces.
// - D3D Device created and handed to Intel Media SDK
// - Intel graphics device adapter id will be determined automatically (does not have to be primary),
//   but with the following caveats:
//     - Device must be active. Normally means a monitor has to be physically attached to device
//     - Device must be enabled in BIOS. Required for the case when used together with a discrete graphics card
//     - For switchable graphics solutions (mobile) make sure that Intel device is the active device

namespace d3d9 {

	mfxStatus CreateHWDevice(mfxSession session, mfxHDL* deviceHandle, HWND hWnd, bool bCreateSharedHandles = false);
	void CleanupHWDevice();
	IDirect3DDevice9Ex* GetDevice();
	void ClearYUVSurfaceD3D(mfxMemId memId);
	void ClearRGBSurfaceD3D(mfxMemId memId);

	mfxStatus simple_alloc(mfxHDL pthis, mfxFrameAllocRequest* request, mfxFrameAllocResponse* response);
	mfxStatus simple_lock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr);
	mfxStatus simple_unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr);
	mfxStatus simple_gethdl(mfxHDL pthis, mfxMemId mid, mfxHDL* handle);
	mfxStatus simple_free(mfxHDL pthis, mfxFrameAllocResponse* response);

}



