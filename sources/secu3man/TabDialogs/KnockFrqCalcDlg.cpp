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
#include "ui-core/DDX_helpers.h"

using namespace fastdelegate;

const UINT CKnockFrqCalcDlg::IDD = IDD_KC_KNOCK_FRQ_CALC;

const float MIN_CYL_DIAMETER = 28.7f;   //mm
const float MAX_CYL_DIAMETER = 470.0f;  //mm

//I hate math.h
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

CKnockFrqCalcDlg::CKnockFrqCalcDlg(CWnd* pParent /*=NULL*/)
: Super(CKnockFrqCalcDlg::IDD, pParent)
, m_cyl_d_edit(CEditEx::MODE_FLOAT)
, m_cyl_d(80.0f)
{
 //empty
}

void CKnockFrqCalcDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_KC_KNOCK_FRQ_CALC_BTN, m_calc_frq_btn);
 DDX_Control(pDX, IDC_KC_KNOCK_FRQ_CALC_EDIT, m_cyl_d_edit);

 m_cyl_d_edit.DDX_Value(pDX, IDC_KC_KNOCK_FRQ_CALC_EDIT, m_cyl_d);
}

BEGIN_MESSAGE_MAP(CKnockFrqCalcDlg, Super)
 ON_BN_CLICKED(IDC_KC_KNOCK_FRQ_CALC_BTN, OnFrqCalcButton)
END_MESSAGE_MAP()

BOOL CKnockFrqCalcDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_calc_frq_btn.LoadBitmaps(MAKEINTRESOURCE(IDB_CALC_UP), MAKEINTRESOURCE(IDB_CALC_DOWN), 
                            MAKEINTRESOURCE(IDB_CALC_FOCUSED), MAKEINTRESOURCE(IDB_CALC_DISABLED));
 m_cyl_d_edit.SetDecimalPlaces(1);
 m_cyl_d_edit.SetLimitText(5);
 m_cyl_d_edit.SetValue(m_cyl_d);

 UpdateDialogControls(this, TRUE);
 return TRUE;
}

void CKnockFrqCalcDlg::OnFrqCalcButton()
{
 UpdateData(TRUE);
 //check limits and correct value if required
 if (m_cyl_d > MAX_CYL_DIAMETER)
 {
  m_cyl_d = MAX_CYL_DIAMETER;
  UpdateData(FALSE);
 }
 if (m_cyl_d < MIN_CYL_DIAMETER)
 {
  m_cyl_d = MIN_CYL_DIAMETER;
  UpdateData(FALSE);
 }
 //calculate approximate frequency
 float frq = 900.0f / ((float)M_PI * (m_cyl_d / 2.0f));
 //delegate result
 if (m_OnCalculate)
  m_OnCalculate(frq);
}

void CKnockFrqCalcDlg::setOnCalculate(EventHandler OnFunction)
{
 m_OnCalculate = OnFunction;
}
