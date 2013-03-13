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
#include <float.h>
#include <math.h>
#include "Resources/resource.h"
#include "MAPCalcDlg.h"

namespace {
void _FillCB(CComboBox& combo, const CMAPCalcDlg::UnitMap& i_units)
{
 combo.ResetContent();
 for(CMAPCalcDlg::UnitMap::const_iterator it = i_units.begin(); it != i_units.end(); ++it)
 {
  int index = combo.AddString(it->second.c_str());
  if (CB_ERR != index)
   combo.SetItemData(index, it->first);
 }
}

bool _SelCB(CComboBox& combo, CMAPCalcDlg::UnitId i_unit)
{
 int n = combo.GetCount();
 for(int i = 0; i < n; ++i)
 {
  DWORD_PTR value = combo.GetItemData(i);
  if (CB_ERR == value)
   return false;
  if (((CMAPCalcDlg::UnitId)value) != i_unit)
   continue;
  combo.SetCurSel(i);
  return true;
 }
 return false; //can't find specified item
}

CMAPCalcDlg::UnitId _GetSelCB(CComboBox& combo)
{
 int index = combo.GetCurSel();
 return combo.GetItemData(index);
}
}

const UINT CMAPCalcDlg::IDD = IDD_MAP_CALC;

/////////////////////////////////////////////////////////////////////////////
// CMAPCalcDlg dialog

BEGIN_MESSAGE_MAP(CMAPCalcDlg, Super)
 ON_BN_CLICKED(IDC_MAP_CALC_KNOW_GRAD_CHECK, OnKnownGradientCheck)
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_GRADIENT_EDIT, OnUpdateGradientCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_GRADIENT_SPIN, OnUpdateGradientCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_GRADIENT_N_COMBO, OnUpdateGradientCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_GRADIENT_D_COMBO, OnUpdateGradientCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_GRADIENT_SLASH, OnUpdateGradientCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_GRADIENT_CAPTION, OnUpdateGradientCtrls)

 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_2NDPT_PRESS_EDIT, OnUpdate2ndptCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_2NDPT_PRESS_SPIN, OnUpdate2ndptCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_2NDPT_PRESS_COMBO, OnUpdate2ndptCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_2NDPT_VOLT_EDIT, OnUpdate2ndptCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_2NDPT_VOLT_SPIN, OnUpdate2ndptCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_2NDPT_VOLT_COMBO, OnUpdate2ndptCtrls)
 ON_UPDATE_COMMAND_UI(IDC_MAP_CALC_2NDPT_CAPTION, OnUpdate2ndptCtrls)

 ON_CBN_SELCHANGE(IDC_MAP_CALC_1STPT_PRESS_COMBO, On1stptPressCombo)
 ON_CBN_SELCHANGE(IDC_MAP_CALC_1STPT_VOLT_COMBO, On1stptVoltCombo)
 ON_CBN_SELCHANGE(IDC_MAP_CALC_2NDPT_PRESS_COMBO, On2ndptPressCombo)
 ON_CBN_SELCHANGE(IDC_MAP_CALC_2NDPT_VOLT_COMBO, On2ndptVoltCombo)
 ON_CBN_SELCHANGE(IDC_MAP_CALC_GRADIENT_N_COMBO, OnGradientNCombo)
 ON_CBN_SELCHANGE(IDC_MAP_CALC_GRADIENT_D_COMBO, OnGradientDCombo)
END_MESSAGE_MAP()

CMAPCalcDlg::CMAPCalcDlg(CWnd* pParent /*=NULL*/)
: Super(CMAPCalcDlg::IDD, pParent)
, m_1stpt_press_edit(CEditEx::MODE_FLOAT)
, m_1stpt_volt_edit(CEditEx::MODE_FLOAT)
, m_2ndpt_press_edit(CEditEx::MODE_FLOAT)
, m_2ndpt_volt_edit(CEditEx::MODE_FLOAT)
, m_gradient_edit(CEditEx::MODE_FLOAT)
, m_1stpt_press(0.0f)
, m_1stpt_volt(0.0f)
, m_2ndpt_press(0.0f)
, m_2ndpt_volt(0.0f)
, m_gradient(0.0f)
, m_enable_gradient_ctrls(false)
, m_enable_2ndpt_ctrls(false)
{
 //empty
}

CMAPCalcDlg::~CMAPCalcDlg()
{
 //empty
}

void CMAPCalcDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_MAP_CALC_1STPT_PRESS_EDIT, m_1stpt_press_edit);
 DDX_Control(pDX, IDC_MAP_CALC_1STPT_PRESS_SPIN, m_1stpt_press_spin);
 DDX_Control(pDX, IDC_MAP_CALC_1STPT_PRESS_COMBO, m_1stpt_press_unit);

 DDX_Control(pDX, IDC_MAP_CALC_1STPT_VOLT_EDIT, m_1stpt_volt_edit);
 DDX_Control(pDX, IDC_MAP_CALC_1STPT_VOLT_SPIN, m_1stpt_volt_spin);
 DDX_Control(pDX, IDC_MAP_CALC_1STPT_VOLT_COMBO, m_1stpt_volt_unit);

 DDX_Control(pDX, IDC_MAP_CALC_2NDPT_PRESS_EDIT, m_2ndpt_press_edit);
 DDX_Control(pDX, IDC_MAP_CALC_2NDPT_PRESS_SPIN, m_2ndpt_press_spin);
 DDX_Control(pDX, IDC_MAP_CALC_2NDPT_PRESS_COMBO, m_2ndpt_press_unit);

 DDX_Control(pDX, IDC_MAP_CALC_2NDPT_VOLT_EDIT, m_2ndpt_volt_edit);
 DDX_Control(pDX, IDC_MAP_CALC_2NDPT_VOLT_SPIN, m_2ndpt_volt_spin);
 DDX_Control(pDX, IDC_MAP_CALC_2NDPT_VOLT_COMBO, m_2ndpt_volt_unit);

 DDX_Control(pDX, IDC_MAP_CALC_GRADIENT_EDIT, m_gradient_edit);
 DDX_Control(pDX, IDC_MAP_CALC_GRADIENT_SPIN, m_gradient_spin);
 DDX_Control(pDX, IDC_MAP_CALC_GRADIENT_N_COMBO, m_gradient_n_unit);
 DDX_Control(pDX, IDC_MAP_CALC_GRADIENT_D_COMBO, m_gradient_d_unit);
 DDX_Control(pDX, IDC_MAP_CALC_KNOW_GRAD_CHECK, m_known_grad_check);

 //Do data exchange and validation
 m_1stpt_press_edit.DDX_Value(pDX, IDC_MAP_CALC_1STPT_PRESS_EDIT, m_1stpt_press);
 DDV_MinMaxFloat(pDX, m_1stpt_press, m_pressLimits.first, m_pressLimits.second);
 m_1stpt_volt_edit.DDX_Value(pDX, IDC_MAP_CALC_1STPT_VOLT_EDIT, m_1stpt_volt);
 DDV_MinMaxFloat(pDX, m_1stpt_volt, m_voltLimits.first, m_voltLimits.second);
 m_2ndpt_press_edit.DDX_Value(pDX, IDC_MAP_CALC_2NDPT_PRESS_EDIT, m_2ndpt_press);
 if (m_enable_2ndpt_ctrls)
  DDV_MinMaxFloat(pDX, m_2ndpt_press, m_pressLimits.first, m_pressLimits.second);
 m_2ndpt_volt_edit.DDX_Value(pDX, IDC_MAP_CALC_2NDPT_VOLT_EDIT, m_2ndpt_volt);
 if (m_enable_2ndpt_ctrls)
  DDV_MinMaxFloat(pDX, m_2ndpt_volt, m_voltLimits.first, m_voltLimits.second);
 m_gradient_edit.DDX_Value(pDX, IDC_MAP_CALC_GRADIENT_EDIT, m_gradient);
 if (m_enable_gradient_ctrls)
  DDV_MinMaxFloat(pDX, m_gradient, m_gradLimits.first, m_gradLimits.second);
}

/////////////////////////////////////////////////////////////////////////////
// CMAPCalcDlg message handlers

void CMAPCalcDlg::OnOK()
{ 
 bool valid = UpdateData(true);
 if (!valid)
  return;

 if (m_OnOK && !m_OnOK())
  return; //controller didn't validate values

 Super::OnOK();
}

void CMAPCalcDlg::OnCancel()
{
 Super::OnCancel();
}

BOOL CMAPCalcDlg::OnInitDialog()
{
 Super::OnInitDialog();
 m_1stpt_press_spin.SetBuddy(&m_1stpt_press_edit);
 m_1stpt_press_edit.SetLimitText(6);

 m_1stpt_volt_spin.SetBuddy(&m_1stpt_volt_edit);
 m_1stpt_volt_edit.SetLimitText(6);

 m_2ndpt_press_spin.SetBuddy(&m_2ndpt_press_edit);
 m_2ndpt_press_edit.SetLimitText(6);

 m_2ndpt_volt_spin.SetBuddy(&m_2ndpt_volt_edit);
 m_2ndpt_volt_edit.SetLimitText(6);

 m_gradient_spin.SetBuddy(&m_gradient_edit);
 m_gradient_edit.SetLimitText(6);

 if (m_OnActivate)
  m_OnActivate();

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CMAPCalcDlg::OnKickIdle(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
 UpdateDialogControls(this, true);
 return FALSE;
}

void CMAPCalcDlg::OnUpdateGradientCtrls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable_gradient_ctrls);
}

