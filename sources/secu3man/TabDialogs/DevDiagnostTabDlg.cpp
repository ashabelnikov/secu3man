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
#include "Resources/resource.h"
#include "DevDiagnostTabDlg.h"

#include "common/FastDelegate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fastdelegate;

const UINT CDevDiagnostTabDlg::IDD = IDD_DEV_DIAGNOSTICS;

BEGIN_MESSAGE_MAP(CDevDiagnostTabDlg, Super)
END_MESSAGE_MAP()

CDevDiagnostTabDlg::CDevDiagnostTabDlg(CWnd* pParent /*=NULL*/)
: Super(CDevDiagnostTabDlg::IDD, pParent)
{
 //todo
}

void CDevDiagnostTabDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
}

LPCTSTR CDevDiagnostTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

BOOL CDevDiagnostTabDlg::OnInitDialog()
{
 Super::OnInitDialog();

 UpdateDialogControls(this,TRUE);
 return TRUE;
}
