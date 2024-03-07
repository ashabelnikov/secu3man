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

/** \file AnglesPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "AnglesPageDlg.h"
#include "common/dpiaware.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/DDX_helpers.h"
#include "ui-core/WndScroller.h"

BEGIN_MESSAGE_MAP(CAnglesPageDlg, Super)
 ON_WM_SIZE()
 ON_WM_DESTROY()
 ON_EN_CHANGE(IDC_PD_ANGLES_MIN_ANGLE_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_MAX_ANGLE_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_CORRECTION_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_DECREASE_SPEED_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_INCREASE_SPEED_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_SHIFT_IGNTIM_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_ANGLES_ZEROAA_CHECK, OnChangeDataZeroAA)
 ON_BN_CLICKED(IDC_PD_ANGLES_ZEROAAOCT_CHECK, OnChangeDataZeroAAOct)
 ON_BN_CLICKED(IDC_PD_ANGLES_ALWAYS_WRKMAP_CHECK, OnChangeData)
 ON_BN_CLICKED(IDC_PD_ANGLES_MANIGNTIM_IDL_CHECK, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_EDIT, OnUpdateControlsCorr)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_SPIN, OnUpdateControlsCorr)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_CAPTION, OnUpdateControlsCorr)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_UNIT, OnUpdateControlsCorr)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_DECREASE_SPEED_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_DECREASE_SPEED_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_DECREASE_SPEED_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_DECREASE_SPEED_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_INCREASE_SPEED_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_INCREASE_SPEED_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_INCREASE_SPEED_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_INCREASE_SPEED_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_ZEROAA_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_ZEROAAOCT_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_ANGLE_SPEED_GROUP, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_SHIFT_IGNTIM_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_SHIFT_IGNTIM_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_SHIFT_IGNTIM_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_SHIFT_IGNTIM_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_ALWAYS_WRKMAP_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MANIGNTIM_IDL_CHECK, OnUpdateControls)
END_MESSAGE_MAP()

CAnglesPageDlg::CAnglesPageDlg()
: m_enabled(false)
, m_max_angle_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_min_angle_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_decrease_speed_edit(CEditEx::MODE_FLOAT, true)
, m_increase_speed_edit(CEditEx::MODE_FLOAT, true)
, m_shift_igntim_edit(CEditEx::MODE_FLOAT, true)
, mp_scr(new CWndScroller)
{
 m_params.max_angle = 60.0f;
 m_params.min_angle = -15.0f;
 m_params.angle_corr = 0.0f;
 m_params.dec_speed = 3.0f;
 m_params.inc_speed = 3.0f;
 m_params.zero_adv_ang = 0;
 m_params.zero_adv_ang_oct = false;
 m_params.shift_igntim = 0.0f;
 m_params.igntim_wrkmap = false;
 m_params.manigntim_idl = true;
}

LPCTSTR CAnglesPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PD_ANGLES_PAGE;
}

void CAnglesPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_ANGLES_MIN_ANGLE_SPIN, m_min_angle_spin);
 DDX_Control(pDX, IDC_PD_ANGLES_MAX_ANGLE_SPIN, m_max_angle_spin);
 DDX_Control(pDX, IDC_PD_ANGLES_CORRECTION_SPIN, m_correction_spin);
 DDX_Control(pDX, IDC_PD_ANGLES_MIN_ANGLE_EDIT, m_min_angle_edit);
 DDX_Control(pDX, IDC_PD_ANGLES_MAX_ANGLE_EDIT, m_max_angle_edit);
 DDX_Control(pDX, IDC_PD_ANGLES_CORRECTION_EDIT, m_correction_edit);
 DDX_Control(pDX, IDC_PD_ANGLES_DECREASE_SPEED_EDIT, m_decrease_speed_edit);
 DDX_Control(pDX, IDC_PD_ANGLES_INCREASE_SPEED_EDIT, m_increase_speed_edit);
 DDX_Control(pDX, IDC_PD_ANGLES_DECREASE_SPEED_SPIN, m_decrease_speed_spin);
 DDX_Control(pDX, IDC_PD_ANGLES_INCREASE_SPEED_SPIN, m_increase_speed_spin);
 DDX_Control(pDX, IDC_PD_ANGLES_ZEROAA_CHECK, m_zeroaa_check);
 DDX_Control(pDX, IDC_PD_ANGLES_SHIFT_IGNTIM_EDIT, m_shift_igntim_edit);
 DDX_Control(pDX, IDC_PD_ANGLES_SHIFT_IGNTIM_SPIN, m_shift_igntim_spin);
 DDX_Control(pDX, IDC_PD_ANGLES_ALWAYS_WRKMAP_CHECK, m_always_wrkmap_check);
 DDX_Control(pDX, IDC_PD_ANGLES_MANIGNTIM_IDL_CHECK, m_manigntim_idl_check);
 DDX_Control(pDX, IDC_PD_ANGLES_ZEROAAOCT_CHECK, m_zeroaaoct_check);

 m_max_angle_edit.DDX_Value(pDX, IDC_PD_ANGLES_MAX_ANGLE_EDIT, m_params.max_angle);
 m_min_angle_edit.DDX_Value(pDX, IDC_PD_ANGLES_MIN_ANGLE_EDIT, m_params.min_angle);
 m_correction_edit.DDX_Value(pDX, IDC_PD_ANGLES_CORRECTION_EDIT, m_params.angle_corr);
 m_decrease_speed_edit.DDX_Value(pDX, IDC_PD_ANGLES_DECREASE_SPEED_EDIT, m_params.dec_speed);
 m_increase_speed_edit.DDX_Value(pDX, IDC_PD_ANGLES_INCREASE_SPEED_EDIT, m_params.inc_speed);
 m_shift_igntim_edit.DDX_Value(pDX, IDC_PD_ANGLES_SHIFT_IGNTIM_EDIT, m_params.shift_igntim);
 DDX_Check_UCHAR(pDX, IDC_PD_ANGLES_ZEROAA_CHECK, m_params.zero_adv_ang);
 DDX_Check_bool(pDX, IDC_PD_ANGLES_ALWAYS_WRKMAP_CHECK, m_params.igntim_wrkmap);
 DDX_Check_bool(pDX, IDC_PD_ANGLES_MANIGNTIM_IDL_CHECK, m_params.manigntim_idl);
 DDX_Check_bool(pDX, IDC_PD_ANGLES_ZEROAAOCT_CHECK, m_params.zero_adv_ang_oct);
}

/////////////////////////////////////////////////////////////////////////////
// CAnglesPageDlg message handlers

void CAnglesPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CAnglesPageDlg::OnUpdateControlsCorr(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && !m_params.zero_adv_ang);
}

BOOL CAnglesPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 //initialize window scroller
 mp_scr->Init(this);

 m_min_angle_spin.SetBuddy(&m_min_angle_edit);
 m_min_angle_edit.SetLimitText(6);
 m_min_angle_edit.SetDecimalPlaces(2);
 m_min_angle_spin.SetRangeAndDelta(-15.0f,60.0f,0.25f);
 m_min_angle_edit.SetRange(-15.0f,60.0f);

 m_max_angle_spin.SetBuddy(&m_max_angle_edit);
 m_max_angle_edit.SetLimitText(5);
 m_max_angle_edit.SetDecimalPlaces(2);
 m_max_angle_spin.SetRangeAndDelta(-15.0f,60.0f,0.25f);
 m_max_angle_edit.SetRange(-15.0f,60.0f);

 m_correction_spin.SetBuddy(&m_correction_edit);
 m_correction_edit.SetLimitText(5);
 m_correction_edit.SetDecimalPlaces(2);
 m_correction_spin.SetRangeAndDelta(-30.0f,30.0f,0.25f);
 m_correction_edit.SetRange(-30.0f,30.0f);

 m_decrease_speed_spin.SetBuddy(&m_decrease_speed_edit);
 m_decrease_speed_edit.SetLimitText(4);
 m_decrease_speed_edit.SetDecimalPlaces(2);
 m_decrease_speed_spin.SetRangeAndDelta(0.0f,10.0f,0.025f);
 m_decrease_speed_edit.SetRange(0.0f,10.0f);

 m_increase_speed_spin.SetBuddy(&m_increase_speed_edit);
 m_increase_speed_edit.SetLimitText(4);
 m_increase_speed_edit.SetDecimalPlaces(2);
 m_increase_speed_spin.SetRangeAndDelta(0.0f,10.0f,0.025f);
 m_increase_speed_edit.SetRange(0.0f,10.0f);

 m_shift_igntim_spin.SetBuddy(&m_shift_igntim_edit);
 m_shift_igntim_edit.SetLimitText(5);
 m_shift_igntim_edit.SetDecimalPlaces(1);
 m_shift_igntim_spin.SetRangeAndDelta(-15.0f,55.0f,0.1f);
 m_shift_igntim_edit.SetRange(-15.0f,55.0f);

 UpdateData(FALSE);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 //Zero ignition timing check
 VERIFY(mp_ttc->AddWindow(&m_zeroaa_check,MLL::GetString(IDS_PD_ANGLES_ZEROAA_CHECK_TT)));
 //minimum advance angle edit and spin
 VERIFY(mp_ttc->AddWindow(&m_min_angle_edit,MLL::GetString(IDS_PD_ANGLES_MIN_ANGLE_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_min_angle_spin,MLL::GetString(IDS_PD_ANGLES_MIN_ANGLE_EDIT_TT)));
 //maximum advance angle edit and spin
 VERIFY(mp_ttc->AddWindow(&m_max_angle_edit,MLL::GetString(IDS_PD_ANGLES_MAX_ANGLE_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_max_angle_spin,MLL::GetString(IDS_PD_ANGLES_MAX_ANGLE_EDIT_TT)));
 //octane correction
 VERIFY(mp_ttc->AddWindow(&m_correction_edit,MLL::GetString(IDS_PD_ANGLES_CORRECTION_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_correction_spin,MLL::GetString(IDS_PD_ANGLES_CORRECTION_EDIT_TT)));
 //decreasing speed
 VERIFY(mp_ttc->AddWindow(&m_decrease_speed_edit,MLL::GetString(IDS_PD_ANGLES_DECREASE_SPEED_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_decrease_speed_spin,MLL::GetString(IDS_PD_ANGLES_DECREASE_SPEED_EDIT_TT)));
 //increasing speed
 VERIFY(mp_ttc->AddWindow(&m_increase_speed_edit,MLL::GetString(IDS_PD_ANGLES_INCREASE_SPEED_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_increase_speed_spin,MLL::GetString(IDS_PD_ANGLES_INCREASE_SPEED_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_shift_igntim_edit,MLL::GetString(IDS_PD_ANGLES_SHIFT_IGNTIM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_shift_igntim_spin,MLL::GetString(IDS_PD_ANGLES_SHIFT_IGNTIM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_always_wrkmap_check,MLL::GetString(IDS_PD_ANGLES_ALWAYS_WRKMAP_CHECK_TT)));
 VERIFY(mp_ttc->AddWindow(&m_manigntim_idl_check,MLL::GetString(IDS_PD_ANGLES_MANIGNTIM_IDL_CHECK_TT)));

 VERIFY(mp_ttc->AddWindow(&m_zeroaaoct_check,MLL::GetString(IDS_PD_ANGLES_ZEROAAOCT_CHECK_TT)));

 mp_ttc->SetMaxTipWidth(250); //enable text wrapping by setting width
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CAnglesPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CAnglesPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

void CAnglesPageDlg::OnChangeDataZeroAA()
{
 if (m_zeroaa_check.GetCheck()==BST_CHECKED)
  m_zeroaaoct_check.SetCheck(BST_UNCHECKED);

 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
 UpdateDialogControls(this, TRUE);
}

void CAnglesPageDlg::OnChangeDataZeroAAOct()
{
 if (m_zeroaaoct_check.GetCheck()==BST_CHECKED)
  m_zeroaa_check.SetCheck(BST_UNCHECKED);

 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
 UpdateDialogControls(this, TRUE);
}

//Enable/disable all controls
void CAnglesPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//get state of controls
bool CAnglesPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CAnglesPageDlg::GetValues(SECU3IO::AnglesPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 memcpy(o_values, &m_params, sizeof(SECU3IO::AnglesPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CAnglesPageDlg::SetValues(const SECU3IO::AnglesPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params, i_values, sizeof(SECU3IO::AnglesPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}

void CAnglesPageDlg::OnSize(UINT nType, int cx, int cy)
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(395));
}
