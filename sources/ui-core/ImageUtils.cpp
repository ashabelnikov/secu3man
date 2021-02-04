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

/** \file ImageUtils.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "ImageUtils.h"
#include "ui-core/FileDialogEx.h"

bool IMAGE_UTILS_API SaveScreenshot(CWnd* pWnd, bool selectFile)
{
 CRect rc;
 pWnd->GetWindowRect(&rc);
 CDC* pWndDC  = pWnd->GetWindowDC();
 CDC capDC;
 capDC.CreateCompatibleDC(pWndDC);
 CBitmap capBitmap;
 capBitmap.CreateCompatibleBitmap(pWndDC, rc.Width(), rc.Height());
 capDC.SelectObject(&capBitmap);
 capDC.BitBlt(0,0,rc.Width(), rc.Height(), pWndDC,0,0, SRCCOPY);
 //put information
 CString str = _T("Copyright (C) 2007-2021 A. Shabelnikov, http://secu3.org");
 capDC.SetTextColor(RGB(50,50,200));
 capDC.SetBkMode(TRANSPARENT);
 CSize size = capDC.GetTextExtent(str);
 int borderHeight = GetSystemMetrics(SM_CYSIZEFRAME) + 1;
 capDC.TextOut(rc.Width() / 2 - size.cx / 2, rc.Height() - size.cy - borderHeight, str);
 //put creation time
 CTime t = CTime::GetCurrentTime();
 str.Format(_T("%d/%d/%d-%d:%d:%d"), t.GetDay(), t.GetMonth(), t.GetYear(), t.GetHour(), t.GetMinute(), t.GetSecond());
 size = capDC.GetTextExtent(str);
 capDC.TextOut(rc.Width() / 2 - size.cx / 2, borderHeight, str);
 //save image
 str.Format(_T("%02d.%02d.%04d_%02d.%02d.%02d.png"), t.GetDay(), t.GetMonth(), t.GetYear(), t.GetHour(), t.GetMinute(), t.GetSecond());
 static TCHAR BASED_CODE szFilter[] = _T("PNG Files (*.png)|*.png||");
 CFileDialogEx save(FALSE,_T("png"), str, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
 INT_PTR result = IDOK;
 if (selectFile)
  result = save.DoModal();
 if (result)
 {
  CImage im;
  im.Attach((HBITMAP)capBitmap.Detach());
  im.Save(save.GetPathName(), Gdiplus::ImageFormatPNG);
 }
 return true;
}

bool LoadImageFromRes(HINSTANCE hinstance, LPCTSTR lpszResourceName, LPCTSTR resType, CImage* pImage)
{
 HRSRC hRib = ::FindResource(hinstance, lpszResourceName, resType);
 if (hRib == NULL)
  return false;

 HGLOBAL hGlobal = LoadResource(hinstance, hRib);
 if (hGlobal == NULL)
  return false;

 LPBYTE lpBuffer = (LPBYTE) ::LockResource(hGlobal);
 if (lpBuffer == NULL)
 {
  FreeResource(hGlobal);
  return false;
 }

 UINT resSize = ::SizeofResource(hinstance, hRib);
 HGLOBAL hRes = ::GlobalAlloc(GMEM_MOVEABLE, resSize);
 if (hRes != NULL)
 {
  IStream* pStream = NULL;
  LPVOID lpResBuffer = ::GlobalLock(hRes);
  ASSERT (lpResBuffer != NULL);
  memcpy(lpResBuffer, lpBuffer, resSize);
  HRESULT hResult = ::CreateStreamOnHGlobal(hRes, TRUE, &pStream);
  if (S_OK == hResult)
  {
   pImage->Load(pStream);
   pStream->Release();
   UnlockResource(hGlobal);
   FreeResource(hGlobal);
   return true;
  }
 }

 UnlockResource(hGlobal);
 FreeResource(hGlobal);
 return false;
}
