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

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cmath>
#include "common_utils.h"
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
#include <intrin.h>
#include <array>
#include <vector>
#endif
#include <yangutil/yangtype.h>
// =================================================================
// Utility functions, not directly tied to Intel Media SDK functionality
//

void PrintErrString(int err,const char* filestr,int line)
{
    switch (err) {
    case   0:
        printf("\n No error.\n");
        break;
    case  -1:
        printf("\n Unknown error: %s %d\n",filestr,line);
        break;
    case  -2:
        printf("\n Null pointer.  Check filename/path + permissions? %s %d\n",filestr,line);
        break;
    case  -3:
        printf("\n Unsupported feature/library load error. %s %d\n",filestr,line);
        break;
    case  -4:
        printf("\n Could not allocate memory. %s %d\n",filestr,line);
        break;
    case  -5:
        printf("\n Insufficient IO buffers. %s %d\n",filestr,line);
        break;
    case  -6:
        printf("\n Invalid handle. %s %d\n",filestr,line);
        break;
    case  -7:
        printf("\n Memory lock failure. %s %d\n",filestr,line);
        break;
    case  -8:
        printf("\n Function called before initialization. %s %d\n",filestr,line);
        break;
    case  -9:
        printf("\n Specified object not found. %s %d\n",filestr,line);
        break;
    case -10:
        printf("\n More input data expected. %s %d\n",filestr,line);
        break;
    case -11:
        printf("\n More output surfaces expected. %s %d\n",filestr,line);
        break;
    case -12:
        printf("\n Operation aborted. %s %d\n",filestr,line);
        break;
    case -13:
        printf("\n HW device lost. %s %d\n",filestr,line);
        break;
    case -14:
        printf("\n Incompatible video parameters. %s %d\n",filestr,line);
        break;
    case -15:
        printf("\n Invalid video parameters. %s %d\n",filestr,line);
        break;
    case -16:
        printf("\n Undefined behavior. %s %d\n",filestr,line);
        break;
    case -17:
        printf("\n Device operation failure. %s %d\n",filestr,line);
        break;
    case -18:
        printf("\n More bitstream data expected. %s %d\n",filestr,line);
        break;
    case -19:
        printf("\n Incompatible audio parameters. %s %d\n",filestr,line);
        break;
    case -20:
        printf("\n Invalid audio parameters. %s %d\n",filestr,line);
        break;
    default:
        printf("\nError code %d,\t%s\t%d\n\n", err, filestr, line);
    }
}

FILE* OpenFile(const char* fileName, const char* mode)
{
    FILE* openFile = nullptr;
    MSDK_FOPEN(openFile, fileName, mode);
    return openFile;
}

void CloseFile(FILE* fHdl)
{
    if(fHdl)
        fclose(fHdl);
}

mfxStatus ReadPlaneData(mfxU16 w, mfxU16 h, mfxU8* buf, mfxU8* ptr,
                        mfxU16 pitch, mfxU16 offset, FILE* fSource)
{
    mfxU32 nBytesRead;
    for (mfxU16 i = 0; i < h; i++) {
        nBytesRead = (mfxU32) fread(buf, 1, w, fSource);
        if (w != nBytesRead)
            return MFX_ERR_MORE_DATA;
        for (mfxU16 j = 0; j < w; j++)
            ptr[i * pitch + j * 2 + offset] = buf[j];
    }
    return MFX_ERR_NONE;
}

mfxStatus LoadRawFrame(mfxFrameSurface1* pSurface, FILE* fSource)
{
    if (!fSource) {
        // Simulate instantaneous access to 1000 "empty" frames.
        static int frameCount = 0;
        if (1000 == frameCount++)
            return MFX_ERR_MORE_DATA;
        else
            return MFX_ERR_NONE;
    }

    mfxStatus sts = MFX_ERR_NONE;
    mfxU32 nBytesRead;
    mfxU16 w, h, i, pitch;
    mfxU8* ptr;
    mfxFrameInfo* pInfo = &pSurface->Info;
    mfxFrameData* pData = &pSurface->Data;

    if (pInfo->CropH > 0 && pInfo->CropW > 0) {
        w = pInfo->CropW;
        h = pInfo->CropH;
    } else {
        w = pInfo->Width;
        h = pInfo->Height;
    }

    pitch = pData->Pitch;
    ptr = pData->Y + pInfo->CropX + pInfo->CropY * pData->Pitch;

    // read luminance plane
    for (i = 0; i < h; i++) {
        nBytesRead = (mfxU32) fread(ptr + i * pitch, 1, w, fSource);
        if (w != nBytesRead)
            return MFX_ERR_MORE_DATA;
    }

    mfxU8 buf[2048];        // maximum supported chroma width for nv12
    w /= 2;
    h /= 2;
    ptr = pData->UV + pInfo->CropX + (pInfo->CropY / 2) * pitch;
    if (w > 2048)
        return MFX_ERR_UNSUPPORTED;

    // load U
    sts = ReadPlaneData(w, h, buf, ptr, pitch, 0, fSource);
    if (MFX_ERR_NONE != sts)
        return sts;
    // load V
    ReadPlaneData(w, h, buf, ptr, pitch, 1, fSource);
    if (MFX_ERR_NONE != sts)
        return sts;

    return MFX_ERR_NONE;
}