void CMAPCalcDlg::OnUpdate2ndptCtrls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enable_2ndpt_ctrls);
}

void CMAPCalcDlg::OnKnownGradientCheck()
{
 if (m_OnKnowGradientCheck)
  m_OnKnowGradientCheck();
}

void CMAPCalcDlg::On1stptPressCombo()
{
 if (m_On1stptPressUnit)
 {
  UnitId id = _GetSelCB(m_1stpt_press_unit);
  m_On1stptPressUnit(id);
 }
}

void CMAPCalcDlg::On1stptVoltCombo()
{
 if (m_On1stptVoltUnit)
 {
  UnitId id = _GetSelCB(m_1stpt_volt_unit);
  m_On1stptVoltUnit(id);
 }
}

void CMAPCalcDlg::On2ndptPressCombo()
{
 if (m_On2ndptPressUnit)
 {
  UnitId id = _GetSelCB(m_2ndpt_press_unit);
  m_On2ndptPressUnit(id);
 }
}

void CMAPCalcDlg::On2ndptVoltCombo()
{
 if (m_On2ndptVoltUnit)
 {
  UnitId id = _GetSelCB(m_2ndpt_volt_unit);
  m_On2ndptVoltUnit(id);
 }
}

void CMAPCalcDlg::OnGradientNCombo()
{
 if (m_OnGradientUnitN)
 {
  UnitId n_id = _GetSelCB(m_gradient_n_unit);
  UnitId d_id = _GetSelCB(m_gradient_d_unit);
  m_OnGradientUnitN(n_id, d_id);
 }
}

void CMAPCalcDlg::OnGradientDCombo()
{
 if (m_OnGradientUnitD)
 {
  UnitId n_id = _GetSelCB(m_gradient_n_unit);
  UnitId d_id = _GetSelCB(m_gradient_d_unit);
  m_OnGradientUnitD(n_id, d_id);
 }
}

void CMAPCalcDlg::Fill1stptPressUnit(const UnitMap& i_units)
{
 _FillCB(m_1stpt_press_unit, i_units);
}

void CMAPCalcDlg::Fill1stptVoltUnit(const UnitMap& i_units)
{
 _FillCB(m_1stpt_volt_unit, i_units);
}

void CMAPCalcDlg::Fill2ndptPressUnit(const UnitMap& i_units)
{
 _FillCB(m_2ndpt_press_unit, i_units);
}

void CMAPCalcDlg::Fill2ndptVoltUnit(const UnitMap& i_units)
{
 _FillCB(m_2ndpt_volt_unit, i_units);
}

void CMAPCalcDlg::FillGradientUnit(const UnitMap& i_n_units, const UnitMap& i_d_units)
{
 _FillCB(m_gradient_n_unit, i_n_units);
 _FillCB(m_gradient_d_unit, i_d_units);
}

void CMAPCalcDlg::SetPressLimits(float i_min, float i_max, int i_dp)
{
 m_pressLimits.first = i_min;
 m_pressLimits.second = i_max;
 float inc = 1.0f / pow(10.0f, i_dp);
 m_1stpt_press_edit.SetDecimalPlaces(i_dp);
 m_1stpt_press_spin.SetRangeAndDelta(i_min, i_max, inc);
 m_2ndpt_press_edit.SetDecimalPlaces(i_dp);
 m_2ndpt_press_spin.SetRangeAndDelta(i_min, i_max, inc);
}

void CMAPCalcDlg::SetVoltLimits(float i_min, float i_max, int i_dp)
{
 m_voltLimits.first = i_min;
 m_voltLimits.second = i_max;
 float inc = 1.0f / pow(10.0f, i_dp);
 m_1stpt_volt_edit.SetDecimalPlaces(i_dp);
 m_1stpt_volt_spin.SetRangeAndDelta(i_min, i_max, inc);
 m_2ndpt_volt_edit.SetDecimalPlaces(i_dp);
 m_2ndpt_volt_spin.SetRangeAndDelta(i_min, i_max, inc);
}

