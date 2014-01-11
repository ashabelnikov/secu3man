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
#include "AnglesPageDlg.h"
#include "ui-core/DDX_helpers.h"
#include "ui-core/ToolTipCtrlEx.h"

const UINT CAnglesPageDlg::IDD = IDD_PD_ANGLES_PAGE;

BEGIN_MESSAGE_MAP(CAnglesPageDlg, Super)
 ON_EN_CHANGE(IDC_PD_ANGLES_MIN_ANGLE_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_MAX_ANGLE_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_CORRECTION_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_DECREASE_SPEAD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_ANGLES_INCREASE_SPEAD_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_ANGLES_ZEROAA_CHECK, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MIN_ANGLE_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_MAX_ANGLE_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_CORRECTION_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_DECREASE_SPEAD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_DECREASE_SPEAD_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_DECREASE_SPEAD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_DECREASE_SPEAD_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_INCREASE_SPEAD_EDIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_INCREASE_SPEAD_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_INCREASE_SPEAD_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_INCREASE_SPEAD_UNIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_ZEROAA_CHECK, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_ANGLES_ANGLE_SPEAD_GROUP, OnUpdateControls)
END_MESSAGE_MAP()

CAnglesPageDlg::CAnglesPageDlg(CWnd* pParent /*=NULL*/)
: Super(CAnglesPageDlg::IDD, pParent)
, m_enabled(false)
, m_max_angle_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_min_angle_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_correction_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED)
, m_decrease_spead_edit(CEditEx::MODE_FLOAT)
, m_increase_spead_edit(CEditEx::MODE_FLOAT)
{
 m_params.max_angle = 60.0f;
 m_params.min_angle = -15.0f;
 m_params.angle_corr = 0.0f;
 m_params.dec_spead = 3.0f;
 m_params.inc_spead = 3.0f;
 m_params.zero_adv_ang = 0;
}

LPCTSTR CAnglesPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD;
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
 DDX_Control(pDX, IDC_PD_ANGLES_DECREASE_SPEAD_EDIT, m_decrease_spead_edit);
 DDX_Control(pDX, IDC_PD_ANGLES_INCREASE_SPEAD_EDIT, m_increase_spead_edit);
 DDX_Control(pDX, IDC_PD_ANGLES_DECREASE_SPEAD_SPIN, m_decrease_spead_spin);
 DDX_Control(pDX, IDC_PD_ANGLES_INCREASE_SPEAD_SPIN, m_increase_spead_spin);
 DDX_Control(pDX, IDC_PD_ANGLES_ZEROAA_CHECK, m_zeroaa_check);

 m_max_angle_edit.DDX_Value(pDX, IDC_PD_ANGLES_MAX_ANGLE_EDIT, m_params.max_angle);
 m_min_angle_edit.DDX_Value(pDX, IDC_PD_ANGLES_MIN_ANGLE_EDIT, m_params.min_angle);
 m_correction_edit.DDX_Value(pDX, IDC_PD_ANGLES_CORRECTION_EDIT, m_params.angle_corr);
 m_decrease_spead_edit.DDX_Value(pDX, IDC_PD_ANGLES_DECREASE_SPEAD_EDIT, m_params.dec_spead);
 m_increase_spead_edit.DDX_Value(pDX, IDC_PD_ANGLES_INCREASE_SPEAD_EDIT, m_params.inc_spead);
 DDX_Check_UCHAR(pDX, IDC_PD_ANGLES_ZEROAA_CHECK, m_params.zero_adv_ang);
}

/////////////////////////////////////////////////////////////////////////////
// CAnglesPageDlg message handlers

//если надо апдейтить отдельные контроллы, то надо будет плодить функции
void CAnglesPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

BOOL CAnglesPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_min_angle_spin.SetBuddy(&m_min_angle_edit);
 m_min_angle_edit.SetLimitText(4);
 m_min_angle_edit.SetDecimalPlaces(2);
 m_min_angle_spin.SetRangeAndDelta(-15.0f,60.0f,0.25f);

 m_max_angle_spin.SetBuddy(&m_max_angle_edit);
 m_max_angle_edit.SetLimitText(4);
 m_max_angle_edit.SetDecimalPlaces(2);
 m_max_angle_spin.SetRangeAndDelta(-15.0f,60.0f,0.25f);

 m_correction_spin.SetBuddy(&m_correction_edit);
 m_correction_edit.SetLimitText(4);
 m_correction_edit.SetDecimalPlaces(2);
 m_correction_spin.SetRangeAndDelta(-30.0f,30.0f,0.25f);

 m_decrease_spead_spin.SetBuddy(&m_decrease_spead_edit);
 m_decrease_spead_edit.SetLimitText(4);
 m_decrease_spead_edit.SetDecimalPlaces(2);
 m_decrease_spead_spin.SetRangeAndDelta(0.0f,10.0f,0.025f);

 m_increase_spead_spin.SetBuddy(&m_increase_spead_edit);
 m_increase_spead_edit.SetLimitText(4);
 m_increase_spead_edit.SetDecimalPlaces(2);
 m_increase_spead_spin.SetRangeAndDelta(0.0f,10.0f,0.025f);
 
 UpdateData(FALSE);
 
  //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_min_angle_spin, MLL::GetString(IDS_PD_ANGLES_MIN_ANGLE_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_min_angle_edit, MLL::GetString(IDS_PD_ANGLES_MIN_ANGLE_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_max_angle_spin, MLL::GetString(IDS_PD_ANGLES_MAX_ANGLE_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_max_angle_edit, MLL::GetString(IDS_PD_ANGLES_MAX_ANGLE_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_zeroaa_check, MLL::GetString(IDS_PD_ANGLES_ZEROAA_CHECK_TT)));
 
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnglesPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//разрешение/запрещение контроллов (всех поголовно)
void CAnglesPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this, TRUE);
}

//что с контроллами?
bool CAnglesPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//эту функцию необходимо использовать когда надо получить данные из диалога
void CAnglesPageDlg::GetValues(SECU3IO::AnglesPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //копируем данные из диалога в переменные
 memcpy(o_values, &m_params, sizeof(SECU3IO::AnglesPar));
}

//эту функцию необходимо использовать когда надо занести данные в диалог
void CAnglesPageDlg::SetValues(const SECU3IO::AnglesPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params, i_values, sizeof(SECU3IO::AnglesPar));
 UpdateData(FALSE); //копируем данные из переменных в диалог
}
