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
#include "resource.h"
#include "secu-3about.h"

extern HINSTANCE hInstance;  //DLLs hInstance

void ABOUT_API DisplayAbout(CWnd* i_pParent)
{
  AboutStc(i_pParent, 
	  hInstance,
	  (LPCTSTR)IDB_BITMAP0001,
	  (LPCTSTR)IDR_RGN0001,
	  _T("SECU-3 Manager v2.0. Build [")__DATE__ _T("]"),
	  _T("(c) Shabelnikov Technology. UA, Gorlovka city\r\nICQ: 405-791-931, http://secu-3.narod.ru\r\n             ...alone from strange world...\r\n"));
}