/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include "stdafx.h"
#include "ChildView.h"

//warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable : 4800 )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
 //empty
}

CChildView::~CChildView()
{
 //empty
}

BEGIN_MESSAGE_MAP(CChildView, Super)
 //empty
END_MESSAGE_MAP()

//Создает диалог из шаблона в памяти.
bool CChildView::Create(CWnd* ip_parent)
{
 HGLOBAL hgbl;
 LPDLGTEMPLATE lpdt;
 LPWORD lpw;
 LPWSTR lpwsz;
 int nchar;

 hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
 ASSERT(hgbl);
 lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

 // Define a dialog box.
 lpdt->style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
 lpdt->dwExtendedStyle = WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE;
 lpdt->cdit = 0; // number of controls
 lpdt->x  = 0;   //<--любые дефайлтные размеры,
 lpdt->y  = 0;   //фреймворк потом подгонит размеры автоматически.
 lpdt->cx = 0;
 lpdt->cy = 0;

 lpw = (LPWORD) (lpdt + 1);
 *lpw++ = 0;   // no menu
 *lpw++ = 0;   // predefined dialog box class (by default)

 lpwsz = (LPWSTR) lpw;
 nchar = 1 + MultiByteToWideChar (CP_ACP, 0, "View", -1, lpwsz, 50);
 lpw   += nchar;

 GlobalUnlock(hgbl);
 BOOL result = CreateIndirect((LPDLGTEMPLATE) hgbl, ip_parent);
 GlobalFree(hgbl);
 return result;
}