void CMAPCalcDlg::SetGradLimits(float i_min, float i_max, int i_dp)
{
 m_gradLimits.first = i_min;
 m_gradLimits.second = i_max;
 float inc = 1.0f / pow(10.0f, i_dp);
 m_gradient_edit.SetDecimalPlaces(i_dp);
 m_gradient_spin.SetRangeAndDelta(i_min, i_max, inc);
}

void CMAPCalcDlg::Set1stptPress(float i_value, UnitId i_unit)
{
 m_1stpt_press = i_value;
 if (::IsWindow(m_hWnd))
 {
  UpdateData(false);
 _SelCB(m_1stpt_press_unit, i_unit);
 }
}

void CMAPCalcDlg::Set1stptVolt(float i_value, UnitId i_unit)
{
 m_1stpt_volt = i_value;
 if (::IsWindow(m_hWnd))
 {
  UpdateData(false);
  _SelCB(m_1stpt_volt_unit, i_unit);
 }
}

void CMAPCalcDlg::Set2ndptPress(float i_value, UnitId i_unit)
{
 m_2ndpt_press = i_value;
 if (::IsWindow(m_hWnd))
 {
  UpdateData(false);
  _SelCB(m_2ndpt_press_unit, i_unit);
 }
}

void CMAPCalcDlg::Set2ndptVolt(float i_value, UnitId i_unit)
{
 m_2ndpt_volt = i_value;
 if (::IsWindow(m_hWnd))
 {
  UpdateData(false);
  _SelCB(m_2ndpt_volt_unit, i_unit);
 }
}

void CMAPCalcDlg::SetGradient(float i_value, UnitId i_n_unit, UnitId i_d_unit)
{
 m_gradient = i_value;
 if (::IsWindow(m_hWnd))
 {
  UpdateData(false);
  _SelCB(m_gradient_n_unit, i_n_unit);
  _SelCB(m_gradient_d_unit, i_d_unit);
 }
}

void CMAPCalcDlg::SetGradientUnit(UnitId i_n_unit, UnitId i_d_unit)
{
 if (!::IsWindow(m_hWnd))
  return;
 _SelCB(m_gradient_n_unit, i_n_unit);
 _SelCB(m_gradient_d_unit, i_d_unit);
}

float CMAPCalcDlg::Get1stptPress(void) const
{
 return m_1stpt_press;
}

float CMAPCalcDlg::Get1stptVolt(void) const
{
 return m_1stpt_volt;
}

float CMAPCalcDlg::Get2ndptPress(void) const
{
 return m_2ndpt_press;
}

float CMAPCalcDlg::Get2ndptVolt(void) const
{
 return m_2ndpt_volt;
}

float CMAPCalcDlg::GetGradient(void) const
{
 return m_gradient;
}

void CMAPCalcDlg::SetKnownGradientCheck(bool i_value)
{
 m_known_grad_check.SetCheck(i_value ? BST_CHECKED : BST_UNCHECKED);
}

bool CMAPCalcDlg::GetKnownGradientCheck(void) const
{
 return m_known_grad_check.GetCheck() == BST_CHECKED; 
}

void CMAPCalcDlg::EnableGradientCtrls(bool i_enable)
{
 m_enable_gradient_ctrls = i_enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, true);
}

void CMAPCalcDlg::Enable2ndptCtrls(bool i_enable)
{
 m_enable_2ndpt_ctrls = i_enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, true);
}

void CMAPCalcDlg::setOnKnowGradientCheck(const EventHandler& OnFunction)
{ m_OnKnowGradientCheck = OnFunction; }
  
void CMAPCalcDlg::setOnActivate(const EventHandler& OnFunction)
{ m_OnActivate = OnFunction; }

void CMAPCalcDlg::setOn1stptPressUnit(const EventHandlerId& OnFunction)
{ m_On1stptPressUnit = OnFunction; }

void CMAPCalcDlg::setOn1stptVoltUnit(const EventHandlerId& OnFunction)
{ m_On1stptVoltUnit = OnFunction; }

void CMAPCalcDlg::setOn2ndptPressUnit(const EventHandlerId& OnFunction)
{ m_On2ndptPressUnit = OnFunction; }

void CMAPCalcDlg::setOn2ndptVoltUnit(const EventHandlerId& OnFunction)
{ m_On2ndptVoltUnit = OnFunction; }

void CMAPCalcDlg::setOnGradientUnit(const EventHandler2Id& OnFunctionN, const EventHandler2Id& OnFunctionD)
{ m_OnGradientUnitN = OnFunctionN, m_OnGradientUnitD = OnFunctionD; }

void CMAPCalcDlg::setOnOK(const EventResult& OnFunction)
{ m_OnOK = OnFunction;}
