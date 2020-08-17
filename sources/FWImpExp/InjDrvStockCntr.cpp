/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file InjDrvStockCntr.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"

#include <shlwapi.h>
#include "common/FastDelegate.h"
#include "InjDrvStockDlg.h"
#include "InjDrvStockCntr.h"
#include "ui-core/MsgBox.h"
//LZMA SDK:
#include "7z/CpuArch.h"
#include "7z/7z.h"
#include "7z/7zAlloc.h"
#include "7z/7zBuf.h"
#include "7z/7zCrc.h"
#include "7z/7zFile.h"

using namespace fastdelegate;

InjDrvStockCntr::InjDrvStockCntr()
: mp_view(new CInjDrvStockDlg())
, mp_data(NULL)
, mp_size(NULL)
, m_status(false)
{
 ASSERT(mp_view);

 //set event handlers to catch events from view
 mp_view->setOnOkButton(MakeDelegate(this,&InjDrvStockCntr::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&InjDrvStockCntr::OnCancelPressed));
 mp_view->setOnActivate(MakeDelegate(this,&InjDrvStockCntr::OnViewActivate));
}

bool InjDrvStockCntr::GetStatus(void)
{
 return m_status;
}

int InjDrvStockCntr::DoLoad(BYTE* p_data, size_t* io_size)
{
 mp_data = p_data;
 mp_size = io_size;
 return mp_view->DoModal();
}

InjDrvStockCntr::~InjDrvStockCntr()
{
 delete mp_view; //avoid memory leak
}

void InjDrvStockCntr::OnOkPressed(void)
{
 AfxGetMainWnd()->BeginWaitCursor();
 m_status = _LoadPreset(true); //extract selected item
 AfxGetMainWnd()->EndWaitCursor();
}

void InjDrvStockCntr::OnCancelPressed(void)
{
 //empty
}

void InjDrvStockCntr::OnViewActivate(void)
{
 m_status = _LoadPreset(false); //load names without extraction
}

static const ISzAlloc g_Alloc = { SzAlloc, SzFree };
#define kInputBufSize ((size_t)1 << 18)

static int Buf_EnsureSize(CBuf *dest, size_t size)
{
 if (dest->size >= size)
  return 1;
 Buf_Free(dest, &g_Alloc);
 return Buf_Create(dest, size, &g_Alloc);
}

static SRes Utf16_To_Char(CBuf *buf, const UInt16 *s, UINT codePage)
{
 unsigned len = 0;
 for (len = 0; s[len] != 0; len++);
 {
  unsigned size = len * 3 + 100;
  if (!Buf_EnsureSize(buf, size))
   return SZ_ERROR_MEM;
  {
   buf->data[0] = 0;
   if (len != 0)
   {
    char defaultChar = '_';
    BOOL defUsed;
    unsigned numChars = 0;
    numChars = WideCharToMultiByte(codePage, 0, (LPCWSTR)s, len, (char *)buf->data, size, &defaultChar, &defUsed);
    if (numChars == 0 || numChars >= size)
     return SZ_ERROR_FAIL;
    buf->data[numChars] = 0;
   }
   return SZ_OK;
  }
 }
}

