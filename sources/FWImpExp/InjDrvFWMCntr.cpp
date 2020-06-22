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

/** \file InjDrvFWMCntr.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"

#include <algorithm>
#include <set>
#include <shlwapi.h>
#include "common/FastDelegate.h"
#include "InjDrvFWMDlg.h"
#include "InjDrvFWMCntr.h"
#include "hexutils/readhex.h"
#include "ui-core/XBrowseForFolder.h"
#include "ui-core/MsgBox.h"
//LZMA SDK:
#include "7z/CpuArch.h"
#include "7z/7z.h"
#include "7z/7zAlloc.h"
#include "7z/7zBuf.h"
#include "7z/7zCrc.h"
#include "7z/7zFile.h"

using namespace fastdelegate;

InjDrvFWMCntr::InjDrvFWMCntr()
: mp_view(new CInjDrvFWMDlg())
, mp_data(NULL)
, m_status(false)
{
 ASSERT(mp_view);

 //set event handlers to catch events from view
 mp_view->setOnOkButton(MakeDelegate(this,&InjDrvFWMCntr::OnOkPressed));
 mp_view->setOnSaveButton(MakeDelegate(this,&InjDrvFWMCntr::OnSavePressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&InjDrvFWMCntr::OnCancelPressed));
 mp_view->setOnActivate(MakeDelegate(this,&InjDrvFWMCntr::OnViewActivate));
 mp_view->setOnChangeFwmCheck(MakeDelegate(this,&InjDrvFWMCntr::OnChangeFwmCheck));
}

bool InjDrvFWMCntr::GetStatus(void)
{
 return m_status;
}

int InjDrvFWMCntr::DoLoad(BYTE* p_data, size_t size)
{
 mp_data = p_data;
 m_size = size;
 return mp_view->DoModal();
}

InjDrvFWMCntr::~InjDrvFWMCntr()
{
 delete mp_view; //avoid memory leak
}

void InjDrvFWMCntr::OnOkPressed(void)
{
 std::set<_TSTRING> opts;
 int procId = mp_view->GetProcCombo();
 _BuildOptList(opts, procId);
 AfxGetMainWnd()->BeginWaitCursor();
 m_status = _LoadFirmware(opts, procId, false); //load and store result
 AfxGetMainWnd()->EndWaitCursor();
}

void InjDrvFWMCntr::OnSavePressed(void)
{
 std::set<_TSTRING> opts;
 int procId = mp_view->GetProcCombo();
 _BuildOptList(opts, procId);
 mp_view->EndDialog(IDCANCEL); //close window
 m_status = _LoadFirmware(opts, procId, true); //load and save result on disk
}

void InjDrvFWMCntr::OnCancelPressed(void)
{
 //empty
}

void InjDrvFWMCntr::OnViewActivate(void)
{
 mp_view->FillProcCombo();

 mp_view->SetProcCombo(PROC_328PB); 

 //set check boxes to default values
 mp_view->SetFwmFlag(FWM_INPINV, false);
 mp_view->SetFwmFlag(FWM_FLBINV, false);
 mp_view->SetFwmFlag(FWM_PWMINV, false);
 mp_view->SetFwmFlag(FWM_PWCORR, false);
}

void InjDrvFWMCntr::OnChangeFwmCheck(int id)
{
 bool state = mp_view->GetFwmFlag((FwmFlag)id);
 if (!state)
  return; //new state: unchecked
 switch(id)
 {
  case FWM_INPINV: 
   break;
  case FWM_FLBINV:
   break;
  case FWM_PWMINV:
   break;
  case FWM_PWCORR:
   break;
 }
}

void InjDrvFWMCntr::_BuildOptList(std::set<_TSTRING> &opts, int procId)
{
 //fill list with all possible options:
 m_allopts.clear();
 _TSTRING seculzid_str = procId==PROC_328 ? _T("seculzid2") : _T("seculzid5");
 m_allopts.insert(seculzid_str);
 m_allopts.insert(_T("inpinv"));
 m_allopts.insert(_T("flbinv"));
 m_allopts.insert(_T("pwminv"));
 m_allopts.insert(_T("pwcorr"));

 //fill list, which will contain only options selected by user:
 opts.clear();
 opts.insert(seculzid_str);

 if (mp_view->GetFwmFlag(FWM_INPINV))
  opts.insert(_T("inpinv"));
 if (mp_view->GetFwmFlag(FWM_FLBINV))
  opts.insert(_T("flbinv"));
 if (mp_view->GetFwmFlag(FWM_PWMINV))
  opts.insert(_T("pwminv"));
 if (mp_view->GetFwmFlag(FWM_PWCORR))
  opts.insert(_T("pwcorr"));
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

bool InjDrvFWMCntr::_LoadFirmware(const std::set<_TSTRING>& opts, int procId, bool save)
{
 ISzAlloc allocImp;
 ISzAlloc allocTempImp;

 CFileInStream archiveStream;
 CLookToRead2 lookStream;
 CSzArEx db;
 SRes res = SZ_OK;
 UInt16 *temp = NULL;
 size_t tempSize = 0;
 bool match = false;

 allocImp = g_Alloc;
 allocTempImp = g_Alloc;

 _TSTRING fnstr = ((procId==PROC_328) ? _TSTRING(_T("secu-lzid2")) : _TSTRING(_T("secu-lzid5"))) + _T(".7z");

 //get current directory
 HMODULE hModule = GetModuleHandle(NULL);
 ASSERT(hModule);
 _tcscpy(m_current_directory,_T(""));
 GetModuleFileName(hModule, m_current_directory, MAX_PATH);
 VERIFY(PathRemoveFileSpec(m_current_directory));

 _TSTRING fullPath = _TSTRING(m_current_directory) + _T("\\") + fnstr;

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

 bool store_data = true;

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
   if (_FindMatchForOpts((const TCHAR*)buf.data, opts))  //our name!
   {
    res = SzArEx_Extract(&db, &lookStream.vt, i, &blockIndex, &outBuffer, &outBufferSize,
                         &offset, &outSizeProcessed, &allocImp, &allocTempImp);
    if (res != SZ_OK)
    {
     Buf_Free(&buf, &g_Alloc);
     break;
    }
    match = true;
    store_data = _StoreResult((const TCHAR*)buf.data, outBuffer + offset, outSizeProcessed, save);
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

 if (!store_data)
  return false; //error during storing of result

 if (res == SZ_OK && match)
  return true; //ok
 else if (res == SZ_OK && !match)
 {
  SECUMessageBox(_T("Firmware with specified options has not been found!\nPlease contact the author!"));
  return false;
 }
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

bool InjDrvFWMCntr::_FindMatchForOpts(const _TSTRING& name, const std::set<_TSTRING>& opts)
{
 //string must contain all options from the opts list
 for (std::set<_TSTRING>::const_iterator it = opts.begin(); it != opts.end(); ++it)
 {
  if (_TSTRING::npos==name.find(*it))
   return false;
 }
 std::vector<_TSTRING> diff(m_allopts.size());
 std::vector<_TSTRING>::iterator ed = std::set_difference(m_allopts.begin(), m_allopts.end(), opts.begin(), opts.end(), diff.begin());
 diff.resize(ed - diff.begin());

 //string must not contain any of options from the m_allopts list
 for (std::vector<_TSTRING>::const_iterator dt = diff.begin(); dt != diff.end(); ++dt)
 {
  if (_TSTRING::npos!=name.find(*dt))
   return false;
 }

 return true;
}

bool InjDrvFWMCntr::_StoreResult(const _TSTRING& name, BYTE* buffHex, size_t sizeHex, bool save)
{
 if (save)
 {
  TCHAR szFolder[MAX_PATH*2] = { _T('\0') };
  BOOL bRet = XBrowseForFolder(AfxGetMainWnd()->m_hWnd, (LPCTSTR)NULL, -1, MLL::LoadString(IDS_FWM_SELECT_FOLDER_FOR_FW),
                              szFolder, sizeof(szFolder) / sizeof(TCHAR) - 2, TRUE);
  if (bRet)
  {
   _TSTRING path = _TSTRING(szFolder) + _T("\\") + name;
   CFile f;
   CFileException ex;
   TCHAR szError[1024];
   if(!f.Open(path.c_str(), CFile::modeWrite | CFile::modeCreate, &ex))
   {
    ex.GetErrorMessage(szError, 1024);
    SECUMessageBox(szError);
    return false;
   }  
   f.Write(buffHex, sizeHex);
   f.Close();
  }
  return true;
 }

 size_t bin_size = 0;
 EReadHexStatus status = HexUtils_ConvertHexToBin(buffHex, sizeHex, mp_data, bin_size, m_size);

 switch(status)
 {
  case RH_INCORRECT_CHKSUM:
   SECUMessageBox(MLL::LoadString(IDS_FWM_HEX_FILE_CRC_ERROR));
   return false; //error
  default:
  case RH_UNEXPECTED_SYMBOL:
   SECUMessageBox(MLL::LoadString(IDS_FWM_HEX_FILE_STRUCTURE_ERROR));
   return false; //error
  case RH_ADDRESS_EXCEDED:
   {
    CString sz, str;
    sz.Format(_T("%d"), m_size);
    str.Format(MLL::LoadString(IDS_FWM_WRONG_FW_FILE_SIZE), sz);
    SECUMessageBox((LPCTSTR)str);   
    return false; //error, wrong size
   }
  case RH_SUCCESS:
   break;
  }

 return true;
}