mfxStatus ReadPlaneData10Bit(mfxU16 w, mfxU16 h, mfxU16* buf, mfxU8* ptr,
    mfxU16 pitch, mfxU16 shift, FILE* fSource)
{
    mfxU32 nBytesRead;
    mfxU16* shortPtr;

    for (mfxU16 i = 0; i < h; i++) {
        nBytesRead = (mfxU32)fread(buf, 2, w, fSource); //Reading in 16bits per pixel.
        if (w != nBytesRead)
            return MFX_ERR_MORE_DATA;
        // Read data with P010 and convert it to MS-P010
        //Shifting left the data in a 16bits boundary
        //Because each 10bit pixel channel takes 2 bytes with the LSB on the right side of the 16bits
        //See this web page for the description of MS-P010 format
        //https://msdn.microsoft.com/en-us/library/windows/desktop/bb970578(v=vs.85).aspx#overview
        if (shift > 0) {
            shortPtr = (mfxU16 *)(ptr + i * pitch);
            for (mfxU16 j = 0; j < w; j++)
                shortPtr[j] = buf[j] << 6;
        }
    }
    return MFX_ERR_NONE;
}

mfxStatus LoadRaw10BitFrame(mfxFrameSurface1* pSurface, FILE* fSource)
{
    if (!fSource) {
        // Simulate instantaneous access to 1000 "empty" frames.
        static int frameCount = 0;
        if (1000 == frameCount++)
            return MFX_ERR_MORE_DATA;
        else
            return MFX_ERR_NONE;
    }

    mfxStatus sts = MFX_ERR_NONE;
    mfxU16 w, h,pitch;
    mfxU8* ptr;
    mfxFrameInfo* pInfo = &pSurface->Info;
    mfxFrameData* pData = &pSurface->Data;

    if (pInfo->CropH > 0 && pInfo->CropW > 0) {
        w = pInfo->CropW;
        h = pInfo->CropH;
    }
    else {
        w = pInfo->Width;
        h = pInfo->Height;
    }

    pitch = pData->Pitch;
    mfxU16 buf[4096];        // maximum supported chroma width for nv12

    // read luminance plane
    ptr = pData->Y + pInfo->CropX + pInfo->CropY * pData->Pitch;
    sts = ReadPlaneData10Bit(w, h, buf, ptr, pitch, pInfo->Shift, fSource);
    if (MFX_ERR_NONE != sts)
        return sts;

    // Load UV plan
    h /= 2;
    ptr = pData->UV + pInfo->CropX + (pInfo->CropY / 2) * pitch;

    // load U
    sts = ReadPlaneData10Bit(w, h, buf, ptr, pitch, pInfo->Shift, fSource);
    if (MFX_ERR_NONE != sts)
        return sts;

    return MFX_ERR_NONE;
}

mfxStatus LoadRawRGBFrame(mfxFrameSurface1* pSurface, FILE* fSource)
{
    if (!fSource) {
        // Simulate instantaneous access to 1000 "empty" frames.
        static int frameCount = 0;
        if (1000 == frameCount++)
            return MFX_ERR_MORE_DATA;
        else
            return MFX_ERR_NONE;
    }

    mfxU32 nBytesRead;
    mfxU16 w, h;
    mfxFrameInfo* pInfo = &pSurface->Info;

    if (pInfo->CropH > 0 && pInfo->CropW > 0) {
        w = pInfo->CropW;
        h = pInfo->CropH;
    } else {
        w = pInfo->Width;
        h = pInfo->Height;
    }

    for (mfxU16 i = 0; i < h; i++) {
        nBytesRead = (mfxU32)fread(pSurface->Data.B + i * pSurface->Data.Pitch,
                           1, w * 4, fSource);
        if ((mfxU32)(w * 4) != nBytesRead)
            return MFX_ERR_MORE_DATA;
    }

    return MFX_ERR_NONE;
}

