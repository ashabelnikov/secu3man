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

/** \file FirmwareMasterCntr.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"

#include <algorithm>
#include <set>
#include <shlwapi.h>
#include "common/FastDelegate.h"
#include "FirmwareMasterDlg.h"
#include "FirmwareMasterCntr.h"
#include "hexutils/readhex.h"
#include "ui-core/MsgBox.h"
//LZMA SDK:
#include "7z/CpuArch.h"
#include "7z/7z.h"
#include "7z/7zAlloc.h"
#include "7z/7zBuf.h"
#include "7z/7zCrc.h"
#include "7z/7zFile.h"

using namespace fastdelegate;

FirmwareMasterCntr::FirmwareMasterCntr()
: mp_view(new CFirmwareMasterDlg())
, mp_data(NULL)
, mp_file_name(NULL)
, m_status(false)
{
 ASSERT(mp_view);

 //set event handlers to catch events from view
 mp_view->setOnOkButton(MakeDelegate(this,&FirmwareMasterCntr::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&FirmwareMasterCntr::OnCancelPressed));
 mp_view->setOnActivate(MakeDelegate(this,&FirmwareMasterCntr::OnViewActivate));
 mp_view->setOnChangeFwmCheck(MakeDelegate(this,&FirmwareMasterCntr::OnChangeFwmCheck));
 mp_view->setOnChangeUnit(MakeDelegate(this,&FirmwareMasterCntr::OnChangeUnit));
 mp_view->setOnChangeSync(MakeDelegate(this,&FirmwareMasterCntr::OnChangeSync));
 mp_view->setOnFuseLink(MakeDelegate(this,&FirmwareMasterCntr::OnFuseLink));
}

bool FirmwareMasterCntr::GetStatus(void)
{
 return m_status;
}

int FirmwareMasterCntr::DoLoad(const _TSTRING& procStr, BYTE* p_data, size_t size, _TSTRING* o_file_name /*= NULL*/)
{
 mp_data = p_data;
 m_size = size;
 mp_file_name = o_file_name;
 m_procStr = procStr;
 return mp_view->DoModal();
}

FirmwareMasterCntr::~FirmwareMasterCntr()
{
 delete mp_view; //avoid memory leak
}

void FirmwareMasterCntr::OnOkPressed(void)
{
 std::set<_TSTRING> opts;
 _BuildOptList(opts);
 AfxGetMainWnd()->BeginWaitCursor();
 m_status = _LoadFirmware(opts); //load and store result
 AfxGetMainWnd()->EndWaitCursor();
}

void FirmwareMasterCntr::OnCancelPressed(void)
{
 //empty
}

void FirmwareMasterCntr::OnViewActivate(void)
{
 mp_view->FillUnitCombo();
 mp_view->FillSyncCombo();

 mp_view->SetUnitCombo(UNIT_SECU3T);
 mp_view->SetSyncCombo(SYNC_TRWLCRAN);

 //set check boxes to default values
 mp_view->SetFwmFlag(FWM_DWELL, true);
 mp_view->SetFwmFlag(FWM_PHIGN, false);
 mp_view->SetFwmFlag(FWM_INJ, false);
 mp_view->SetFwmFlag(FWM_TPIC, false);
 mp_view->SetFwmFlag(FWM_FPWM, true);
 mp_view->SetFwmFlag(FWM_2CHIG, false);
 mp_view->SetFwmFlag(FWM_OBD, false);
 mp_view->SetFwmFlag(FWM_CAFR, false);
 mp_view->SetFwmFlag(FWM_SPLIT, false);

 mp_view->EnableFwmFlag(FWM_TPIC, false);
}

void FirmwareMasterCntr::OnChangeFwmCheck(int id)
{
 bool state = mp_view->GetFwmFlag((FwmFlag)id);
 if (!state)
  return; //new state: unchecked
 switch(id)
 {
  case FWM_PHIGN: 
   mp_view->SetFwmFlag(FWM_2CHIG, false);
   break;
  case FWM_2CHIG:
   mp_view->SetFwmFlag(FWM_PHIGN, false);
   mp_view->SetFwmFlag(FWM_DWELL, false);
   break;
  case FWM_INJ:
   mp_view->SetFwmFlag(FWM_CAFR, false);
   break;
  case FWM_CAFR:
   mp_view->SetFwmFlag(FWM_INJ, false);
   break;
  case FWM_DWELL:
   mp_view->SetFwmFlag(FWM_2CHIG, false);
   break;
 }
}

void FirmwareMasterCntr::OnChangeUnit(void)
{
 if (mp_view->GetUnitCombo() != UNIT_SECU3I)
 { //TPIC8101 supported only in the SECU-3i
  mp_view->EnableFwmFlag(FWM_TPIC, false);
  mp_view->SetFwmFlag(FWM_TPIC, false);
 }
 else
 {
  mp_view->EnableFwmFlag(FWM_TPIC, true);
 }

 if (mp_view->GetUnitCombo() == UNIT_SECU3TR6)
 { //OBD is not supported in SECU-3t rev.6
  mp_view->EnableFwmFlag(FWM_OBD, false);
  mp_view->SetFwmFlag(FWM_OBD, false);
 }
 else
 {
  mp_view->EnableFwmFlag(FWM_OBD, true);
 }
}

