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
#include "KnockFrqCalcDlg.h"

#include "common/FastDelegate.h"

using namespace fastdelegate;

const UINT CKnockFrqCalcDlg::IDD = IDD_KC_KNOCK_FRQ_CALC;

CKnockFrqCalcDlg::CKnockFrqCalcDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockFrqCalcDlg::IDD, pParent)
{
 //empty
}

void CKnockFrqCalcDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
// DDX_Control(pDX, IDC_, m_);
}

BEGIN_MESSAGE_MAP(CKnockFrqCalcDlg, Super)
// ON_UPDATE_COMMAND_UI(IDC_, OnUpdate)
// ON_BN_CLICKED(IDC_, On)
END_MESSAGE_MAP()

BOOL CKnockFrqCalcDlg::OnInitDialog()
{
 Super::OnInitDialog();

 UpdateDialogControls(this,TRUE);
 return TRUE;
}