mfxStatus WriteBitStreamFrame(mfxBitstream* pMfxBitstream, FILE* fSink)
{
    if (!pMfxBitstream)
       return MFX_ERR_NULL_PTR;

    if (fSink) {
        mfxU32 nBytesWritten =
            (mfxU32) fwrite(pMfxBitstream->Data + pMfxBitstream->DataOffset, 1,
                            pMfxBitstream->DataLength, fSink);

        if (nBytesWritten != pMfxBitstream->DataLength)
            return MFX_ERR_UNDEFINED_BEHAVIOR;
    }

    pMfxBitstream->DataLength = 0;

    return MFX_ERR_NONE;
}

mfxStatus ReadBitStreamData(mfxBitstream* pBS, FILE* fSource)
{
    memmove(pBS->Data, pBS->Data + pBS->DataOffset, pBS->DataLength);
    pBS->DataOffset = 0;

    mfxU32 nBytesRead = (mfxU32) fread(pBS->Data + pBS->DataLength, 1,
                                       pBS->MaxLength - pBS->DataLength,
                                       fSource);

    if (0 == nBytesRead)
        return MFX_ERR_MORE_DATA;

    pBS->DataLength += nBytesRead;

    return MFX_ERR_NONE;
}

mfxStatus WriteSection(mfxU8* plane, mfxU16 factor, mfxU16 chunksize,
                       mfxFrameInfo* pInfo, mfxFrameData* pData, mfxU32 i,
                       mfxU32 j, FILE* fSink)
{
    if (chunksize !=
        fwrite(plane +
               (pInfo->CropY * pData->Pitch / factor + pInfo->CropX) +
               i * pData->Pitch + j, 1, chunksize, fSink))
        return MFX_ERR_UNDEFINED_BEHAVIOR;
    return MFX_ERR_NONE;
}

mfxStatus WriteRawFrame(mfxFrameSurface1* pSurface, FILE* fSink)
{
    mfxFrameInfo* pInfo = &pSurface->Info;
    mfxFrameData* pData = &pSurface->Data;
    mfxU32 nByteWrite;
    mfxU16 i, j, h, w, pitch;
    mfxU8* ptr;
    mfxStatus sts = MFX_ERR_NONE;

    if (pInfo->CropH > 0 && pInfo->CropW > 0)
    {
        w = pInfo->CropW;
        h = pInfo->CropH;
    }
    else
    {
        w = pInfo->Width;
        h = pInfo->Height;
    }

    if (pInfo->FourCC == MFX_FOURCC_RGB4 || pInfo->FourCC == MFX_FOURCC_A2RGB10)
    {
        pitch = pData->Pitch;
        ptr = yang_min(yang_min(pData->R, pData->G), pData->B);

        for (i = 0; i < h; i++)
        {
            nByteWrite = (mfxU32)fwrite(ptr + i * pitch, 1, w * 4, fSink);
            if ((mfxU32)(w * 4) != nByteWrite)
            {
                return MFX_ERR_MORE_DATA;
            }
        }
    }
    else
    {
        for (i = 0; i < pInfo->CropH; i++)
            sts = WriteSection(pData->Y, 1, pInfo->CropW, pInfo, pData, i, 0, fSink);

        h = pInfo->CropH / 2;
        w = pInfo->CropW;
        for (i = 0; i < h; i++)
            for (j = 0; j < w; j += 2)
                sts = WriteSection(pData->UV, 2, 1, pInfo, pData, i, j, fSink);
        for (i = 0; i < h; i++)
            for (j = 1; j < w; j += 2)
                sts = WriteSection(pData->UV, 2, 1, pInfo, pData, i, j, fSink);
    }

    return sts;
}