void FirmwareMasterCntr::OnChangeSync(void)
{
 if (mp_view->GetSyncCombo() != SYNC_TRWLCRAN)
 {
  mp_view->EnableFwmFlag(FWM_2CHIG, false);
  mp_view->SetFwmFlag(FWM_2CHIG, false);
 }
 else
 {
  mp_view->EnableFwmFlag(FWM_2CHIG, true);
 }

 if (mp_view->GetSyncCombo() == SYNC_DISTHALL || mp_view->GetSyncCombo() == SYNC_NP1CRAN)
 {
  mp_view->EnableFwmFlag(FWM_PHIGN, false);
  mp_view->SetFwmFlag(FWM_PHIGN, false);
  mp_view->EnableFwmFlag(FWM_SPLIT, false); //disable splitting
  mp_view->SetFwmFlag(FWM_SPLIT, false);
 }
 else
 {
  mp_view->EnableFwmFlag(FWM_PHIGN, true);
  mp_view->EnableFwmFlag(FWM_SPLIT, true); //enable splitting
 }
}

void FirmwareMasterCntr::_BuildOptList(std::set<_TSTRING> &opts)
{
 //fill list with all possible options:
 m_allopts.clear();
 m_allopts.insert(_T("secu-3i"));
 m_allopts.insert(_T("secu-3t"));
 m_allopts.insert(_T("rev6"));
 m_allopts.insert(_T("rev9"));
 m_allopts.insert(_T("cams"));
 m_allopts.insert(_T("hall"));
 m_allopts.insert(_T("ckpnp1"));
 m_allopts.insert(_T("dwell"));
 m_allopts.insert(_T("phign"));
 m_allopts.insert(_T("inj"));
 m_allopts.insert(_T("tpic"));
 m_allopts.insert(_T("fpwm"));
 m_allopts.insert(_T("2chig"));
 m_allopts.insert(_T("obd"));
 m_allopts.insert(_T("cafr"));
 m_allopts.insert(_T("split"));

 //fill list, which will contain only options selected by user:
 opts.clear();
 //unit:
 if (mp_view->GetUnitCombo() == UNIT_SECU3I)
  opts.insert(_T("secu-3i"));
 else if (mp_view->GetUnitCombo() == UNIT_SECU3TR6)
 {
  opts.insert(_T("secu-3t"));
  opts.insert(_T("rev6"));
 }
 else
 {
  opts.insert(_T("secu-3t"));
  opts.insert(_T("rev9"));
 }

 //sync:
 int sync = mp_view->GetSyncCombo();
 if (sync == SYNC_TRWLCAMS)
  opts.insert(_T("cams"));
 else if (sync == SYNC_DISTHALL)
  opts.insert(_T("hall"));
 else if (sync == SYNC_NP1CRAN)
  opts.insert(_T("ckpnp1"));

 //cheks:
 if (mp_view->GetFwmFlag(FWM_DWELL))
  opts.insert(_T("dwell"));
 if (mp_view->GetFwmFlag(FWM_PHIGN))
  opts.insert(_T("phign"));
 if (mp_view->GetFwmFlag(FWM_INJ))
  opts.insert(_T("inj"));
 if (mp_view->GetFwmFlag(FWM_TPIC))
  opts.insert(_T("tpic"));
 if (mp_view->GetFwmFlag(FWM_FPWM))
  opts.insert(_T("fpwm"));
 if (mp_view->GetFwmFlag(FWM_2CHIG))
  opts.insert(_T("2chig"));
 if (mp_view->GetFwmFlag(FWM_OBD))
  opts.insert(_T("obd"));
 if (mp_view->GetFwmFlag(FWM_CAFR))
  opts.insert(_T("cafr"));
 if (mp_view->GetFwmFlag(FWM_SPLIT))
  opts.insert(_T("split"));
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

bool FirmwareMasterCntr::_LoadFirmware(const std::set<_TSTRING>& opts)
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

 _TSTRING fnstr;
 if (mp_view->GetFwmFlag(FWM_SPLIT))
  fnstr = _T("secu3app-") + m_procStr + _T("-split") + _T(".7z");
 else
  fnstr = _T("secu3app-") + m_procStr + _T(".7z");

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
    store_data = _StoreResult((const TCHAR*)buf.data, outBuffer + offset, outSizeProcessed);
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

bool FirmwareMasterCntr::_FindMatchForOpts(const _TSTRING& name, const std::set<_TSTRING>& opts)
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

bool FirmwareMasterCntr::_StoreResult(const _TSTRING& name, BYTE* buffHex, size_t sizeHex)
{
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

 *mp_file_name = name;
 return true;
}

void FirmwareMasterCntr::OnFuseLink(void)
{
 SECUMessageBox(IDS_FWM_FUSEBITS_SECU, MB_OK | MB_ICONINFORMATION);
}
