 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "STCAbout.h"
#include "secu-3about.h"
#include "..\common\unicodesupport.h"

extern HINSTANCE hInstance;  //DLLs hInstance

void ABOUT_API DisplayAbout(CWnd* i_pParent)
{
  USES_CONVERSION;
  CString string; CString templ = MLL::LoadString(IDS_SOFTWARE_INFO);
  string.Format(templ, A2T(__DATE__)); //compiler uses only ASCII format...

  AboutStc(i_pParent, hInstance, (LPCTSTR)IDB_BITMAP0001, (LPCTSTR)IDR_RGN0001,
	  string, MLL::LoadString(IDS_AUTHOR_INFO));
}
