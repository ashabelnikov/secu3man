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

#include "stdafx.h"
#include "ParamTabBaseDlg.h"

CParamTabBaseDlg::CParamTabBaseDlg(UINT nIDTemplate, CWnd* pParentWnd /* = NULL*/)
: Super(nIDTemplate, pParentWnd)
{
 //empty
}

BOOL CParamTabBaseDlg::PreTranslateMessage(MSG* pMsg)
{
 //MS says:
 //The default WM_LBUTTONDOWN handler for edit controls will not set focus to the edit control if its parent
 //is an inactive captioned child window. This code is implemented as part of the Windows API function, DefWindowProc().
 //This behavior is by design. 
/*
 //This code is needed only when parent window of control has WS_CHILD style and has caption bar
 if (pMsg->message==WM_LBUTTONDOWN)
 {
  TCHAR name[32];
  GetClassName(pMsg->hwnd, name, 31);
  //Set focus for edit controls which belong only to this dialog
  if (!_tcscmp(name, _T("Edit")) && ::GetParent(pMsg->hwnd) == this->m_hWnd)
   ::SetFocus(pMsg->hwnd);
 }*/

 return Super::PreTranslateMessage(pMsg);
}
