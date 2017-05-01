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

/** \file CESettingsDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include <afxpriv.h>
#include "resource.h"
#include "CESettingsDlg.h"
#include "ui-core/EditEx.h"
#include "io-core/SECU3IO.h"

const UINT CCESettingsDlg::IDD = IDD_CESETTINGS_EDITOR;

/////////////////////////////////////////////////////////////////////////////
// CCESettingsDlg dialog

BEGIN_MESSAGE_MAP(CCESettingsDlg, Super)
 ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOkButton)
 ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()

CCESettingsDlg::CCESettingsDlg(CWnd* pParent /*=NULL*/)
: Super(CCESettingsDlg::IDD, pParent)
, mp_data(new SECU3IO::CESettingsData())
, m_map_v_min_edit(CEditEx::MODE_FLOAT)
, m_map_v_max_edit(CEditEx::MODE_FLOAT)
, m_map_v_em_edit(CEditEx::MODE_FLOAT)
, m_vbat_v_min_edit(CEditEx::MODE_FLOAT)
, m_vbat_v_max_edit(CEditEx::MODE_FLOAT)
, m_vbat_v_em_edit(CEditEx::MODE_FLOAT)
, m_cts_v_min_edit(CEditEx::MODE_FLOAT)
, m_cts_v_max_edit(CEditEx::MODE_FLOAT)
, m_cts_v_em_edit(CEditEx::MODE_FLOAT)
, m_ks_v_min_edit(CEditEx::MODE_FLOAT)
, m_ks_v_max_edit(CEditEx::MODE_FLOAT)
, m_ks_v_em_edit(CEditEx::MODE_FLOAT)
, m_tps_v_min_edit(CEditEx::MODE_FLOAT)
, m_tps_v_max_edit(CEditEx::MODE_FLOAT)
, m_tps_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i1_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i1_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i1_v_em_edit(CEditEx::MODE_FLOAT)
, m_add_i2_v_min_edit(CEditEx::MODE_FLOAT)
, m_add_i2_v_max_edit(CEditEx::MODE_FLOAT)
, m_add_i2_v_em_edit(CEditEx::MODE_FLOAT)
{
 //empty
}

CCESettingsDlg::~CCESettingsDlg()
{
 if (mp_data)
  delete mp_data;
}

void CCESettingsDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_CESETT_MAP_V_MIN_EDIT, m_map_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_MAP_V_MIN_SPIN, m_map_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_MAP_V_MAX_EDIT, m_map_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_MAP_V_MAX_SPIN, m_map_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_MAP_V_EM_EDIT, m_map_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_MAP_V_EM_SPIN, m_map_v_em_spin);

 DDX_Control(pDX, IDC_CESETT_VBAT_V_MIN_EDIT, m_vbat_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_MIN_SPIN, m_vbat_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_MAX_EDIT, m_vbat_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_MAX_SPIN, m_vbat_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_EM_EDIT, m_vbat_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_VBAT_V_EM_SPIN, m_vbat_v_em_spin);

 DDX_Control(pDX, IDC_CESETT_CTS_V_MIN_EDIT, m_cts_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_CTS_V_MIN_SPIN, m_cts_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_CTS_V_MAX_EDIT, m_cts_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_CTS_V_MAX_SPIN, m_cts_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_CTS_V_EM_EDIT, m_cts_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_CTS_V_EM_SPIN, m_cts_v_em_spin);

 DDX_Control(pDX, IDC_CESETT_KS_V_MIN_EDIT, m_ks_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_KS_V_MIN_SPIN, m_ks_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_KS_V_MAX_EDIT, m_ks_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_KS_V_MAX_SPIN, m_ks_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_KS_V_EM_EDIT, m_ks_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_KS_V_EM_SPIN, m_ks_v_em_spin);

 DDX_Control(pDX, IDC_CESETT_TPS_V_MIN_EDIT, m_tps_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_TPS_V_MIN_SPIN, m_tps_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_TPS_V_MAX_EDIT, m_tps_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_TPS_V_MAX_SPIN, m_tps_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_TPS_V_EM_EDIT, m_tps_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_TPS_V_EM_SPIN, m_tps_v_em_spin);

 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_MIN_EDIT, m_add_i1_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_MIN_SPIN, m_add_i1_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_MAX_EDIT, m_add_i1_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_MAX_SPIN, m_add_i1_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_EM_EDIT, m_add_i1_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I1_V_EM_SPIN, m_add_i1_v_em_spin);

 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_MIN_EDIT, m_add_i2_v_min_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_MIN_SPIN, m_add_i2_v_min_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_MAX_EDIT, m_add_i2_v_max_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_MAX_SPIN, m_add_i2_v_max_spin);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_EM_EDIT, m_add_i2_v_em_edit);
 DDX_Control(pDX, IDC_CESETT_ADD_I2_V_EM_SPIN, m_add_i2_v_em_spin);

 //Do data exchange and validation
 m_map_v_min_edit.DDX_Value(pDX, IDC_CESETT_MAP_V_MIN_EDIT, mp_data->map_v_min);
 DDV_MinMaxFloat(pDX, mp_data->map_v_min, 0.0f, 5.5f);
 m_map_v_max_edit.DDX_Value(pDX, IDC_CESETT_MAP_V_MAX_EDIT, mp_data->map_v_max);
 DDV_MinMaxFloat(pDX, mp_data->map_v_max, 0.0f, 5.5f);
 m_map_v_em_edit.DDX_Value(pDX, IDC_CESETT_MAP_V_EM_EDIT, mp_data->map_v_em);
 DDV_MinMaxFloat(pDX, mp_data->map_v_em, 0.0f, 5.5f);

 m_vbat_v_min_edit.DDX_Value(pDX, IDC_CESETT_VBAT_V_MIN_EDIT, mp_data->vbat_v_min);
 DDV_MinMaxFloat(pDX, mp_data->vbat_v_min, 8.0f, 18.0f);
 m_vbat_v_max_edit.DDX_Value(pDX, IDC_CESETT_VBAT_V_MAX_EDIT, mp_data->vbat_v_max);
 DDV_MinMaxFloat(pDX, mp_data->vbat_v_max, 8.0f, 18.0f);
 m_vbat_v_em_edit.DDX_Value(pDX, IDC_CESETT_VBAT_V_EM_EDIT, mp_data->vbat_v_em);
 DDV_MinMaxFloat(pDX, mp_data->vbat_v_em, 8.0f, 18.0f);

 m_cts_v_min_edit.DDX_Value(pDX, IDC_CESETT_CTS_V_MIN_EDIT, mp_data->cts_v_min);
 DDV_MinMaxFloat(pDX, mp_data->cts_v_min, 0.0f, 5.5f);
 m_cts_v_max_edit.DDX_Value(pDX, IDC_CESETT_CTS_V_MAX_EDIT, mp_data->cts_v_max);
 DDV_MinMaxFloat(pDX, mp_data->cts_v_max, 0.0f, 5.5f);
 m_cts_v_em_edit.DDX_Value(pDX, IDC_CESETT_CTS_V_EM_EDIT, mp_data->cts_v_em);
 DDV_MinMaxFloat(pDX, mp_data->cts_v_em, 0.0f, 5.5f);

 m_ks_v_min_edit.DDX_Value(pDX, IDC_CESETT_KS_V_MIN_EDIT, mp_data->ks_v_min);
 DDV_MinMaxFloat(pDX, mp_data->ks_v_min, 0.0f, 5.5f);
 m_ks_v_max_edit.DDX_Value(pDX, IDC_CESETT_KS_V_MAX_EDIT, mp_data->ks_v_max);
 DDV_MinMaxFloat(pDX, mp_data->ks_v_max, 0.0f, 5.5f);
 m_ks_v_em_edit.DDX_Value(pDX, IDC_CESETT_KS_V_EM_EDIT, mp_data->ks_v_em);
 DDV_MinMaxFloat(pDX, mp_data->ks_v_em, 0.0f, 5.5f);

 m_tps_v_min_edit.DDX_Value(pDX, IDC_CESETT_TPS_V_MIN_EDIT, mp_data->tps_v_min);
 DDV_MinMaxFloat(pDX, mp_data->tps_v_min, 0.0f, 5.5f);
 m_tps_v_max_edit.DDX_Value(pDX, IDC_CESETT_TPS_V_MAX_EDIT, mp_data->tps_v_max);
 DDV_MinMaxFloat(pDX, mp_data->tps_v_max, 0.0f, 5.5f);
 m_tps_v_em_edit.DDX_Value(pDX, IDC_CESETT_TPS_V_EM_EDIT, mp_data->tps_v_em);
 DDV_MinMaxFloat(pDX, mp_data->tps_v_em, 0.0f, 5.5f);

 m_add_i1_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I1_V_MIN_EDIT, mp_data->add_i1_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_min, 0.0f, 5.5f);
 m_add_i1_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I1_V_MAX_EDIT, mp_data->add_i1_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_max, 0.0f, 5.5f);
 m_add_i1_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I1_V_EM_EDIT, mp_data->add_i1_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_em, 0.0f, 5.5f);

 m_add_i2_v_min_edit.DDX_Value(pDX, IDC_CESETT_ADD_I2_V_MIN_EDIT, mp_data->add_i2_v_min);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_min, 0.0f, 5.5f);
 m_add_i2_v_max_edit.DDX_Value(pDX, IDC_CESETT_ADD_I2_V_MAX_EDIT, mp_data->add_i2_v_max);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_max, 0.0f, 5.5f);
 m_add_i2_v_em_edit.DDX_Value(pDX, IDC_CESETT_ADD_I2_V_EM_EDIT, mp_data->add_i2_v_em);
 DDV_MinMaxFloat(pDX, mp_data->add_i1_v_em, 0.0f, 5.5f);

}

