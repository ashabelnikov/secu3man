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

/** \file STCAbout.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "STCAbout.h"

struct About_data
{
 TCHAR about_product[1024];
 TCHAR about_description[1024];
 LPCTSTR  d_lprgnname;
 HINSTANCE hInstance;
 BOOL showCloseButton;
 BOOL modal; //modal flag
 int autoClose;
}d_about; //singleton!

#define AUTOCLOSE_TIMER_ID 1
#define ABOUT_STC_RGN_RESOURCE_TYPE _T("RGN")

static bool is_instance = false; //singleton :-)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ----------------------------------------------------------------------------------------
// NOTE: Resource type of rgnName must be of type ABOUT_STC_RGN_RESOURCE_TYPE
// ----------------------------------------------------------------------------------------
void AboutStc(CWnd* i_pParent, HINSTANCE i_hInstance,LPCTSTR bmpName,LPCTSTR rgnName,LPCTSTR product, LPCTSTR description, bool splashScreen, int timeToShow)
{
 static WNDCLASSEX wcex;
 TCHAR  szWindowClass[100];
 static HWND       hWnd;
 static BITMAP     bm;
 static HBITMAP    hWindowBitmap = NULL;
 int sx,sy;

 if (is_instance)
  return;  //only one instance allowed!

 d_about.d_lprgnname = rgnName;
 d_about.hInstance = i_hInstance;

 //following parameters depends on splash screen mode
 d_about.showCloseButton = splashScreen ? false : true;
 d_about.modal = splashScreen ? false : true;
 d_about.autoClose = splashScreen ? timeToShow : -1;

 hWindowBitmap = LoadBitmap(i_hInstance,bmpName);
 if (NULL==hWindowBitmap)
 {
  AfxMessageBox(_T("Can't load bitmap"),MB_OK|MB_ICONSTOP);
  return;
 }

 wsprintf(szWindowClass,_T("%s%d"),_T("STC_about"),i_pParent);

 _tcscpy(d_about.about_product,product);
 _tcscpy(d_about.about_description,description);

 GetObject(hWindowBitmap,sizeof(BITMAP),&bm);

 wcex.cbSize        = sizeof(WNDCLASSEX);
 wcex.style         = CS_HREDRAW|CS_VREDRAW|CS_BYTEALIGNWINDOW;
 wcex.lpfnWndProc   = (WNDPROC)WndProc;
 wcex.cbClsExtra    = 0;
 wcex.cbWndExtra    = 0;
 wcex.hInstance     = i_hInstance;
 wcex.hIcon         = NULL;
 wcex.hCursor       = (HCURSOR)LoadImage(NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0, LR_SHARED);
 wcex.hbrBackground = CreatePatternBrush((HBITMAP)hWindowBitmap);
 wcex.lpszMenuName  = NULL;
 wcex.lpszClassName = szWindowClass;
 wcex.hIconSm       = NULL;
 RegisterClassEx(&wcex);

 sx = GetSystemMetrics(SM_CXSCREEN);
 sy = GetSystemMetrics(SM_CYSCREEN);
 sx = (sx / 2) - bm.bmWidth / 2;
 sy = (sy / 2) - bm.bmHeight / 2;

 hWnd = CreateWindowEx(WS_EX_LEFT | WS_EX_TOPMOST,
          szWindowClass,
          _T("About"),
          WS_POPUP,
          sx,
          sy,
          bm.bmWidth,
          bm.bmHeight,
          i_pParent->m_hWnd,
          NULL,
          i_hInstance,
          NULL);

 if (hWnd == NULL)
 {
  AfxMessageBox(_T("Can't create window"),MB_OK | MB_ICONSTOP);
  return;
 }

 ShowWindow(hWnd,SW_SHOWNORMAL);
 UpdateWindow(hWnd);

 //set flag wich indicates that instance has been created!
 is_instance = true;
}

// ----------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 HRSRC   hResource;
 HGLOBAL resPoints;
 DWORD   resSize;
 LPVOID  rgnPoints;
 HRGN    hWindowRegion;
 static  CFont fntTitle;
 static  CFont fntDescr;
 static  HWND  hCloseButton = NULL;
 HDC     hdc;
 PAINTSTRUCT pt;

 switch (message)
 {
  case WM_CREATE:
   hResource = FindResource(d_about.hInstance,d_about.d_lprgnname,ABOUT_STC_RGN_RESOURCE_TYPE);
   if (NULL==hResource)
    AfxMessageBox(_T("Can't find resource!"),MB_OK|MB_ICONSTOP);

   resPoints = LoadResource(d_about.hInstance,hResource);
   if (NULL==resPoints)
    AfxMessageBox(_T("Can't load resource!"),MB_OK|MB_ICONSTOP);

   resSize = SizeofResource(d_about.hInstance,hResource);
   if (0==resSize)
    AfxMessageBox(_T("Size of resource = 0 !"),MB_OK|MB_ICONSTOP);

   rgnPoints = LockResource(resPoints);
   hWindowRegion = ExtCreateRegion(NULL,resSize,(RGNDATA*)rgnPoints);
   SetWindowRgn(hWnd,hWindowRegion,TRUE);

   hCloseButton = CreateWindowEx(NULL,_T("button"),_T("close"),
                WS_CHILD | (d_about.showCloseButton ? WS_VISIBLE : 0),
                280,186,45,16,hWnd,(HMENU)IDC_BUTTON_CLOSE,
                d_about.hInstance,NULL);

   if (d_about.modal)
    ::SetFocus(hCloseButton);

   fntDescr.CreateFont(
   -9,                        // nHeight
   0,                         // nWidth
   0,                         // nEscapement
   0,                         // nOrientation
   FW_NORMAL,                 // nWeight
   FALSE,                     // bItalic
   FALSE,                     // bUnderline
   0,                         // cStrikeOut
   ANSI_CHARSET,              // nCharSet
   OUT_DEFAULT_PRECIS,        // nOutPrecision
   CLIP_DEFAULT_PRECIS,       // nClipPrecision
   DEFAULT_QUALITY,           // nQuality
   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
   _T("Arial"));

   fntTitle.CreateFont(
   -11,                       // nHeight
   0,                         // nWidth
   0,                         // nEscapement
   0,                         // nOrientation
   FW_NORMAL,                 // nWeight
   FALSE,                     // bItalic
   FALSE,                     // bUnderline
   0,                         // cStrikeOut
   ANSI_CHARSET,              // nCharSet
   OUT_DEFAULT_PRECIS,        // nOutPrecision
   CLIP_DEFAULT_PRECIS,       // nClipPrecision
   DEFAULT_QUALITY,           // nQuality
   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
   _T("Arial"));

   SendMessage(hCloseButton,WM_SETFONT,(LPARAM)fntDescr.m_hObject,TRUE);

   if (d_about.autoClose > 0)
     SetTimer(hWnd, AUTOCLOSE_TIMER_ID, d_about.autoClose, NULL);
   break;
  case WM_LBUTTONDOWN:
   SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,lParam);
   break;
  case WM_COMMAND:
   if (LOWORD(wParam)==IDC_BUTTON_CLOSE)
    DestroyWindow(hWnd);
   break;
  case WM_DESTROY:
   fntTitle.DeleteObject();
   fntDescr.DeleteObject();
   //we again ready for create a new instance!
   is_instance = false;
   break;
  case WM_KILLFOCUS:
   if (d_about.modal)
    if ((HWND)wParam!=hCloseButton)
     SetFocus(hWnd);
   break;
  case WM_PAINT:
   hdc = BeginPaint(hWnd,&pt);
   SetBkColor(hdc,RGB(0,0,0));
   //draw title
   SetTextColor(hdc,RGB(40,250,50));
   SelectObject(hdc,fntTitle.m_hObject);
   TextOut(hdc,195,16,d_about.about_product,_tcslen(d_about.about_product));
   //draw description
   SelectObject(hdc,fntDescr.m_hObject);
   SetTextColor(hdc,RGB(250,240,250));
   SetBkMode(hdc, TRANSPARENT);
   DrawText(hdc,d_about.about_description,-1,CRect(203,45,433,180),DT_EDITCONTROL |DT_WORD_ELLIPSIS);

   EndPaint(hWnd,&pt);
   break;
  case WM_TIMER:
   ASSERT(KillTimer(hWnd, AUTOCLOSE_TIMER_ID));
   DestroyWindow(hWnd);
   break;
  default:
   return DefWindowProc(hWnd, message, wParam, lParam);
 }
 return 0;
}

// ----------------------------------------------------------------------------------------
