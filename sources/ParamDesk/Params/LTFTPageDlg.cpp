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

/** \file LTFTPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "LTFTPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"
#include "ui-core/ddx_helpers.h"
#include "ui-core/fnt_helpers.h"
#include "common/dpiaware.h"

BEGIN_MESSAGE_MAP(CLTFTPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_CBN_SELCHANGE(IDC_PD_LTFT_MODE_COMBO, OnChangeData)

 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_CLT_DW_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_CLT_UP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_IAT_UP_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_GRAD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_GPA_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_GPD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_MIN_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_MAX_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_RPM0_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_RPM1_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_LOAD0_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_LEARN_LOAD1_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_DEAD_BAND0_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_LTFT_DEAD_BAND1_EDIT, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MODE_COMBO,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MODE_CAPTION,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_CLT_DW_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_CLT_DW_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_CLT_DW_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_CLT_DW_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_CLT_UP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_CLT_UP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_CLT_UP_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_CLT_UP_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_IAT_UP_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_IAT_UP_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_IAT_UP_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_IAT_UP_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GRAD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GRAD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GRAD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GRAD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GPA_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GPA_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GPA_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GPA_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GPD_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GPD_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GPD_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_GPD_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MIN_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MIN_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MIN_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MIN_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MAX_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MAX_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MAX_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_MAX_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_RPM0_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_RPM0_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_RPM0_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_RPM0_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_RPM1_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_RPM1_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_RPM1_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_RPM1_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_LOAD0_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_LOAD0_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_LOAD0_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_LOAD0_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_LOAD1_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_LOAD1_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_LOAD1_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_LEARN_LOAD1_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_DEAD_BAND0_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_DEAD_BAND0_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_DEAD_BAND0_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_DEAD_BAND0_UNIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_DEAD_BAND1_EDIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_DEAD_BAND1_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_DEAD_BAND1_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_LTFT_DEAD_BAND1_UNIT,OnUpdateControls)
END_MESSAGE_MAP()

CLTFTPageDlg::CLTFTPageDlg()
: m_enabled(false)
, mp_scr(new CWndScroller)
, m_ltft_learn_clt_dw_edit(CEditEx::MODE_FLOAT, true)
, m_ltft_learn_clt_up_edit(CEditEx::MODE_FLOAT, true)
, m_ltft_learn_iat_up_edit(CEditEx::MODE_FLOAT, true)
, m_ltft_learn_grad_edit(CEditEx::MODE_FLOAT, true)
, m_ltft_learn_gpa_edit(CEditEx::MODE_FLOAT, true)
, m_ltft_learn_gpd_edit(CEditEx::MODE_FLOAT, true)
, m_ltft_min_edit(CEditEx::MODE_FLOAT | CEditEx::MODE_SIGNED, true)
, m_ltft_max_edit(CEditEx::MODE_FLOAT, true)
{
 m_ltft_learn_rpm_edit[0].SetMode(CEditEx::MODE_INT);
 m_ltft_learn_rpm_edit[0].SetOwnDDV(true);
 m_ltft_learn_load_edit[0].SetMode(CEditEx::MODE_FLOAT);
 m_ltft_learn_load_edit[0].SetOwnDDV(true);
 m_ltft_dead_band_edit[0].SetMode(CEditEx::MODE_FLOAT);
 m_ltft_dead_band_edit[0].SetOwnDDV(true);
 m_ltft_learn_rpm_edit[1].SetMode(CEditEx::MODE_INT);
 m_ltft_learn_rpm_edit[1].SetOwnDDV(true);
 m_ltft_learn_load_edit[1].SetMode(CEditEx::MODE_FLOAT);
 m_ltft_learn_load_edit[1].SetOwnDDV(true);
 m_ltft_dead_band_edit[1].SetMode(CEditEx::MODE_FLOAT);
 m_ltft_dead_band_edit[1].SetOwnDDV(true);

 m_params.ltft_mode = 0;
 m_params.ltft_learn_clt = 90.0f;
 m_params.ltft_learn_clt_up = 96.0f;
 m_params.ltft_learn_iat_up = 45.0f;
 m_params.ltft_learn_grad = 0.05f;
 m_params.ltft_learn_gpa = 0.0f;
 m_params.ltft_learn_gpd = 0.0f;
 m_params.ltft_min = -24.6f;
 m_params.ltft_max =  24.6f;
 m_params.ltft_learn_rpm[0] = 500;
 m_params.ltft_learn_rpm[1] = 600;
 m_params.ltft_learn_load[0] = 10.0;
 m_params.ltft_learn_load[1] = 100.0f;
 m_params.ltft_dead_band[0] = 0.6f;
 m_params.ltft_dead_band[1] = 0.6f;
}

CLTFTPageDlg::~CLTFTPageDlg()
{
 //empty
}

LPCTSTR CLTFTPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PD_LTFT_PAGE;
}

void CLTFTPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX,IDC_PD_LTFT_MODE_COMBO, m_ltft_mode_combo);

 DDX_Control(pDX,IDC_PD_LTFT_LEARN_CLT_DW_EDIT, m_ltft_learn_clt_dw_edit);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_CLT_DW_SPIN, m_ltft_learn_clt_dw_spin);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_CLT_UP_EDIT, m_ltft_learn_clt_up_edit);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_CLT_UP_SPIN, m_ltft_learn_clt_up_spin);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_IAT_UP_EDIT, m_ltft_learn_iat_up_edit);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_IAT_UP_SPIN, m_ltft_learn_iat_up_spin);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_GRAD_EDIT, m_ltft_learn_grad_edit);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_GRAD_SPIN, m_ltft_learn_grad_spin);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_GPA_EDIT, m_ltft_learn_gpa_edit);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_GPA_SPIN, m_ltft_learn_gpa_spin);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_GPD_EDIT, m_ltft_learn_gpd_edit);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_GPD_SPIN, m_ltft_learn_gpd_spin);
 DDX_Control(pDX,IDC_PD_LTFT_MIN_EDIT, m_ltft_min_edit);
 DDX_Control(pDX,IDC_PD_LTFT_MIN_SPIN, m_ltft_min_spin);
 DDX_Control(pDX,IDC_PD_LTFT_MAX_EDIT, m_ltft_max_edit);
 DDX_Control(pDX,IDC_PD_LTFT_MAX_SPIN, m_ltft_max_spin);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_RPM0_EDIT, m_ltft_learn_rpm_edit[0]);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_RPM0_SPIN, m_ltft_learn_rpm_spin[0]);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_RPM1_EDIT, m_ltft_learn_rpm_edit[1]);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_RPM1_SPIN, m_ltft_learn_rpm_spin[1]);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_LOAD0_EDIT, m_ltft_learn_load_edit[0]);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_LOAD0_SPIN, m_ltft_learn_load_spin[0]);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_LOAD1_EDIT, m_ltft_learn_load_edit[1]);
 DDX_Control(pDX,IDC_PD_LTFT_LEARN_LOAD1_SPIN, m_ltft_learn_load_spin[1]);
 DDX_Control(pDX,IDC_PD_LTFT_DEAD_BAND0_EDIT, m_ltft_dead_band_edit[0]);
 DDX_Control(pDX,IDC_PD_LTFT_DEAD_BAND0_SPIN, m_ltft_dead_band_spin[0]);
 DDX_Control(pDX,IDC_PD_LTFT_DEAD_BAND1_EDIT, m_ltft_dead_band_edit[1]);
 DDX_Control(pDX,IDC_PD_LTFT_DEAD_BAND1_SPIN, m_ltft_dead_band_spin[1]);

 DDX_CBIndex_int(pDX, IDC_PD_LTFT_MODE_COMBO, m_params.ltft_mode);
 m_ltft_learn_clt_dw_edit.DDX_Value(pDX, IDC_PD_LTFT_LEARN_CLT_DW_EDIT, m_params.ltft_learn_clt);
 m_ltft_learn_clt_up_edit.DDX_Value(pDX, IDC_PD_LTFT_LEARN_CLT_UP_EDIT, m_params.ltft_learn_clt_up);
 m_ltft_learn_iat_up_edit.DDX_Value(pDX, IDC_PD_LTFT_LEARN_IAT_UP_EDIT, m_params.ltft_learn_iat_up);
 m_ltft_learn_grad_edit.DDX_Value(pDX, IDC_PD_LTFT_LEARN_GRAD_EDIT, m_params.ltft_learn_grad);
 m_ltft_learn_gpa_edit.DDX_Value(pDX, IDC_PD_LTFT_LEARN_GPA_EDIT, m_params.ltft_learn_gpa);
 m_ltft_learn_gpd_edit.DDX_Value(pDX, IDC_PD_LTFT_LEARN_GPD_EDIT,m_params.ltft_learn_gpd);
 m_ltft_min_edit.DDX_Value(pDX, IDC_PD_LTFT_MIN_EDIT,m_params.ltft_min);
 m_ltft_max_edit.DDX_Value(pDX, IDC_PD_LTFT_MAX_EDIT,m_params.ltft_max);
 m_ltft_learn_rpm_edit[0].DDX_Value(pDX,IDC_PD_LTFT_LEARN_RPM0_EDIT, m_params.ltft_learn_rpm[0]);
 m_ltft_learn_rpm_edit[1].DDX_Value(pDX,IDC_PD_LTFT_LEARN_RPM1_EDIT, m_params.ltft_learn_rpm[1]);
 m_ltft_learn_load_edit[0].DDX_Value(pDX,IDC_PD_LTFT_LEARN_LOAD0_EDIT, m_params.ltft_learn_load[0]);
 m_ltft_learn_load_edit[1].DDX_Value(pDX,IDC_PD_LTFT_LEARN_LOAD1_EDIT, m_params.ltft_learn_load[1]);
 m_ltft_dead_band_edit[0].DDX_Value(pDX,IDC_PD_LTFT_DEAD_BAND0_EDIT, m_params.ltft_dead_band[0]);
 m_ltft_dead_band_edit[1].DDX_Value(pDX,IDC_PD_LTFT_DEAD_BAND1_EDIT, m_params.ltft_dead_band[1]);
}

void CLTFTPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

/////////////////////////////////////////////////////////////////////////////
// CLTFTPageDlg message handlers

BOOL CLTFTPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_ltft_mode_combo.AddString(MLL::LoadString(IDS_PD_LTFT_MODE_OFF));
 m_ltft_mode_combo.AddString(MLL::LoadString(IDS_PD_LTFT_MODE_PETROL));
 m_ltft_mode_combo.AddString(MLL::LoadString(IDS_PD_LTFT_MODE_GAS));
 m_ltft_mode_combo.AddString(MLL::LoadString(IDS_PD_LTFT_MODE_PETGAS));

 m_ltft_learn_clt_dw_spin.SetBuddy(&m_ltft_learn_clt_dw_edit);
 m_ltft_learn_clt_dw_edit.SetLimitText(6);
 m_ltft_learn_clt_dw_edit.SetDecimalPlaces(2);
 m_ltft_learn_clt_dw_spin.SetRangeAndDelta(0, 270.0f, 0.25f);
 m_ltft_learn_clt_dw_edit.SetRange(0, 270.0f);

 m_ltft_learn_clt_up_spin.SetBuddy(&m_ltft_learn_clt_up_edit);
 m_ltft_learn_clt_up_edit.SetLimitText(6);
 m_ltft_learn_clt_up_edit.SetDecimalPlaces(2);
 m_ltft_learn_clt_up_spin.SetRangeAndDelta(0, 270.0f, 0.25f);
 m_ltft_learn_clt_up_edit.SetRange(0, 270.0f);

 m_ltft_learn_iat_up_spin.SetBuddy(&m_ltft_learn_iat_up_edit);
 m_ltft_learn_iat_up_edit.SetLimitText(6);
 m_ltft_learn_iat_up_edit.SetDecimalPlaces(2);
 m_ltft_learn_iat_up_spin.SetRangeAndDelta(0, 270.0f, 0.25f);
 m_ltft_learn_iat_up_edit.SetRange(0, 270.0f);

 m_ltft_learn_grad_spin.SetBuddy(&m_ltft_learn_grad_edit);
 m_ltft_learn_grad_edit.SetLimitText(5);
 m_ltft_learn_grad_edit.SetDecimalPlaces(3);
 m_ltft_learn_grad_spin.SetRangeAndDelta(0, 0.99f, 0.005f);
 m_ltft_learn_grad_edit.SetRange(0, 0.99f);

 m_ltft_learn_gpa_spin.SetBuddy(&m_ltft_learn_gpa_edit);
 m_ltft_learn_gpa_edit.SetLimitText(6);
 m_ltft_learn_gpa_edit.SetDecimalPlaces(1);
 m_ltft_learn_gpa_spin.SetRangeAndDelta(0, 500.0f, 0.1f);
 m_ltft_learn_gpa_edit.SetRange(0, 500.0f);

 m_ltft_learn_gpd_spin.SetBuddy(&m_ltft_learn_gpd_edit);
 m_ltft_learn_gpd_edit.SetLimitText(6);
 m_ltft_learn_gpd_edit.SetDecimalPlaces(1);
 m_ltft_learn_gpd_spin.SetRangeAndDelta(0, 500.0f, 0.1f);
 m_ltft_learn_gpd_edit.SetRange(0, 500.0f);

 m_ltft_min_spin.SetBuddy(&m_ltft_min_edit);
 m_ltft_min_edit.SetLimitText(6);
 m_ltft_min_edit.SetDecimalPlaces(1);
 m_ltft_min_spin.SetRangeAndDelta(-24.6f, .0f, 0.1f);
 m_ltft_min_edit.SetRange(-24.6f, .0f);

 m_ltft_max_spin.SetBuddy(&m_ltft_max_edit);
 m_ltft_max_edit.SetLimitText(6);
 m_ltft_max_edit.SetDecimalPlaces(1);
 m_ltft_max_spin.SetRangeAndDelta(.0f, 24.6f, 0.1f);
 m_ltft_max_edit.SetRange(.0f, 24.6f);

 m_ltft_learn_rpm_spin[0].SetBuddy(&m_ltft_learn_rpm_edit[0]);
 m_ltft_learn_rpm_edit[0].SetLimitText(6);
 m_ltft_learn_rpm_edit[0].SetDecimalPlaces(5);
 m_ltft_learn_rpm_spin[0].SetRangeAndDelta(0, 25000, 10);
 m_ltft_learn_rpm_edit[0].SetRange(0, 25000);

 m_ltft_learn_rpm_spin[1].SetBuddy(&m_ltft_learn_rpm_edit[1]);
 m_ltft_learn_rpm_edit[1].SetLimitText(6);
 m_ltft_learn_rpm_edit[1].SetDecimalPlaces(5);
 m_ltft_learn_rpm_spin[1].SetRangeAndDelta(0, 25000, 10);
 m_ltft_learn_rpm_edit[1].SetRange(0, 25000);

 m_ltft_learn_load_spin[0].SetBuddy(&m_ltft_learn_load_edit[0]);
 m_ltft_learn_load_edit[0].SetLimitText(6);
 m_ltft_learn_load_edit[0].SetDecimalPlaces(2);
 m_ltft_learn_load_spin[0].SetRangeAndDelta(0.0f, 500.0f, 1.0f);
 m_ltft_learn_load_edit[0].SetRange(0.0f, 500.0f);

 m_ltft_learn_load_spin[1].SetBuddy(&m_ltft_learn_load_edit[1]);
 m_ltft_learn_load_edit[1].SetLimitText(6);
 m_ltft_learn_load_edit[1].SetDecimalPlaces(2);
 m_ltft_learn_load_spin[1].SetRangeAndDelta(0.0f, 500.0f, 1.0f);
 m_ltft_learn_load_edit[1].SetRange(0.0f, 500.0f);

 m_ltft_dead_band_spin[0].SetBuddy(&m_ltft_dead_band_edit[0]);
 m_ltft_dead_band_edit[0].SetLimitText(5);
 m_ltft_dead_band_edit[0].SetDecimalPlaces(1);
 m_ltft_dead_band_spin[0].SetRangeAndDelta(0.0f, 10.0f, 0.1f);
 m_ltft_dead_band_edit[0].SetRange(0.0f, 10.0f);

 m_ltft_dead_band_spin[1].SetBuddy(&m_ltft_dead_band_edit[1]);
 m_ltft_dead_band_edit[1].SetLimitText(5);
 m_ltft_dead_band_edit[1].SetDecimalPlaces(1);
 m_ltft_dead_band_spin[1].SetRangeAndDelta(0.0f, 10.0f, 0.1f);
 m_ltft_dead_band_edit[1].SetRange(0.0f, 10.0f);

 //initialize window scroller
 mp_scr->Init(this);

 //create a tooltip control and assign tooltips
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));
 VERIFY(mp_ttc->AddWindow(&m_ltft_mode_combo, MLL::GetString(IDS_PD_LTFT_MODE_COMBO_TT)));

 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_clt_dw_edit, MLL::GetString(IDS_PD_LTFT_LEARN_CLT_DW_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_clt_dw_spin, MLL::GetString(IDS_PD_LTFT_LEARN_CLT_DW_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_clt_up_edit, MLL::GetString(IDS_PD_LTFT_LEARN_CLT_UP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_clt_up_spin, MLL::GetString(IDS_PD_LTFT_LEARN_CLT_UP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_iat_up_edit, MLL::GetString(IDS_PD_LTFT_LEARN_IAT_UP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_iat_up_spin, MLL::GetString(IDS_PD_LTFT_LEARN_IAT_UP_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_grad_edit, MLL::GetString(IDS_PD_LTFT_LEARN_GRAD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_grad_spin, MLL::GetString(IDS_PD_LTFT_LEARN_GRAD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_gpa_edit, MLL::GetString(IDS_PD_LTFT_LEARN_GPA_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_gpa_spin, MLL::GetString(IDS_PD_LTFT_LEARN_GPA_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_gpd_edit, MLL::GetString(IDS_PD_LTFT_LEARN_GPD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_gpd_spin, MLL::GetString(IDS_PD_LTFT_LEARN_GPD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_min_edit, MLL::GetString(IDS_PD_LTFT_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_min_spin, MLL::GetString(IDS_PD_LTFT_MIN_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_max_edit, MLL::GetString(IDS_PD_LTFT_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_max_spin, MLL::GetString(IDS_PD_LTFT_MAX_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_rpm_edit[0], MLL::GetString(IDS_PD_LTFT_LEARN_RPM0_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_rpm_spin[0], MLL::GetString(IDS_PD_LTFT_LEARN_RPM0_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_rpm_edit[1], MLL::GetString(IDS_PD_LTFT_LEARN_RPM1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_rpm_spin[1], MLL::GetString(IDS_PD_LTFT_LEARN_RPM1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_load_edit[0], MLL::GetString(IDS_PD_LTFT_LEARN_LOAD0_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_load_spin[0], MLL::GetString(IDS_PD_LTFT_LEARN_LOAD0_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_load_edit[1], MLL::GetString(IDS_PD_LTFT_LEARN_LOAD1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_learn_load_spin[1], MLL::GetString(IDS_PD_LTFT_LEARN_LOAD1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_dead_band_edit[0], MLL::GetString(IDS_PD_LTFT_DEAD_BAND0_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_dead_band_spin[0], MLL::GetString(IDS_PD_LTFT_DEAD_BAND0_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_dead_band_edit[1], MLL::GetString(IDS_PD_LTFT_DEAD_BAND1_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_ltft_dead_band_spin[1], MLL::GetString(IDS_PD_LTFT_DEAD_BAND1_EDIT_TT)));
 
 mp_ttc->SetMaxTipWidth(250); //Enable text wrapping
 mp_ttc->ActivateToolTips(true);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);
 return TRUE;  // return TRUE unless you set the focus to a control
}

void CLTFTPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CLTFTPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change in view content(see class ParamPageEvents)
}

//Enable/Disable controls (all)
void CLTFTPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//что с контроллами?
bool CLTFTPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//Use this function for obtaining data from dialog
void CLTFTPageDlg::GetValues(SECU3IO::LtftPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog controls to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::LtftPar));
}

//Use this function to set dialog data
void CLTFTPageDlg::SetValues(const SECU3IO::LtftPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::LtftPar));
 UpdateData(FALSE); //copy data from variables to dialog controls
 UpdateDialogControls(this, TRUE);
}

void CLTFTPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(600));
}
