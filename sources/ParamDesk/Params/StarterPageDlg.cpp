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

/** \file StarterPageDlg.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "Resources/resource.h"
#include "common/dpiaware.h"
#include "StarterPageDlg.h"
#include "ui-core/ToolTipCtrlEx.h"
#include "ui-core/WndScroller.h"
#include "ui-core/DDX_helpers.h"

BEGIN_MESSAGE_MAP(CStarterPageDlg, Super)
 ON_WM_DESTROY()
 ON_WM_SIZE()
 ON_EN_CHANGE(IDC_PD_STARTER_OFF_RPM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_SMAP_ABANDON_RPM_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_CRANKTORUNTIME_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_AFTSTRSTR_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_AFTSTRSTR1_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_PRIMECOLD_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_PRIMEHOT_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_PRIMEDELAY_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_FLDCLRTPS_EDIT, OnChangeData)
 ON_EN_CHANGE(IDC_PD_STARTER_STBLSTRCNT_EDIT, OnChangeData)
 ON_BN_CLICKED(IDC_PD_STARTER_FLDCLRSTART_CHECK, OnChangeData)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_SMAP_ABANDON_RPM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_SMAP_ABANDON_RPM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_SMAP_ABANDON_RPM_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_SMAP_ABANDON_RPM_EDIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_OFF_RPM_SPIN,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_OFF_RPM_CAPTION,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_OFF_RPM_UNIT,OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_OFF_RPM_EDIT,OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_CRANKTORUNTIME_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_CRANKTORUNTIME_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_CRANKTORUNTIME_UNIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_CRANKTORUNTIME_EDIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_AFTSTRSTR_SPIN,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_AFTSTRSTR_CAPTION,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_AFTSTRSTR_UNIT,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_AFTSTRSTR_EDIT,OnUpdateInjGasControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_AFTSTRSTR1_SPIN,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_AFTSTRSTR1_CAPTION,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_AFTSTRSTR1_UNIT,OnUpdateInjGasControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_AFTSTRSTR1_EDIT,OnUpdateInjGasControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMECOLD_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMECOLD_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMECOLD_UNIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMECOLD_EDIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMEHOT_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMEHOT_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMEHOT_UNIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMEHOT_EDIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMEDELAY_SPIN,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMEDELAY_CAPTION,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMEDELAY_UNIT,OnUpdateFuelInjectionControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_PRIMEDELAY_EDIT,OnUpdateFuelInjectionControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_FLDCLRTPS_SPIN,OnUpdateInjGasChokeControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_FLDCLRTPS_CAPTION,OnUpdateInjGasChokeControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_FLDCLRTPS_UNIT,OnUpdateInjGasChokeControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_FLDCLRTPS_EDIT,OnUpdateInjGasChokeControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_STBLSTRCNT_SPIN, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_STBLSTRCNT_CAPTION, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_STBLSTRCNT_UNIT, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_STBLSTRCNT_EDIT, OnUpdateControls)

 ON_UPDATE_COMMAND_UI(IDC_PD_STARTER_FLDCLRSTART_CHECK, OnUpdateInjGasChokeControls)
END_MESSAGE_MAP()

CStarterPageDlg::CStarterPageDlg()
: m_enabled(false)
, m_fuel_injection(false)
, m_gasdose(false)
, m_choke(false)
, m_starter_off_rpm_edit(CEditEx::MODE_INT, true)
, m_smap_abandon_rpm_edit(CEditEx::MODE_INT, true)
, m_cranktoruntime_edit(CEditEx::MODE_FLOAT, true)
, m_primecold_edit(CEditEx::MODE_FLOAT, true)
, m_primehot_edit(CEditEx::MODE_FLOAT, true)
, m_primedelay_edit(CEditEx::MODE_FLOAT, true)
, m_fldclrtps_edit(CEditEx::MODE_FLOAT, true)
, m_stblstrcnt_edit(CEditEx::MODE_INT, true)
, mp_scr(new CWndScroller)
{
 m_aftstrstr_edit[0].SetMode(CEditEx::MODE_INT);
 m_aftstrstr_edit[0].SetOwnDDV(true);
 m_aftstrstr_edit[1].SetMode(CEditEx::MODE_INT);
 m_aftstrstr_edit[1].SetOwnDDV(true);

 m_params.starter_off  = 600;
 m_params.smap_abandon = 700;
 m_params.inj_cranktorun_time = 3.00f;
 m_params.inj_aftstr_strokes[0] = 150;
 m_params.inj_aftstr_strokes[1] = 150;
 m_params.inj_prime_cold =6.0f;
 m_params.inj_prime_hot = 2.0f;
 m_params.inj_prime_delay = 2.0f;
 m_params.inj_floodclear_tps = 75.0f;
 m_params.stbl_str_cnt = 10;
 m_params.fldclr_start = true;
}

LPCTSTR CStarterPageDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD_PD_STARTER_PAGE;
}

void CStarterPageDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_PD_STARTER_SMAP_ABANDON_RPM_SPIN, m_smap_abandon_rpm_spin);
 DDX_Control(pDX, IDC_PD_STARTER_OFF_RPM_SPIN, m_starter_off_rpm_spin);
 DDX_Control(pDX, IDC_PD_STARTER_SMAP_ABANDON_RPM_EDIT, m_smap_abandon_rpm_edit);
 DDX_Control(pDX, IDC_PD_STARTER_OFF_RPM_EDIT, m_starter_off_rpm_edit);
 DDX_Control(pDX, IDC_PD_STARTER_CRANKTORUNTIME_EDIT, m_cranktoruntime_edit);
 DDX_Control(pDX, IDC_PD_STARTER_CRANKTORUNTIME_SPIN, m_cranktoruntime_spin);
 DDX_Control(pDX, IDC_PD_STARTER_AFTSTRSTR_EDIT, m_aftstrstr_edit[0]);
 DDX_Control(pDX, IDC_PD_STARTER_AFTSTRSTR_SPIN, m_aftstrstr_spin[0]);
 DDX_Control(pDX, IDC_PD_STARTER_AFTSTRSTR1_EDIT, m_aftstrstr_edit[1]);
 DDX_Control(pDX, IDC_PD_STARTER_AFTSTRSTR1_SPIN, m_aftstrstr_spin[1]);
 DDX_Control(pDX, IDC_PD_STARTER_PRIMECOLD_EDIT, m_primecold_edit);
 DDX_Control(pDX, IDC_PD_STARTER_PRIMECOLD_SPIN, m_primecold_spin);
 DDX_Control(pDX, IDC_PD_STARTER_PRIMEHOT_EDIT, m_primehot_edit);
 DDX_Control(pDX, IDC_PD_STARTER_PRIMEHOT_SPIN, m_primehot_spin);
 DDX_Control(pDX, IDC_PD_STARTER_PRIMEDELAY_EDIT, m_primedelay_edit);
 DDX_Control(pDX, IDC_PD_STARTER_PRIMEDELAY_SPIN, m_primedelay_spin);
 DDX_Control(pDX, IDC_PD_STARTER_FLDCLRTPS_EDIT, m_fldclrtps_edit);
 DDX_Control(pDX, IDC_PD_STARTER_FLDCLRTPS_SPIN, m_fldclrtps_spin);
 DDX_Control(pDX, IDC_PD_STARTER_STBLSTRCNT_EDIT, m_stblstrcnt_edit);
 DDX_Control(pDX, IDC_PD_STARTER_STBLSTRCNT_SPIN, m_stblstrcnt_spin);
 DDX_Control(pDX, IDC_PD_STARTER_FLDCLRSTART_CHECK, m_fldclrstart_check);

 m_starter_off_rpm_edit.DDX_Value(pDX, IDC_PD_STARTER_OFF_RPM_EDIT, m_params.starter_off);
 m_smap_abandon_rpm_edit.DDX_Value(pDX, IDC_PD_STARTER_SMAP_ABANDON_RPM_EDIT, m_params.smap_abandon);
 m_cranktoruntime_edit.DDX_Value(pDX, IDC_PD_STARTER_CRANKTORUNTIME_EDIT, m_params.inj_cranktorun_time);
 m_aftstrstr_edit[0].DDX_Value(pDX, IDC_PD_STARTER_AFTSTRSTR_EDIT, m_params.inj_aftstr_strokes[0]);
 m_aftstrstr_edit[1].DDX_Value(pDX, IDC_PD_STARTER_AFTSTRSTR1_EDIT, m_params.inj_aftstr_strokes[1]);
 m_primecold_edit.DDX_Value(pDX, IDC_PD_STARTER_PRIMECOLD_EDIT, m_params.inj_prime_cold);
 m_primehot_edit.DDX_Value(pDX, IDC_PD_STARTER_PRIMEHOT_EDIT, m_params.inj_prime_hot);
 m_primedelay_edit.DDX_Value(pDX, IDC_PD_STARTER_PRIMEDELAY_EDIT, m_params.inj_prime_delay);
 m_fldclrtps_edit.DDX_Value(pDX, IDC_PD_STARTER_FLDCLRTPS_EDIT, m_params.inj_floodclear_tps);
 m_stblstrcnt_edit.DDX_Value(pDX, IDC_PD_STARTER_STBLSTRCNT_EDIT, m_params.stbl_str_cnt);
 DDX_Check_bool(pDX, IDC_PD_STARTER_FLDCLRSTART_CHECK, m_params.fldclr_start);
}

void CStarterPageDlg::OnUpdateControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled);
}

void CStarterPageDlg::OnUpdateFuelInjectionControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && m_fuel_injection);
}

void CStarterPageDlg::OnUpdateInjGasControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (m_fuel_injection || m_gasdose));
}

void CStarterPageDlg::OnUpdateInjGasChokeControls(CCmdUI* pCmdUI)
{
 pCmdUI->Enable(m_enabled && (m_fuel_injection || m_gasdose || m_choke));
}

/////////////////////////////////////////////////////////////////////////////
// CStarterPageDlg message handlers

BOOL CStarterPageDlg::OnInitDialog()
{
 Super::OnInitDialog();

 m_smap_abandon_rpm_edit.SetLimitText(4);
 m_smap_abandon_rpm_spin.SetBuddy(&m_smap_abandon_rpm_edit);
 m_smap_abandon_rpm_spin.SetRangeAndDelta(0, 6000, 10);
 m_smap_abandon_rpm_edit.SetRange(0, 6000);

 m_starter_off_rpm_edit.SetLimitText(4);
 m_starter_off_rpm_spin.SetBuddy(&m_starter_off_rpm_edit);
 m_starter_off_rpm_spin.SetRangeAndDelta(0, 6000, 10);
 m_starter_off_rpm_edit.SetRange(0, 6000);

 m_cranktoruntime_edit.SetLimitText(5);
 m_cranktoruntime_spin.SetBuddy(&m_cranktoruntime_edit);
 m_cranktoruntime_edit.SetDecimalPlaces(2);
 m_cranktoruntime_spin.SetRangeAndDelta(0.10f,99.00f, 0.01f);
 m_cranktoruntime_edit.SetRange(0.10f, 99.00f);

 m_aftstrstr_edit[0].SetLimitText(4);
 m_aftstrstr_spin[0].SetBuddy(&m_aftstrstr_edit[0]);
 m_aftstrstr_spin[0].SetRangeAndDelta(0, 1020, 4);  //strokes
 m_aftstrstr_edit[0].SetRange(0, 1020);

 m_aftstrstr_edit[1].SetLimitText(4);
 m_aftstrstr_spin[1].SetBuddy(&m_aftstrstr_edit[1]);
 m_aftstrstr_spin[1].SetRangeAndDelta(0, 1020, 4);  //strokes
 m_aftstrstr_edit[1].SetRange(0, 1020);

 m_primecold_edit.SetLimitText(4);
 m_primecold_spin.SetBuddy(&m_primecold_edit);
 m_primecold_edit.SetDecimalPlaces(1);
 m_primecold_spin.SetRangeAndDelta(.0f, 100.0f, 0.1f);
 m_primecold_edit.SetRange(0, 100.0f);

 m_primehot_edit.SetLimitText(4);
 m_primehot_spin.SetBuddy(&m_primehot_edit);
 m_primehot_edit.SetDecimalPlaces(1);
 m_primehot_spin.SetRangeAndDelta(.0f, 100.0f, 0.1f);
 m_primehot_edit.SetRange(0, 100.0f);

 m_primedelay_edit.SetLimitText(4);
 m_primedelay_spin.SetBuddy(&m_primedelay_edit);
 m_primedelay_edit.SetDecimalPlaces(1);
 m_primedelay_spin.SetRangeAndDelta(.0f, 25.0f, 0.1f);
 m_primedelay_edit.SetRange(0, 25.0f);

 m_fldclrtps_edit.SetLimitText(5);
 m_fldclrtps_spin.SetBuddy(&m_fldclrtps_edit);
 m_fldclrtps_edit.SetDecimalPlaces(1);
 m_fldclrtps_spin.SetRangeAndDelta(.0f, 100.0f, 0.5f);
 m_fldclrtps_edit.SetRange(0, 100.0f);

 m_stblstrcnt_edit.SetLimitText(4);
 m_stblstrcnt_spin.SetBuddy(&m_stblstrcnt_edit);
 m_stblstrcnt_edit.SetDecimalPlaces(1);
 m_stblstrcnt_spin.SetRangeAndDelta(0, 255, 1);
 m_stblstrcnt_edit.SetRange(0, 255);

 UpdateData(FALSE);
 UpdateDialogControls(this, TRUE);

 //Create tooltip control
 mp_ttc.reset(new CToolTipCtrlEx());
 VERIFY(mp_ttc->Create(this, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON));

 VERIFY(mp_ttc->AddWindow(&m_smap_abandon_rpm_edit, MLL::GetString(IDS_PD_STARTER_SMAP_ABANDON_RPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_smap_abandon_rpm_spin, MLL::GetString(IDS_PD_STARTER_SMAP_ABANDON_RPM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_starter_off_rpm_edit, MLL::GetString(IDS_PD_STARTER_OFF_RPM_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_starter_off_rpm_spin, MLL::GetString(IDS_PD_STARTER_OFF_RPM_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_cranktoruntime_edit, MLL::GetString(IDS_PD_STARTER_CRANKTORUNTIME_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_cranktoruntime_spin, MLL::GetString(IDS_PD_STARTER_CRANKTORUNTIME_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_aftstrstr_edit[0], MLL::GetString(IDS_PD_STARTER_AFTSTRSTR_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_aftstrstr_spin[0], MLL::GetString(IDS_PD_STARTER_AFTSTRSTR_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_aftstrstr_edit[1], MLL::GetString(IDS_PD_STARTER_AFTSTRSTR_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_aftstrstr_spin[1], MLL::GetString(IDS_PD_STARTER_AFTSTRSTR_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_primecold_edit, MLL::GetString(IDS_PD_STARTER_PRIMECOLD_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_primecold_spin, MLL::GetString(IDS_PD_STARTER_PRIMECOLD_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_primehot_edit, MLL::GetString(IDS_PD_STARTER_PRIMEHOT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_primehot_spin, MLL::GetString(IDS_PD_STARTER_PRIMEHOT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_primedelay_edit, MLL::GetString(IDS_PD_STARTER_PRIMEDELAY_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_primedelay_spin, MLL::GetString(IDS_PD_STARTER_PRIMEDELAY_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_fldclrtps_edit, MLL::GetString(IDS_PD_STARTER_FLDCLRTPS_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_fldclrtps_spin, MLL::GetString(IDS_PD_STARTER_FLDCLRTPS_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_stblstrcnt_edit, MLL::GetString(IDS_PD_STARTER_STBLSTRCNT_EDIT_TT)));
 VERIFY(mp_ttc->AddWindow(&m_stblstrcnt_spin, MLL::GetString(IDS_PD_STARTER_STBLSTRCNT_EDIT_TT)));

 VERIFY(mp_ttc->AddWindow(&m_fldclrstart_check, MLL::GetString(IDS_PD_STARTER_FLDCLRSTART_CHECK_TT)));

 mp_ttc->SetMaxTipWidth(250); //Set width for text wrapping
 mp_ttc->ActivateToolTips(true);

 //initialize window scroller
 mp_scr->Init(this);

 return TRUE;  // return TRUE unless you set the focus to a control
}

void CStarterPageDlg::OnDestroy()
{
 Super::OnDestroy();
 mp_scr->Close();
}

void CStarterPageDlg::OnChangeData()
{
 UpdateData();
 OnChangeNotify(); //notify event receiver about change of view content(see class ParamPageEvents)
}

//Enable/disable controls (All)
void CStarterPageDlg::Enable(bool enable)
{
 if (m_enabled == enable)
  return; //already has needed state
 m_enabled = enable;
 if (::IsWindow(m_hWnd))
  UpdateDialogControls(this,TRUE);
}

//Get state of controls (enabled/disabled)
bool CStarterPageDlg::IsEnabled(void)
{
 return m_enabled;
}

//Use this function for getting values from dialog
void CStarterPageDlg::GetValues(SECU3IO::StartrPar* o_values)
{
 ASSERT(o_values);
 UpdateData(TRUE); //copy data from dialog to variables
 memcpy(o_values,&m_params, sizeof(SECU3IO::StartrPar));
}

//Use this function to set values in dialog
void CStarterPageDlg::SetValues(const SECU3IO::StartrPar* i_values)
{
 ASSERT(i_values);
 memcpy(&m_params,i_values, sizeof(SECU3IO::StartrPar));
 UpdateData(FALSE); //copy data from variables to dialog
}

void CStarterPageDlg::EnableFuelInjection(bool i_enable)
{
 if (m_fuel_injection == i_enable)
  return; //already has needed state
 m_fuel_injection = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CStarterPageDlg::EnableGasdose(bool i_enable)
{
 if (m_gasdose == i_enable)
  return; //already has needed state
 m_gasdose = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CStarterPageDlg::EnableChoke(bool i_enable)
{
 if (m_choke == i_enable)
  return; //already has needed state
 m_choke = i_enable;
 if (::IsWindow(this->m_hWnd))
  UpdateDialogControls(this, TRUE);
}

void CStarterPageDlg::OnSize( UINT nType, int cx, int cy )
{
 Super::OnSize(nType, cx, cy);

 DPIAware da;
 if (mp_scr.get())
  mp_scr->SetViewSize(cx, da.ScaleY(420));
}
