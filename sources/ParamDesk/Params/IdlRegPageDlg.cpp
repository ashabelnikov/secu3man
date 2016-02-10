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

/** \file IdlRegPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "IdlRegPageDlg.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"

const UINT CIdlRegPageDlg::IDD = IDD_PD_IDLREG_PAGE;

BEGIN_MESSAGE_MAP(CIdlRegPageDlg, Super)
 ON_WM_DESTROY()
 ON_EN_CHANGE(IDC_PD_IDLREG_DEAD_BAND_RPM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_FACTOR_NEG_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_FACTOR_POS_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_GOAL_RPM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_RESTRICTION_MIN_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_RESTRICTION_MAX_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_IDLREG_TURN_ON_TEMP_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_IDLREG_USE_REGULATOR, OnChangeData)
 ON_BN_CLICKED(IDC_PD_IDLREG_USE_ONGAS, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTORS_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_POS_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_POS_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_POS_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_NEG_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_NEG_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_FACTOR_NEG_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_GOAL_RPM_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_GOAL_RPM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_GOAL_RPM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_GOAL_RPM_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DEAD_BAND_RPM_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DEAD_BAND_RPM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DEAD_BAND_RPM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_DEAD_BAND_RPM_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_USE_REGULATOR,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_USE_ONGAS,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MIN_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MIN_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MIN_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MAX_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MAX_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTION_MAX_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_RESTRICTIONS_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_TURN_ON_TEMP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_TURN_ON_TEMP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_IDLREG_TURN_ON_TEMP_UNIT,OnUpdateControls)

END_MESSAGE_MAP()

CIdlRegPageDlg::CIdlRegPageDlg(CWnd* pParent /*=NULL*/)
: Super(CIdlRegPageDlg::IDD, pParent)
, m_enabled(false)
, m_factor_pos_edit(CEditEx::MODE_FLOAT, true)
, m_factor_neg_edit(CEditEx::MODE_FLOAT, true)
, m_restriction_min_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_restriction_max_edit(CEditEx::MODE_FLOAT, true)
, m_goal_rpm_edit(CEditEx::MODE_INT, true)
, m_dead_band_rpm_edit(CEditEx::MODE_INT, true)
, m_turn_on_temp_edit(CEditEx::MODE_FLOAT, true)
, mp_scr(new CWndScroller)
{
 m_params.ifac1 = 1.0f;
 m_params.ifac2 = 1.0f;
 m_params.MINEFR = 10;
 m_params.idling_rpm = 850;
 m_params.idl_regul = false;
 m_params.use_regongas = true;
 m_params.min_angle = -15.0f;
 m_params.max_angle = 30.0f;
 m_params.turn_on_temp = 50.0f;
}

LPCTSTR CIdlRegPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
}

void CIdlRegPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_IDLREG_USE_REGULATOR, m_use_regulator);
 DDX_Control(pDX, IDC_PD_IDLREG_GOAL_RPM_SPIN, m_goal_rpm_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_GOAL_RPM_EDIT, m_goal_rpm_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_FACTOR_POS_SPIN, m_factor_pos_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_FACTOR_POS_EDIT, m_factor_pos_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_FACTOR_NEG_SPIN, m_factor_neg_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_FACTOR_NEG_EDIT, m_factor_neg_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_DEAD_BAND_RPM_SPIN, m_dead_band_rpm_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_DEAD_BAND_RPM_EDIT, m_dead_band_rpm_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_RESTRICTION_MIN_EDIT, m_restriction_min_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_RESTRICTION_MIN_SPIN, m_restriction_min_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_RESTRICTION_MAX_EDIT, m_restriction_max_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_RESTRICTION_MAX_SPIN, m_restriction_max_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_TURN_ON_TEMP_EDIT, m_turn_on_temp_edit);
 DDX_Control(pDX, IDC_PD_IDLREG_TURN_ON_TEMP_SPIN, m_turn_on_temp_spin);
 DDX_Control(pDX, IDC_PD_IDLREG_USE_ONGAS, m_use_regongas);

 m_factor_pos_edit.DDX_Value(pDX, IDC_PD_IDLREG_FACTOR_POS_EDIT, m_params.ifac1);
 m_factor_neg_edit.DDX_Value(pDX, IDC_PD_IDLREG_FACTOR_NEG_EDIT, m_params.ifac2);
 m_restriction_min_edit.DDX_Value(pDX, IDC_PD_IDLREG_RESTRICTION_MIN_EDIT, m_params.min_angle);
 m_restriction_max_edit.DDX_Value(pDX, IDC_PD_IDLREG_RESTRICTION_MAX_EDIT, m_params.max_angle);
 m_goal_rpm_edit.DDX_Value(pDX, IDC_PD_IDLREG_GOAL_RPM_EDIT, m_params.idling_rpm);
 m_dead_band_rpm_edit.DDX_Value(pDX, IDC_PD_IDLREG_DEAD_BAND_RPM_EDIT, m_params.MINEFR);
 m_turn_on_temp_edit.DDX_Value(pDX, IDC_PD_IDLREG_TURN_ON_TEMP_EDIT, m_params.turn_on_temp);
 DDX_Check_bool(pDX, IDC_PD_IDLREG_USE_REGULATOR, m_params.idl_regul);
 DDX_Check_bool(pDX, IDC_PD_IDLREG_USE_ONGAS, m_params.use_regongas);
}