bool InjDrvStockCntr::_LoadPreset(bool extract)
{
 ISzAlloc allocImp;
 ISzAlloc allocTempImp;

 CFileInStream archiveStream;
 CLookToRead2 lookStream;
 CSzArEx db;
 SRes res = SZ_OK;
 UInt16 *temp = NULL;
 size_t tempSize = 0;

 allocImp = g_Alloc;
 allocTempImp = g_Alloc;

 int extIndex = 0;
 if (extract)
  extIndex = mp_view->GetSelectedIndex();  

 //get current directory
 HMODULE hModule = GetModuleHandle(NULL);
 ASSERT(hModule);
 _tcscpy(m_current_directory, _T(""));
 GetModuleFileName(hModule, m_current_directory, MAX_PATH);
 VERIFY(PathRemoveFileSpec(m_current_directory));

 _TSTRING fullPath = _TSTRING(m_current_directory) + _T("\\") + _T("lzid-presets.7z");

 if (InFile_Open(&archiveStream.file, fullPath.c_str()))
 {
  SECUMessageBox((_TSTRING(_T("Can't find and open \"")) + fullPath + _T("\" file!")).c_str());
  return false;
 }

 FileInStream_CreateVTable(&archiveStream);
 LookToRead2_CreateVTable(&lookStream, False);
 lookStream.buf = NULL;

 lookStream.buf = (Byte *)ISzAlloc_Alloc(&allocImp, kInputBufSize);
 if (!lookStream.buf)
  res = SZ_ERROR_MEM;
 else
 {
  lookStream.bufSize = kInputBufSize;
  lookStream.realStream = &archiveStream.vt;
  LookToRead2_Init(&lookStream);
 }

 CrcGenerateTable();
 SzArEx_Init(&db);

 bool store_res = true;

 if (res == SZ_OK)
 {
  res = SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocTempImp);

  UInt32 i;
  UInt32 blockIndex = 0xFFFFFFFF;
  Byte *outBuffer = 0;
  size_t outBufferSize = 0;

  for (i = 0; i < db.NumFiles; i++)
  {
   size_t offset = 0;
   size_t outSizeProcessed = 0;
   size_t len;
   if (SzArEx_IsDir(&db, i))
    continue; //skip directories

   len = SzArEx_GetFileNameUtf16(&db, i, NULL);

   if (len > tempSize)
   {
    SzFree(NULL, temp);
    tempSize = len;
    temp = (UInt16 *)SzAlloc(NULL, tempSize * sizeof(temp[0]));
    if (!temp)
    {
     res = SZ_ERROR_MEM;
     break;
    }
   }

   SzArEx_GetFileNameUtf16(&db, i, temp);
   //convert UTF16 => char string
   CBuf buf; SRes res;
   Buf_Init(&buf);
   res = Utf16_To_Char(&buf, temp, CP_OEMCP);
   if (res != SZ_OK)
   {
    Buf_Free(&buf, &g_Alloc);
    break;
   }

   if (!extract)
   { //just append list with name of file
    mp_view->InsertItem((const TCHAR*)buf.data);
   }
   else if (i == extIndex)  //our selected item!
   { //extract selected item to buffer
    res = SzArEx_Extract(&db, &lookStream.vt, i, &blockIndex, &outBuffer, &outBufferSize,
                         &offset, &outSizeProcessed, &allocImp, &allocTempImp);
    if (res != SZ_OK)
    {
     Buf_Free(&buf, &g_Alloc);
     break;
    }
    //store contents of extracted file
    if (mp_size && outSizeProcessed <= *mp_size)
    {
     std::copy(outBuffer + offset, outBuffer + offset + outSizeProcessed, mp_data); //data
     *mp_size = outSizeProcessed; //size of data
    }
    else
     store_res = false;
    Buf_Free(&buf, &g_Alloc);
    break;
   }
   Buf_Free(&buf, &g_Alloc);
  }
  ISzAlloc_Free(&allocImp, outBuffer);
 } //ok?

 SzFree(NULL, temp);
 SzArEx_Free(&db, &allocImp);
 ISzAlloc_Free(&allocImp, lookStream.buf);
 File_Close(&archiveStream.file);

 if (!store_res)
  return false; //error during storing of result

 if (res == SZ_OK)
  return true; //ok
 else if (res == SZ_ERROR_UNSUPPORTED)
  SECUMessageBox(_T("Decoder doesn't support this archive!"));
 else if (res == SZ_ERROR_MEM)
  SECUMessageBox(_T("Can't allocate memory for extraction!"));
 else if (res == SZ_ERROR_CRC)
  SECUMessageBox(_T("CRC error!"));
 else
 {
  TCHAR str[32];
  _itot(res, str, 10);
  SECUMessageBox(str);
 }

 return false;
}
