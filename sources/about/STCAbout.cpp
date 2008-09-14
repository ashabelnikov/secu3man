 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "STCAbout.h"

struct About_data
{
 TCHAR about_product[1024];
 TCHAR about_description[1024];
 LPCTSTR  d_lprgnname;
 HINSTANCE hInstance;
}d_about; //singleton!


#define ABOUT_STC_RGN_RESOURCE_TYPE _T("RGN")
 

static bool is_instance = false; //singleton :-)


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// ----------------------------------------------------------------------------------------
// NOTE: Resource type of rgnName must be of type ABOUT_STC_RGN_RESOURCE_TYPE
// ----------------------------------------------------------------------------------------
void AboutStc(CWnd* i_pParent, HINSTANCE i_hInstance,LPCTSTR bmpName,LPCTSTR rgnName,LPCTSTR product, LPCTSTR description)
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

  wcex.cbSize         = sizeof(WNDCLASSEX); 
  wcex.style			= CS_HREDRAW|CS_VREDRAW|CS_BYTEALIGNWINDOW;
  wcex.lpfnWndProc	= (WNDPROC)WndProc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= i_hInstance;
  wcex.hIcon			= NULL;
  wcex.hCursor		= ::LoadCursor(NULL, IDC_CROSS);
  wcex.hbrBackground	=  CreatePatternBrush((HBITMAP)hWindowBitmap);
  wcex.lpszMenuName	= NULL;
  wcex.lpszClassName	= szWindowClass;
  wcex.hIconSm		= NULL;
  RegisterClassEx(&wcex);

  sx = GetSystemMetrics(SM_CXSCREEN);
  sy = GetSystemMetrics(SM_CYSCREEN);
  sx = (sx/2)-bm.bmWidth/2;
  sy = (sy/2)-bm.bmHeight/2;

  hWnd = CreateWindowEx(WS_EX_LEFT,
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
	AfxMessageBox(_T("Can't create window"),MB_OK|MB_ICONSTOP);
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
  static  CFont   fntTitle;
  static  CFont   fntDescr;
  static  HWND    hCloseButton;
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
                WS_CHILD | WS_VISIBLE,
                103,66,45,16,hWnd,(HMENU)1200,
				d_about.hInstance,NULL);


	  fntDescr.CreateFont(
      12,                        // nHeight
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
      14,                        // nHeight
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

	  break;
	case WM_LBUTTONDOWN:
      SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,lParam); 		
	  break;
    case WM_COMMAND:
      if (LOWORD(wParam)==1200)
	    DestroyWindow(hWnd);
	  break;
	case WM_DESTROY:
      fntTitle.DeleteObject();
      fntDescr.DeleteObject();
	  //we again ready for create a new instance!
	  is_instance = false;
	  break;
	case WM_KILLFOCUS:
	  if ((HWND)wParam!=hCloseButton)
	    SetFocus(hWnd);
	  break;
    case WM_PAINT:
      hdc = BeginPaint(hWnd,&pt);
      SetBkColor(hdc,RGB(0,0,0));
	  //draw title
	  SetTextColor(hdc,RGB(40,250,50));
      SelectObject(hdc,fntTitle.m_hObject);
	  TextOut(hdc,25,12,d_about.about_product,_tcslen(d_about.about_product));
      //draw description
      SelectObject(hdc,fntDescr.m_hObject);
	  SetTextColor(hdc,RGB(250,240,250));
	  DrawText(hdc,d_about.about_description,-1,CRect(25,30,230,70),DT_EDITCONTROL |DT_WORD_ELLIPSIS);

      EndPaint(hWnd,&pt); 
	  break;
	default:
	  return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// ----------------------------------------------------------------------------------------