/////////////////////////////////////////////////////////////////////////////
// CCESettingsDlg message handlers

void CCESettingsDlg::OnOK()
{
 bool valid = UpdateData(true);
 if (!valid)
  return;

 Super::OnOK();
}

void CCESettingsDlg::OnCancel()
{
 Super::OnCancel();
}

void CCESettingsDlg::OnUpdateOkButton(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(true);
}

BOOL CCESettingsDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_map_v_min_spin.SetBuddy(&m_map_v_min_edit);
 m_map_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_map_v_min_edit.SetLimitText(6);
 m_map_v_min_edit.SetDecimalPlaces(3);
 m_map_v_max_spin.SetBuddy(&m_map_v_max_edit);
 m_map_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_map_v_max_edit.SetLimitText(6);
 m_map_v_max_edit.SetDecimalPlaces(3);
 m_map_v_em_spin.SetBuddy(&m_map_v_em_edit);
 m_map_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_map_v_em_edit.SetLimitText(6);
 m_map_v_em_edit.SetDecimalPlaces(3);

 m_vbat_v_min_spin.SetBuddy(&m_vbat_v_min_edit);
 m_vbat_v_min_spin.SetRangeAndDelta(8.0, 18.0, 0.01);
 m_vbat_v_min_edit.SetLimitText(6);
 m_vbat_v_min_edit.SetDecimalPlaces(3);
 m_vbat_v_max_spin.SetBuddy(&m_vbat_v_max_edit);
 m_vbat_v_max_spin.SetRangeAndDelta(8.0, 18.0, 0.01);
 m_vbat_v_max_edit.SetLimitText(6);
 m_vbat_v_max_edit.SetDecimalPlaces(3);
 m_vbat_v_em_spin.SetBuddy(&m_vbat_v_em_edit);
 m_vbat_v_em_spin.SetRangeAndDelta(8.0, 18.0, 0.01);
 m_vbat_v_em_edit.SetLimitText(6);
 m_vbat_v_em_edit.SetDecimalPlaces(3);

 m_cts_v_min_spin.SetBuddy(&m_cts_v_min_edit);
 m_cts_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_cts_v_min_edit.SetLimitText(6);
 m_cts_v_min_edit.SetDecimalPlaces(3);
 m_cts_v_max_spin.SetBuddy(&m_cts_v_max_edit);
 m_cts_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_cts_v_max_edit.SetLimitText(6);
 m_cts_v_max_edit.SetDecimalPlaces(3);
 m_cts_v_em_spin.SetBuddy(&m_cts_v_em_edit);
 m_cts_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_cts_v_em_edit.SetLimitText(6);
 m_cts_v_em_edit.SetDecimalPlaces(3);

 m_ks_v_min_spin.SetBuddy(&m_ks_v_min_edit);
 m_ks_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_ks_v_min_edit.SetLimitText(6);
 m_ks_v_min_edit.SetDecimalPlaces(3);
 m_ks_v_max_spin.SetBuddy(&m_ks_v_max_edit);
 m_ks_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_ks_v_max_edit.SetLimitText(6);
 m_ks_v_max_edit.SetDecimalPlaces(3);
 m_ks_v_em_spin.SetBuddy(&m_ks_v_em_edit);
 m_ks_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_ks_v_em_edit.SetLimitText(6);
 m_ks_v_em_edit.SetDecimalPlaces(3);

 m_tps_v_min_spin.SetBuddy(&m_tps_v_min_edit);
 m_tps_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_tps_v_min_edit.SetLimitText(6);
 m_tps_v_min_edit.SetDecimalPlaces(3);
 m_tps_v_max_spin.SetBuddy(&m_tps_v_max_edit);
 m_tps_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_tps_v_max_edit.SetLimitText(6);
 m_tps_v_max_edit.SetDecimalPlaces(3);
 m_tps_v_em_spin.SetBuddy(&m_tps_v_em_edit);
 m_tps_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_tps_v_em_edit.SetLimitText(6);
 m_tps_v_em_edit.SetDecimalPlaces(3);

 m_add_i1_v_min_spin.SetBuddy(&m_add_i1_v_min_edit);
 m_add_i1_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i1_v_min_edit.SetLimitText(6);
 m_add_i1_v_min_edit.SetDecimalPlaces(3);
 m_add_i1_v_max_spin.SetBuddy(&m_add_i1_v_max_edit);
 m_add_i1_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i1_v_max_edit.SetLimitText(6);
 m_add_i1_v_max_edit.SetDecimalPlaces(3);
 m_add_i1_v_em_spin.SetBuddy(&m_add_i1_v_em_edit);
 m_add_i1_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i1_v_em_edit.SetLimitText(6);
 m_add_i1_v_em_edit.SetDecimalPlaces(3);

 m_add_i2_v_min_spin.SetBuddy(&m_add_i2_v_min_edit);
 m_add_i2_v_min_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i2_v_min_edit.SetLimitText(6);
 m_add_i2_v_min_edit.SetDecimalPlaces(3);
 m_add_i2_v_max_spin.SetBuddy(&m_add_i2_v_max_edit);
 m_add_i2_v_max_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i2_v_max_edit.SetLimitText(6);
 m_add_i2_v_max_edit.SetDecimalPlaces(3);
 m_add_i2_v_em_spin.SetBuddy(&m_add_i2_v_em_edit);
 m_add_i2_v_em_spin.SetRangeAndDelta(0, 5.5, 0.01);
 m_add_i2_v_em_edit.SetLimitText(6);
 m_add_i2_v_em_edit.SetDecimalPlaces(3);

 UpdateData(FALSE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT CCESettingsDlg::OnKickIdle(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
 UpdateDialogControls(this, true);
 return FALSE;
}

void CCESettingsDlg::SetValues(const SECU3IO::CESettingsData& i_data)
{
 if (mp_data)
  *mp_data = i_data;
}

void CCESettingsDlg::GetValues(SECU3IO::CESettingsData& o_data)
{
 if (mp_data)
  o_data = *mp_data;
}
