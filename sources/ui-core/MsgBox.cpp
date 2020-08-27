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

/** \file MsgBox.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "MsgBox.h"
#include "ui-core/ToolTipCtrlEx.h"

int MSGBOX_API SECUMessageBox(LPCTSTR lpszText, UINT nType /*= MB_OK*/, UINT nIDHelp /*= 0*/)
{
 int result = AfxMessageBox(lpszText, nType, nIDHelp);
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}

int MSGBOX_API SECUMessageBox(UINT nIDPrompt, UINT nType /*= MB_OK*/, UINT nIDHelp /*= (UINT) -1*/)
{
 int result = AfxMessageBox(nIDPrompt, nType, nIDHelp);
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}

int MSGBOX_API SECUMessageBox(const CString& text, UINT nType /*= MB_OK*/, UINT nIDHelp /*= 0*/)
{
 int result = AfxMessageBox((LPCTSTR)text, nType, nIDHelp);
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}