mfxStatus WriteSection10Bit(mfxU8* plane, mfxU16 factor, mfxU16 chunksize,
    mfxFrameInfo* pInfo, mfxFrameData* pData, mfxU32 i,
    /*mfxU32 j,*/ FILE* fSink)
{
    // Temporary buffer to convert MS-P010 to P010
    std::vector<mfxU16> tmp;
    mfxU16* shortPtr;

    shortPtr = (mfxU16*)(plane + (pInfo->CropY * pData->Pitch / factor + pInfo->CropX) + i * pData->Pitch);
    if (pInfo->Shift)
    {
        // Convert MS-P010 to P010 and write
        tmp.resize(pData->Pitch);

        //Shifting right the data in a 16bits boundary
        //Because each 10bit pixel channel takes 2 bytes with the LSB on the right side of the 16bits
        //See this web page for the description of 10bit YUV format
        //https://msdn.microsoft.com/en-us/library/windows/desktop/bb970578(v=vs.85).aspx#overview
        for (int idx = 0; idx < pInfo->CropW; idx++)
        {
            tmp[idx] = shortPtr[idx] >> 6;
        }
        if (chunksize != fwrite(&tmp[0], 1, chunksize, fSink))
            return MFX_ERR_UNDEFINED_BEHAVIOR;
    }
    else {
        if (chunksize != fwrite(shortPtr, 1, chunksize, fSink))
            return MFX_ERR_UNDEFINED_BEHAVIOR;
    }

    return MFX_ERR_NONE;
}

mfxStatus WriteRaw10BitFrame(mfxFrameSurface1* pSurface, FILE* fSink)
{
    mfxFrameInfo* pInfo = &pSurface->Info;
    mfxFrameData* pData = &pSurface->Data;
    mfxStatus sts = MFX_ERR_NONE;


    for (mfxU16 i = 0; i < pInfo->CropH; i++) {
        sts = WriteSection10Bit(pData->Y, 1, pInfo->CropW * 2, pInfo, pData, i, fSink);
    }

    for (mfxU16 i = 0; i < pInfo->CropH / 2; i++) {
        sts = WriteSection10Bit(pData->UV, 2, pInfo->CropW * 2, pInfo, pData, i, fSink);
    }

    return sts;
}

int GetFreeTaskIndex(Task* pTaskPool, mfxU16 nPoolSize)
{
    if (pTaskPool)
        for (int i = 0; i < nPoolSize; i++)
            if (!pTaskPool[i].syncp)
                return i;
    return MFX_ERR_NOT_FOUND;
}

void ClearYUVSurfaceSysMem(mfxFrameSurface1* pSfc, mfxU16 width, mfxU16 height)
{
    // In case simulating direct access to frames we initialize the allocated surfaces with default pattern
    memset(pSfc->Data.Y, 100, width * height);  // Y plane
    memset(pSfc->Data.U, 50, (width * height)/2);  // UV plane
}


// Get free raw frame surface
int GetFreeSurfaceIndex(mfxFrameSurface1** pSurfacesPool, mfxU16 nPoolSize)
{
    if (pSurfacesPool)
        for (mfxU16 i = 0; i < nPoolSize; i++)
            if (0 == pSurfacesPool[i]->Data.Locked)
                return i;
    return MFX_ERR_NOT_FOUND;
}

int GetFreeSurfaceIndex(const std::vector<mfxFrameSurface1>& pSurfacesPool)
{
    auto it = std::find_if(pSurfacesPool.begin(), pSurfacesPool.end(), [](const mfxFrameSurface1& surface) {
                        return 0 == surface.Data.Locked;
                    });

    if(it == pSurfacesPool.end())
        return MFX_ERR_NOT_FOUND;
    else return it - pSurfacesPool.begin();
}

char mfxFrameTypeString(mfxU16 FrameType)
{
    mfxU8 FrameTmp = FrameType & 0xF;
    char FrameTypeOut;
    switch (FrameTmp) {
    case MFX_FRAMETYPE_I:
        FrameTypeOut = 'I';
        break;
    case MFX_FRAMETYPE_P:
        FrameTypeOut = 'P';
        break;
    case MFX_FRAMETYPE_B:
        FrameTypeOut = 'B';
        break;
    default:
        FrameTypeOut = '*';
    }
    return FrameTypeOut;
}

