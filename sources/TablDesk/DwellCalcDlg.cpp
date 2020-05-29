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

/** \file DwellCalcDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <float.h>
#include <math.h>
#include "resource.h"
#include "DwellCalcDlg.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CDwellCalcDlg::IDD = IDD_DWELL_CALC;

/////////////////////////////////////////////////////////////////////////////
// CDwellCalcDlg dialog

BEGIN_MESSAGE_MAP(CDwellCalcDlg, Super)
END_MESSAGE_MAP()

CDwellCalcDlg::CDwellCalcDlg(CWnd* pParent /*=NULL*/)
: Super(CDwellCalcDlg::IDD, pParent)
, m_r_edit(CEditEx::MODE_FLOAT)
, m_l_edit(CEditEx::MODE_FLOAT)
, m_i_edit(CEditEx::MODE_FLOAT)
, m_r(0.5f), m_l(5.0f), m_i(8.0f)
, mp_v_values(NULL)
, mp_t_values(NULL)
, m_size(0)
, m_t_lo_limit(0.25f)
, m_t_hi_limit(16.0f)
{
 //empty
}

CDwellCalcDlg::~CDwellCalcDlg()
{
 delete mp_v_values;
 delete mp_t_values;
}

void CDwellCalcDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_DWELL_CALC_R_EDIT, m_r_edit);
 DDX_Control(pDX, IDC_DWELL_CALC_R_SPIN, m_r_spin);
 DDX_Control(pDX, IDC_DWELL_CALC_L_EDIT, m_l_edit);
 DDX_Control(pDX, IDC_DWELL_CALC_L_SPIN, m_l_spin);
 DDX_Control(pDX, IDC_DWELL_CALC_I_EDIT, m_i_edit);
 DDX_Control(pDX, IDC_DWELL_CALC_I_SPIN, m_i_spin);

 m_r_edit.DDX_Value(pDX, IDC_DWELL_CALC_R_EDIT, m_r);
 m_l_edit.DDX_Value(pDX, IDC_DWELL_CALC_L_EDIT, m_l);
 m_i_edit.DDX_Value(pDX, IDC_DWELL_CALC_I_EDIT, m_i);
}

/////////////////////////////////////////////////////////////////////////////
// CDwellCalcDlg message handlers

void CDwellCalcDlg::OnOK()
{
 if (mp_t_values && mp_v_values)
 {
  UpdateData(TRUE);
  double R = m_r, L = m_l, I = m_i;
  for(size_t i = 0; i < m_size; ++i)
  {
   /*
   double U = mp_v_values[i] - 1.5; //compensate voltage drop on the transistor + drop on wires
   if (U < 1.0) U = 1.0;            //prevent div. by zero case
   if (R < 0.01) R = 0.01;          //prevent div. by zero case
   double x = 1.0 - (R * I / U);
   if (x <= 0) x = DBL_EPSILON;     //prevent U/R <= I case
   mp_t_values[i] = static_cast<float>((-L / R) * log(x));
   */

   /*
   if (R < 0.01) R = 0.01;          //prevent div. by zero case
   double U = mp_v_values[i] - 1.5; //compensate voltage drop on the transistor + drop on wires
   if (U < 0.01) U = 0.01;          //prevent div. by zero case
   double x = 1.0 - (R * I / U);
   if (x <= 0) x = DBL_EPSILON;     //prevent floating point error
   mp_t_values[i] = (float)((L / R) * log(1.0 / x));
   */

   //basic equation: U = L * (dI / dt);
   if (R < 0.01) R = 0.01;          //prevent div. by zero case
   double U = mp_v_values[i] - (1.5 + (R * I)); //compensate voltage drop on the transistor + drop on wires + drop on the active resistance of coil
   if (U < 0.01) U = 0.01;          //prevent div. by zero case
   mp_t_values[i] = (float)((I * L) / U);   

   //restrict values
   if (mp_t_values[i] > m_t_hi_limit)
    mp_t_values[i] = m_t_hi_limit;
   if (mp_t_values[i] < m_t_lo_limit)
    mp_t_values[i] = m_t_lo_limit;
  }
 }
 Super::OnOK();
}

void CDwellCalcDlg::OnCancel()
{
 Super::OnCancel();
}

BOOL CDwellCalcDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_r_spin.SetBuddy(&m_r_edit);
 m_r_edit.SetLimitText(4);
 m_r_edit.SetDecimalPlaces(2);
 m_r_spin.SetRangeAndDelta(0.1f, 5.0f, 0.01f);

 m_l_spin.SetBuddy(&m_l_edit);
 m_l_edit.SetLimitText(5);
 m_l_edit.SetDecimalPlaces(2);
 m_l_spin.SetRangeAndDelta(1.0f, 10.0f, 0.01f);

 m_i_spin.SetBuddy(&m_i_edit);
 m_i_edit.SetLimitText(5);
 m_i_edit.SetDecimalPlaces(2);
 m_i_spin.SetRangeAndDelta(1.0f, 20.0f, 0.01f);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_r_edit, MLL::GetString(IDS_DWELL_CALC_R_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_r_spin, MLL::GetString(IDS_DWELL_CALC_R_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_l_edit, MLL::GetString(IDS_DWELL_CALC_L_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_l_spin, MLL::GetString(IDS_DWELL_CALC_L_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_i_edit, MLL::GetString(IDS_DWELL_CALC_I_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_i_spin, MLL::GetString(IDS_DWELL_CALC_I_EDIT_TT)));
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CDwellCalcDlg::SetVoltageValues(const float* values, size_t size)
{
 delete mp_v_values;
 delete mp_t_values;
 mp_v_values = new float[size];
 mp_t_values = new float[size]; 
 m_size = size;
 std::copy(values, values + size, mp_v_values);
}

void CDwellCalcDlg::SetLimits(float t_lo_limit, float t_hi_limit)
{
 m_t_lo_limit = t_lo_limit; 
 m_t_hi_limit = t_hi_limit;
}

const float* CDwellCalcDlg::GetTimeValues(void)
{
 return mp_t_values;
}

INT_PTR CDwellCalcDlg::DoModal()
{
 INT_PTR result = Super::DoModal();
 CToolTipCtrlEx::ActivateAllTooltips(false, false); //update visibility status of tool tips (prevent bug)
 return result;
}