/////////////////////////////////////////////////////////////////////////////
// CIdlRegPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CIdlRegPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CIdlRegPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_factor_pos_spin.SetBuddy(&m_factor_pos_edit);
 m_factor_pos_edit.SetLimitText(5);
 m_factor_pos_edit.SetDecimalPlaces(2);
 m_factor_pos_spin.SetRangeAndDelta(0.0f,10.0f,0.01f);
 m_factor_pos_edit.SetRange(0.0f,10.0f);

 m_factor_neg_spin.SetBuddy(&m_factor_neg_edit);
 m_factor_neg_edit.SetLimitText(5);
 m_factor_neg_edit.SetDecimalPlaces(2);
 m_factor_neg_spin.SetRangeAndDelta(0.0f,10.0f,0.01f);
 m_factor_neg_edit.SetRange(0.0f,10.0f);

 m_dead_band_rpm_edit.SetLimitText(3);
 m_dead_band_rpm_edit.SetDecimalPlaces(3);
 m_dead_band_rpm_spin.SetBuddy(&m_dead_band_rpm_edit);
 m_dead_band_rpm_spin.SetRangeAndDelta(0,500,1);
 m_dead_band_rpm_edit.SetRange(0,500);

 m_goal_rpm_edit.SetLimitText(4);
 m_goal_rpm_spin.SetBuddy(&m_goal_rpm_edit);
 m_goal_rpm_spin.SetRangeAndDelta(250,1800,5);
 m_goal_rpm_edit.SetRange(250,1800);

 m_restriction_min_spin.SetBuddy(&m_restriction_min_edit);
 m_restriction_min_edit.SetLimitText(5);
 m_restriction_min_edit.SetDecimalPlaces(2);
 m_restriction_min_spin.SetRangeAndDelta(-15.0f,30.0f,0.025f);
 m_restriction_min_edit.SetRange(-15.0f,30.0f);

 m_restriction_max_spin.SetBuddy(&m_restriction_max_edit);
 m_restriction_max_edit.SetLimitText(5);
 m_restriction_max_edit.SetDecimalPlaces(2);
 m_restriction_max_spin.SetRangeAndDelta(-15.0f,30.0f,0.025f);
 m_restriction_max_edit.SetRange(-15.0f,30.0f);

 m_turn_on_temp_spin.SetBuddy(&m_turn_on_temp_edit);
 m_turn_on_temp_edit.SetLimitText(5);
 m_turn_on_temp_edit.SetDecimalPlaces(2);
 m_turn_on_temp_spin.SetRangeAndDelta(0.0f,100.0f,0.2f);
 m_turn_on_temp_edit.SetRange(0.0f,100.0f);

 UpdateData(FALSE);

 //initialize window scroller
 mp_scr->Init(this);
 CRect wndRect; GetWindowRect(&wndRect);
 mp_scr->SetViewSize(0, int(wndRect.Height() * 1.15f));
 
 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_use_regulator, MLL::GetString(IDS_PD_IDLREG_USE_REGULATOR_TT)));
 VERIFY(mp_ttc->AddWindow(&m_goal_rpm_spin, MLL::GetString(IDS_PD_IDLREG_GOAL_RPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_goal_rpm_edit, MLL::GetString(IDS_PD_IDLREG_GOAL_RPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_factor_pos_spin, MLL::GetString(IDS_PD_IDLREG_FACTOR_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_factor_pos_edit, MLL::GetString(IDS_PD_IDLREG_FACTOR_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_factor_neg_spin, MLL::GetString(IDS_PD_IDLREG_FACTOR_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_factor_neg_edit, MLL::GetString(IDS_PD_IDLREG_FACTOR_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_dead_band_rpm_spin, MLL::GetString(IDS_PD_IDLREG_DEAD_BAND_RPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_dead_band_rpm_edit, MLL::GetString(IDS_PD_IDLREG_DEAD_BAND_RPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_turn_on_temp_edit, MLL::GetString(IDS_PD_IDLREG_TURN_ON_TEMP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_turn_on_temp_spin, MLL::GetString(IDS_PD_IDLREG_TURN_ON_TEMP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_restriction_min_edit, MLL::GetString(IDS_PD_IDLREG_RESTRICTION_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_restriction_min_spin, MLL::GetString(IDS_PD_IDLREG_RESTRICTION_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_restriction_max_edit, MLL::GetString(IDS_PD_IDLREG_RESTRICTION_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_restriction_max_spin, MLL::GetString(IDS_PD_IDLREG_RESTRICTION_EDIT_TT)));

 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);
  
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CIdlRegPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CIdlRegPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//Enable/disable all controls
void CIdlRegPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//get state of all controls (enabled or disabled)
bool CIdlRegPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CIdlRegPageDlg::GetValues(SECU3IO::IdlRegPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values,&m_params, sizeof(SECU3IO::IdlRegPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CIdlRegPageDlg::SetValues(const SECU3IO::IdlRegPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::IdlRegPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