#if defined(_WIN32) || defined(_WIN64)
//This function is modified according to the MSDN example at
// https://msdn.microsoft.com/en-us/library/hskdteyh(v=vs.140).aspx#Example
void showCPUInfo() {
    int nIds;
    int nExIds;
    std::vector<std::array<int, 4>> vendorData;
    std::vector<std::array<int, 4>> procData;
    std::array<int, 4> cpui;

    // Calling __cpuid with 0x0 as the function_id argument
    // gets the number of the highest valid function ID.
    __cpuid(cpui.data(), 0);
    nIds = cpui[0];

    for (int i = 0; i <= nIds; ++i)
    {
        __cpuidex(cpui.data(), i, 0);
        vendorData.push_back(cpui);
    }

    // Capture vendor string
    char vendor[0x20];
    memset(vendor, 0, sizeof(vendor));
    *reinterpret_cast<int*>(vendor) = vendorData[0][1];
    *reinterpret_cast<int*>(vendor + 4) = vendorData[0][3];
    *reinterpret_cast<int*>(vendor + 8) = vendorData[0][2];

    // Calling __cpuid with 0x80000000 as the function_id argument
    // gets the number of the highest valid extended ID.
    __cpuid(cpui.data(), 0x80000000);
    nExIds = cpui[0];

    char processor[0x40];
    memset(processor, 0, sizeof(processor));

    for (int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuidex(cpui.data(), i, 0);
        procData.push_back(cpui);
    }

    // Interpret CPU brand string if reported
    if (nExIds >= 0x80000004)
    {
        memcpy(processor, procData[2].data(), sizeof(cpui));
        memcpy(processor + 16, procData[3].data(), sizeof(cpui));
        memcpy(processor + 32, procData[4].data(), sizeof(cpui));
    }

    printf("Vendor: %s\n", vendor);
    printf("Processor: %s\n", processor);
    printf("Please check http://ark.intel.com/ for the GPU info\n");
}
#endif

const mfxPluginUID & msdkGetPluginUID(mfxIMPL impl, msdkComponentType type, mfxU32 uCodecid)
{
    if (impl == MFX_IMPL_SOFTWARE)
    {
        switch (type)
        {
        case MSDK_VDECODE:
            switch (uCodecid)
            {
            case MFX_CODEC_HEVC:
                return MFX_PLUGINID_HEVCD_SW;
            }
            break;
        case MSDK_VENCODE:
            switch (uCodecid)
            {
            case MFX_CODEC_HEVC:
                return MFX_PLUGINID_HEVCE_SW;
            }
            break;
        }
    }
    else if ((impl |= MFX_IMPL_HARDWARE))
    {
        switch (type)
        {
        case MSDK_VDECODE:
            switch (uCodecid)
            {
            case MFX_CODEC_HEVC:
                return MFX_PLUGINID_HEVCD_HW; // MFX_PLUGINID_HEVCD_SW for now
            case MFX_CODEC_VP8:
                return MFX_PLUGINID_VP8D_HW;
            }
            break;
        case MSDK_VENCODE:
            switch (uCodecid)
            {
            case MFX_CODEC_HEVC:
                return MFX_PLUGINID_HEVCE_HW;
            }
            break;
        case MSDK_VENC:
            switch (uCodecid)
            {
            case MFX_CODEC_HEVC:
                return MFX_PLUGINID_HEVCE_FEI_HW;   // HEVC FEI uses ENC interface
            }
            break;
        }
    }

    return MSDK_PLUGINGUID_NULL;
}

bool AreGuidsEqual(const mfxPluginUID& guid1, const mfxPluginUID& guid2)
{
    for (size_t i = 0; i != sizeof(mfxPluginUID); i++)
    {
        if (guid1.Data[i] != guid2.Data[i])
            return false;
    }
    return true;
}

char* ConvertGuidToString(const mfxPluginUID& guid)
{
    static char szGuid[256] = { 0 };

    for (size_t i = 0; i != sizeof(mfxPluginUID); i++)
    {
        sprintf(&szGuid[2 * i], "%02x", guid.Data[i]);
    }

    return szGuid;
}

mfxStatus ConvertFrameRate(mfxF64 dFrameRate, mfxU32* pnFrameRateExtN, mfxU32* pnFrameRateExtD)
{
    MSDK_CHECK_POINTER(pnFrameRateExtN, MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(pnFrameRateExtD, MFX_ERR_NULL_PTR);

    mfxU32 fr;

    fr = (mfxU32)(dFrameRate + .5);

    if (fabs(fr - dFrameRate) < 0.0001)
    {
        *pnFrameRateExtN = fr;
        *pnFrameRateExtD = 1;
        return MFX_ERR_NONE;
    }

    fr = (mfxU32)(dFrameRate * 1.001 + .5);

    if (fabs(fr * 1000 - dFrameRate * 1001) < 10)
    {
        *pnFrameRateExtN = fr * 1000;
        *pnFrameRateExtD = 1001;
        return MFX_ERR_NONE;
    }

    *pnFrameRateExtN = (mfxU32)(dFrameRate * 10000 + .5);
    *pnFrameRateExtD = 10000;

    return MFX_ERR_NONE;
}
